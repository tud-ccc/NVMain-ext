/*******************************************************************************
# Copyright 2018 TU Dresden
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# Authors: Asif Ali Khan
*
* Please cite the following paper if you are using this work.
* 
* Asif Ali Khan, Fazal Hameed, and Jeronimo Castrillon. 2018. NVMain Extension for Multi-Level Cache Systems.
* In Proceedings of the Rapido'18 Workshop on Rapid Simulation and Performance Evaluation: Methods and Tools (RAPIDO '18). ACM, New York, NY, USA, Article 7, 6 pages. 
*
*******************************************************************************/

#include "Caches/SRAMCache/SRAMCache.h"
#include "include/NVMHelpers.h"
#include "NVM/nvmain.h"
#include "Decoders/DRCDecoder/DRCDecoder.h"
#include "src/EventQueue.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <cstdlib>
#include <unistd.h>

//#define NOT_REQUIRED 1

using namespace NVM;

SRAMCache::SRAMCache( )
{
    src_hits = 0;
    src_misses = 0;
    src_evicts = 0;
    src_fills = 0;
    src_hitrate = 0;
    bypassedReq = 0;
    srcBanks = 0;
    srcSets = 0;
    missedReq = 0;
    srcAssoc = 0;
    stateTimer = 0;
    srcDataLatency = 1;
    srcTagLatency = 1;
    L3Cache = NULL;
    tcEnable = false;
        
}

SRAMCache::~SRAMCache( )
{

  delete []L3Cache;
  delete []adaptProbability;
 delete []policyCounter;
 delete []probCounter_pb;
 delete []probCounter_follower;
   
}

void SRAMCache::SetConfig( Config *conf, bool createChildren )
{
  
    std::cout<<"Inside SetConfig of SRAMCache.\n";
       
    /*************** L3 Cache Setup ***********************/
    uint64_t /*srcBanks,*/ srcSets, srcAssoc;

    if( conf->KeyExists( "L3Sets" ) )
        srcSets = static_cast<uint64_t>( conf->GetValue( "L3Sets" ) );
    else
       srcSets=32;
    
    if( conf->KeyExists( "L3Assoc" ) )
        srcAssoc = static_cast<uint64_t>( conf->GetValue( "L3Assoc" ) );
    else
       srcAssoc=1;

      if( conf->KeyExists( "L3QueueSize" ) )
        srcQueueSize = static_cast<uint64_t>( conf->GetValue( "L3QueueSize" ) );
    else
       srcQueueSize = 1;
        
    if( conf->KeyExists( "L3DALatency" ) ) /* L3 Data Access Latency */
        srcDataLatency = static_cast<uint64_t>( conf->GetValue( "L3DALatency" ) );
    else
       srcDataLatency = 1;
    
    if( conf->KeyExists( "L3TALatency" ) ) /* L3 Tag Access Latency */
        srcTagLatency = static_cast<uint64_t>( conf->GetValue( "L3TALatency" ) );
    else
       srcTagLatency = 1;
    
    if( conf->GetString( "SRC_ByPass" ) == "true" )
      SRAM_bypass = true;
    else
      SRAM_bypass = false;
         
    /* Decoder Setup. We are using the same decoder as used by DRC, but rows and banks are used as per src configuration. For the sake of uniformity, rows in SRC should be same as that in DRC */
    int rows, channels, ranks, cols, subarrays, banks, ignoreBits;
    
    AddressTranslator *srcAT = DecoderFactory::CreateDecoderNoWarn( conf->GetString( "Decoder" ) ); //This creates a DRC decoder
    srcAT->SetDefaultField( NO_FIELD );
    srcAT->SetConfig( conf, createChildren );
    SetDecoder( srcAT );
     
    banks = 1; /* L3 has only 1 bank */
    rows = srcSets; /* we use row only for the sake of clarity. Otherwise, it is redundant. */
    subarrays = 1;
    cols = 1;
    ranks = 1;
    channels = 1;
    srcCM = 1;	    

    TranslationMethod *srcMethod = new TranslationMethod();
    srcMethod->SetBitWidths( NVM::mlog2( rows ),
                                     NVM::mlog2( cols ),
                                     NVM::mlog2( banks ),
                                     NVM::mlog2( ranks ),
                                     NVM::mlog2( channels ),
                                     NVM::mlog2( subarrays )
                                     );
    srcMethod->SetCount( rows, cols, banks, ranks, channels, subarrays ); 
    srcMethod->SetAddressMappingScheme("R:C:BK:RK:CH:SA");
    srcAT->SetTranslationMethod( srcMethod  );
    
    
#ifdef NOT_REQUIRED  
    /* When selecting a child, use the channel field from a DRC decoder. */
    DRCDecoder *srcDecoder = new DRCDecoder( );
    srcDecoder->SetConfig( conf, createChildren );
    srcDecoder->SetTranslationMethod( srcMethod );
    srcDecoder->SetDefaultField( CHANNEL_FIELD );
      /* Set ignore bits for DRC decoder*/
      if( conf->KeyExists( "IgnoreBits" ) )
      {
      ignoreBits = conf->GetValue( "IgnoreBits" );
      srcDecoder->SetIgnoreBits(ignoreBits);
      }
    SetDecoder( srcDecoder );     
    
#endif    
 
    Config *config=conf;
    if( config->GetSimInterface( ) != NULL )
        config->GetSimInterface( )->SetConfig( conf, createChildren );
    else
      std::cout << "Warning: Sim Interface should be allocated before configuration!" << std::endl;
    
    /* L3 initialization */
    L3Cache = new CacheBank(srcSets, 1, srcAssoc, 64 ); /* rows, sets per row, Cache Lines/blocks per set, cach line/block size */
    L3Cache->SetCM( srcCM );
    
    if( createChildren )
    {
      if( (conf->GetString("TagCacheEnable")=="true") && ( conf->GetString("DRCVariant") != "LO_Cache" ) ) /* LO doesn't support Tag Cache */
      {
	tagCache=new TagCache();
	std::stringstream confString;
        confString.str( "" );     
        confString << "TagCache";
        tagCache->StatName( confString.str( ) );
	AddChild( tagCache );
	tagCache->SetParent( this );
	tagCache->SetConfig( conf, true );
	tcEnable = true;
      }
      else
      {
	nvmain=new NVMain();
	AddChild( nvmain );
	nvmain->SetParent( this );
	globalEventQueue->SetFrequency( conf->GetEnergy( "CPUFreq" ) * 1000000.0 );
	globalEventQueue->AddSystem( nvmain, conf );
	nvmain->SetConfig( conf,"defaultMemory", true );
	nvmain->PrintHierarchy( );
      }
	
    }
    
    /*
     * ByPass Configuration
     */
    if( conf->KeyExists( "numCores" ) )
    {
      numCores = static_cast<ncounter_t>( conf->GetValue( "numCores" ) );
    }
    else
    {
      numCores = 4;
    }
    
    adaptProbability = new uint64_t[numCores];  
    policyCounter = new uint64_t[numCores]; 
    probCounter_pb = new uint64_t[numCores];
    probCounter_follower = new uint64_t[numCores];
    
    for( uint64_t i = 0; i < numCores; i++ )
      {
	adaptProbability[i] = PA;  // Initialize to PA initially
     	policyCounter[i] = (-64 + 512); // Gogal: Initialized to PA
     	//probCounter_pa[i] = PB_INSERTION_RATE_INVERSE;  // Gogal: probCounter_pa not required
     	probCounter_pb[i] = PB_INSERTION_RATE_INVERSE;
     	probCounter_follower[i] = PB_INSERTION_RATE_INVERSE;
      }
    
    this->RegisterStats();
}

void SRAMCache::RegisterStats( )
{
    AddStat(src_hits);       
    AddStat(src_misses); 	  
    AddStat(src_hitrate); 
    AddStat(src_evicts);
    AddStat(missedReq);
    AddStat(bypassedReq);
    
//    AddStat(src_fills); 
}

bool SRAMCache::QueueFull( )
{
  bool rv = false;
  
  if( (srcQueue.size() >= srcQueueSize) )
    rv = true;
  
  return rv;
}

/* This function is called to Translate and Set Translated Address. We use Translated address in almost all requests. */
void SRAMCache::Retranslate( NVMainRequest *req )
{
    uint64_t col, row, bank, rank, chan, subarray;
    GetDecoder()->Translate( req->address.GetPhysicalAddress(), &row, &col, &bank, &rank, &chan, &subarray );
    req->address.SetTranslatedAddress( row, col, bank, rank, chan, subarray );
}


/* This function is used for Warmup. No Timing Information is needed. */
  
bool SRAMCache::IssueAtomic( NVMainRequest *req )
{
    NVMDataBlock dummy;
    
    /* set each memory partition as per the configuration */
    ( void )Retranslate( req );
    
#ifdef NOT_REQUIRED     
    std::cout<<"SRAMCache: Physical Row Col bank rank chan SubArray :\t"<<std::dec<<req->address.GetPhysicalAddress()<<"\t"<<std::dec<<req->address.GetRow()<<"\t"<<
    std::dec<<req->address.GetCol()<<"\t"<<req->address.GetBank()<<"\t"<<std::dec<<req->address.GetRank()<<"\t"<<std::dec<<req->address.GetChannel()<<"\t"<<std::dec<<req->address.GetSubArray()
    <<std::endl;
#endif 
    
    if( L3Cache->Present( req->address ) )
    {
      if( req->type == WRITE )
	L3Cache->Write( req->address, req->data); /* Do not install request. Update MRU Information and set the dirty bit */
      else
	L3Cache->Read( req->address, &dummy );
      
    //  src_hits++;
    }
    else
    {
      if( L3Cache->SetFull( req->address ) ) 
      {
	NVMAddress victim;
        (void)L3Cache->ChooseVictim( req->address, &victim );
        (void)L3Cache->Evict( victim, &dummy );
      }

      (void)L3Cache->Install( req->address, dummy );
     // src_misses++;
    }
    
    if( tcEnable )
      tagCache->IssueAtomic( req );
    else
      nvmain->IssueAtomic( req );
    
    return true;
    
}

bool SRAMCache::IsIssuable( NVMainRequest *request, FailReason * /*fail*/ )
{
    bool rv = false;
  
    if( !QueueFull() ) /* Queue is full. so request is not issuable. */
    {
      if( tcEnable )
      {
	if( tagCache->IsIssuable(request) )
	  rv = true;	  
      }
      else
      {
	if( nvmain->drcReady(request) )
	  rv = true;
      }
    }
    
//      if( tcEnable && tagCache->IsIssuable(request) == false )
//        rv = false;
     
    return rv;
}



/* Not Used in our case. Left for future references */
bool SRAMCache::IssueFunctional( NVMainRequest *req )
{
  ( void )Retranslate( req ); //Doesn't needed though. Only for the sake of correctness
  
  return L3Cache->Present( req->address );
}


bool SRAMCache::IssueCommand( NVMainRequest *req )
{  
  
  // std::cout<<"In IssueCommand of L3: Address: "<<std::hex<<req->address.GetPhysicalAddress()<<std::dec<<std::endl;
  bool rv = false;
  NVMainRequest *srcReq = new NVMainRequest();
  srcReq->address = req->address;
  srcReq->data = req->data;
  srcReq->type = req->type;
  srcReq->owner = this;
  srcReq->arrivalCycle = GetEventQueue()->GetCurrentCycle();
  srcReq->reqInfo = static_cast<void *>( req ); //save the original Request
  ( void )Retranslate( srcReq );
  
  if( !QueueFull() )
  {
    srcQueue.push( srcReq );
    rv = true;
  }
  else
  {
    missedReq++;
  }
  
  return rv;
}
  
 
bool SRAMCache::RequestComplete( NVMainRequest *srcReq )
{
  bool rv = false;
  bool hit = false;
  
  if( srcReq->owner == this )
  {
   //   std::cout<<"Request Complete of L3"<<std::endl;
      
      if( srcReq->tag == SRC_DIRTY_EVICT ) /* Dirty Evict request sent to child has returned. */
      {
	
	delete srcReq;
	return true;
	
      }
      else
      {
	  hit = L3Cache->Present( srcReq->address );      

	  switch( srcReq->type )
	  {
	      case READ:
	      {
		  if( hit )
		    L3Cache->Read( srcReq->address, &(srcReq->data) ); /* Update MRU Information */
		  else
		  {
		    /* Mind:: Forward original Request to next level. Where will it come back? to SRC or top module? It goes to top module via SRC  */
		    NVMainRequest *req;
		    req = static_cast<NVMainRequest *>( srcReq->reqInfo );
		    req->tag = SRC_FILL;   
		 
		    /* Request type is read and it is not Present in SRC, therefore it is issued to the next level*/
		    drcWaitQueue.push( req );
		   		    
		  }
		  break;
	      }
	      case WRITE:
	      {
		  if( hit )
		    L3Cache->Write( srcReq->address, srcReq->data ); /* Replace the existing block and set dirty bit */
		  //else
		  //{
		    
		    // we have to the fill srcReq in L3. Doing outside switch statement(only for the sake of optimization)
		  //}
		  break;
	      }
	      default:
		// std::cout<<" Error! Unknown Request type "<<req->type<<"!\n";
		  break;
	   }
	    
	    
	   if( hit )
	   {
	      src_hits++;
	//      std::cout<<"SRC Hits & Misses: "<<src_hits<<"\t"<<src_misses<<std::endl;
	      NVMainRequest *originalReq = static_cast<NVMainRequest *>( srcReq->reqInfo );
	      GetParent()->RequestComplete( originalReq ); /* Inform the Parent about Request Completion */
	   }
	   else  /* we have to install the request here and see for dirty evict */
	   {
		src_misses++;
	     
		if( /*srcReq->tag == SRC_FILL &&*/ SRAM_bypass && BypassLevel( srcReq->address ) )
		{
		  bypassedReq++;
		}
		else
		{
		    NVMAddress victim;
		    NVMDataBlock victimData;
		    bool dirty = false;
		    
		    if( L3Cache->SetFull( srcReq->address ) )
		    {
		      dirty = L3Cache->ChooseVictim( srcReq->address, &victim );
		      
		      ( void )L3Cache->Evict( victim, &victimData );
		      src_evicts++;
		    }
		    
		    ( void )L3Cache->Install( srcReq->address, srcReq->data );
		    
		    if( dirty )
		    {
		      NVMainRequest *dirtyEvict = new NVMainRequest( );
		      dirtyEvict->address = victim;
		      dirtyEvict->data = victimData;
		      dirtyEvict->owner = this;
		      dirtyEvict->type = WRITE;
		      dirtyEvict->tag = SRC_DIRTY_EVICT;
		      dirtyEvict->arrivalCycle = GetEventQueue( )->GetCurrentCycle( );
		      
		      drcWaitQueue.push( dirtyEvict );
		    }
		
	
		 }
	   }
      
	   delete srcReq;
	   rv = true;
      } /* end of block A*/
  }
  else
  {
    rv = false;
    GetParent( )->RequestComplete( srcReq );
  }
  
  return rv;
  
}

void SRAMCache::Cycle( ncycle_t steps)
{
  NVMainRequest *srcReq, *drcReq;
  
  srcReq = NULL;
  
  /* Issue SRC commands */
  if( !srcQueue.empty( ) ) 
  {
    srcReq = srcQueue.front(); /* This is not the original req but since we need only the address, it is the same in both (we don't need to extract back the orignal) */
    srcQueue.pop( );
   
 //   bool hit = L3Cache->Present( srcReq->address ); /* Hit/Miss Information */
    
//    if( hit )
//      stateTimer = GetEventQueue( )->GetCurrentCycle( ) + srcTagLatency + srcDataLatency; /* Request responds (completes) in this much time */
//    else
//      stateTimer = GetEventQueue( )->GetCurrentCycle( ) + srcTagLatency;
    
    stateTimer = 5;
    
    GetEventQueue( )->InsertEvent( EventResponse, this, srcReq, stateTimer ); /* sent to the RequestComplete{bcz eventType is EventResponse: Source: EventQueue.cpp} of this (after read/write delay), which returns it RequestComplete of SRC */
 
   // this->RequestComplete( srcReq );
  }
  
  if( !drcWaitQueue.empty() )
  {
    drcReq = drcWaitQueue.front();
    
    if( tcEnable )
    {
      
	if ( tagCache->IsIssuable( drcReq ) )
	{
	  tagCache->IssueCommand ( drcReq );
	  drcWaitQueue.pop(); // Remove the request from the queue, since it has been issued
	}  
	  
	tagCache->Cycle( steps );
    }
    else
    {
      if ( nvmain->drcReady( drcReq ) )
	{
	  nvmain->IssueCommand ( drcReq );
	  drcWaitQueue.pop(); // Remove the request from the queue, since it has been issued
	}  
    
	nvmain->Cycle( steps );
    }
  }
}

void SRAMCache::CalculateStats( )
{
   src_hitrate = 0.0;  // Gogal: drc hit rate added for LH-Cache
   if( src_hits+src_misses > 0 )
     src_hitrate = static_cast<float>(src_hits) / static_cast<float>(src_misses+src_hits);
   
   if(tcEnable)
      tagCache->CalculateStats( );
   else
      nvmain->CalculateStats( );
}

bool SRAMCache::BypassLevel(NVMAddress& addr)
{
      bool byPass = false;
      uint16_t setType, coreID;
      bool change_policy = false; 
      
      uint64_t setID = addr.GetRow();
      
      setType = FindSetType( setID, coreID );
      
      if( setType == FollowerSet ) //Follower Set
      {
	  if( adaptProbability[coreID] == PA ) //Follower set probability is equivalent to set0 probability. Check Set0 Prob, if it is PA, don't bypass else check counter value
	    {
	     // byPass = false; //Don't bypass. False is defult value so don't need to re-assign it
	    }
	    else // Policy for Follower is PB i.e. insertion rate is 1 / 64
	    {
		if( probCounter_follower[coreID] == 1 )
		{
		 // byPass = false; //Don't Bypass.
		  probCounter_follower[coreID] = PB_INSERTION_RATE_INVERSE;
		}
		else
		{
		  byPass = true;
		}
		
		probCounter_follower[coreID]--;
	    }
      }
      else if( setType == SS0 )
      {
	  //Leave ByPass false. Don't Bypass. 
	  if( policyCounter[coreID] < 1024 ) // We are assuming 7 bit counter that can have maximum value of 127 (0-127)
	      policyCounter[coreID]++;
      }
      else // SetType is SS1 
      {
	  if( policyCounter[coreID] > 0 ) // We are assuming 7 bit counter that can have minimum value of 0 (0-127)
	      policyCounter[coreID]--;
	  
	  if( probCounter_pb[coreID] == 1 )
	  {
	     // byPass = false; //default selected.
	     probCounter_pb[coreID] = PB_INSERTION_RATE_INVERSE;
	  }
	  else
	     byPass = true;

	  probCounter_pb[coreID]--;   
      }
      
      /* Update the policy */
      
      if( policyCounter[coreID] < (-32 + 512) && adaptProbability[coreID] == PB)
      { 
	  adaptProbability[coreID] = PA; 
	  change_policy = true;
      }
      else if( ( policyCounter[coreID] > (32 + 512) ) && ( adaptProbability[coreID] == PA ) )
      { 
	  adaptProbability[coreID] = PB; 
	  change_policy = true;
      }
      
	
   
      #ifdef DEBUG
      if( change_policy == true )
      {
       // std::cout << sim_cycle ;
       std::cout << "core_id= " << coreID  "follower policy = " << adaptProbability[coreID] << endl;    
      }      
      #endif    
      
      return byPass;
}


int SRAMCache::FindSetType(uint64_t setID, uint16_t& coreID)
{
  
    uint16_t setypte = FollowerSet; 
    uint16_t temp = setID % SAMPLING_FREQUENCY;
    coreID = 0;
    
    if( ( temp < 2 * numCores ) )
    {
        coreID = temp / 2;
	
        if ( temp % 2 == 0 )
	{
	    setypte = SS0;   
	}
	else
	{
	    setypte = SS1;  
	}
            
    }
    
    //std::cout<< "ByPass Values: numCores: setID: coreId: settype: "<<numCores<<"\t"<<setID<<"\t"<<coreID<<"\t"<<setypte<<std::endl;
    return setypte;  
  
}

