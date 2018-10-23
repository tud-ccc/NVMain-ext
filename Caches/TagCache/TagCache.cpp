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

#include "Caches/TagCache/TagCache.h"


//#define NOT_REQUIRED

using namespace NVM;

TagCache::TagCache( )
{
//    tagCacheFills = 0;
    tagCacheHits = 0;
    tagCacheMisses = 0;
    tagCacheEvicts = 0;
    tagcache_hitrate = 0;
}

TagCache::~TagCache( )
{
    uint64_t r;

    for( r = 0; r < tcSets; r++ )
    {
      delete [] tagCache[r];
    }

    delete [] tagCache;
    
    if(nvmain)
      delete nvmain;
}

void TagCache::SetConfig( Config *conf, bool createChildren )
{
    this->config = conf;

    /*************** TagCache Setup ***********************/
    
    if( conf->KeyExists( "TagCacheSets" ) )
        tcSets = static_cast<uint64_t>( conf->GetValue( "TagCacheSets" ) );
    else
        tcSets=32;

   // tcAssoc = 16;
    if( conf->KeyExists( "TagCacheAssoc" ) )
        tcAssoc = static_cast<uint64_t>( conf->GetValue( "TagCacheAssoc" ) );
    else
        tcAssoc=1;

    tagCache = new TagCacheEntry *[tcSets];	//tcSets = Number of Rows in Tag Cache
    
    for (uint64_t i = 0; i < tcSets; i++)
    {
      tagCache[i] = new TagCacheEntry [tcAssoc];
      for( uint64_t j = 0; j < tcAssoc; j++ )
      {
	tagCache[i][j].row = 0;
	tagCache[i][j].bank = 0;
	tagCache[i][j].rank = 0;
	tagCache[i][j].chan = 0;
	tagCache[i][j].flags = TCACHE_ENTRY_NONE;
      }
    }
    
    /* Decoder Setup! We are using the same setup as that of DRC */
    
    int channels, ranks, banks, rows, cols, subarrays, ignoreBits, cm;
    
//     AddressTranslator *tcAT = DecoderFactory::CreateDecoderNoWarn( conf->GetString( "Decoder" ) ); //This creates a DRC decoder
//     tcAT->SetDefaultField( NO_FIELD );
//     tcAT->SetConfig( conf, createChildren );
//     SetDecoder( tcAT );
//      
     if( conf->KeyExists( "MATHeight" ) )
     {
         rows = conf->GetValue( "MATHeight" );
         subarrays = conf->GetValue( "ROWS" ) / conf->GetValue( "MATHeight" );
     }
     else
     {
         rows = conf->GetValue( "ROWS" );
         subarrays = 1;
     }
   // subarrays = 1;
    //cols = conf->GetValue( "COLS" );
    cm = static_cast<ncounter_t>( conf->GetValue( "CM" ) );
    cols = 4 * cm;
    
    banks = conf->GetValue( "BANKS" );
    ranks = conf->GetValue( "RANKS" );
    channels = conf->GetValue( "DRC_CHANNELS" );    

    TranslationMethod *tcMethod = new TranslationMethod();
    tcMethod->SetBitWidths( NVM::mlog2( rows ),
                                     NVM::mlog2( cols ),
                                     NVM::mlog2( banks ),
                                     NVM::mlog2( ranks ),
                                     NVM::mlog2( channels ),
                                     NVM::mlog2( subarrays )
                                     );
    tcMethod->SetCount( rows, cols, banks, ranks, channels, subarrays );
    tcMethod->SetAddressMappingScheme(conf->GetString("AddressMappingScheme"));
    
    //tcMethod->SetAddressMappingScheme("R:C:BK:RK:CH:SA");
    //tcAT->SetTranslationMethod( tcMethod  );
    
    DRCDecoder *drcDecoder = new DRCDecoder( );
    drcDecoder->SetConfig( config, createChildren );
    drcDecoder->SetTranslationMethod( tcMethod );
    drcDecoder->SetDefaultField( CHANNEL_FIELD );
    
    /* Set ignore bits for DRC decoder*/
    if( conf->KeyExists( "IgnoreBits" ) )
    {
       ignoreBits = conf->GetValue( "IgnoreBits" );
       drcDecoder->SetIgnoreBits(ignoreBits);
    }

    SetDecoder( drcDecoder );
 
    Config *config=conf;
    if( config->GetSimInterface( ) != NULL )
        config->GetSimInterface( )->SetConfig( conf, createChildren );
    else
      std::cout << "Warning: Sim Interface should be allocated before configuration!" << std::endl;
    
    /* Mind: Why not DRAMCache directly? It does the same (apparently) */    
    nvmain = new NVMain();

   
    if( createChildren )
    {
      
      AddChild( nvmain );
      nvmain->SetParent( this );
      globalEventQueue->AddSystem( nvmain, config ); 
      nvmain->SetConfig( config, "defaultMemory", true );
      nvmain->PrintHierarchy( ); 	//Displays the whole hirarichy that we get in the output
    }
 
    this->RegisterStats();
    
     std::cout << "Created a TagCache!" << std::endl;
}

void TagCache::RegisterStats( )
{
    AddStat( tagCacheHits );
    AddStat( tagCacheMisses );
    AddStat( tagcache_hitrate );
    AddStat( tagCacheEvicts );
//    AddStat( tagCacheFills ); //becasue misses and fills are same here
}


void TagCache::Retranslate( NVMainRequest *req )
{
    uint64_t col, row, bank, rank, chan, subarray;
    GetDecoder()->Translate( req->address.GetPhysicalAddress(), &row, &col, &bank, &rank, &chan, &subarray );
    
    req->address.SetTranslatedAddress( row, col, bank, rank, chan, subarray );
}

/**** Used For TagCache Warmup ********/

bool TagCache::IssueAtomic( NVMainRequest *req )
{
	( void )Retranslate( req ); 
	
#ifdef NOT_REQUIRED
    std::cout<<"TagCache: PA Col SA CH bank Row Rank :\t"<<std::dec<<req->address.GetPhysicalAddress()<<"\t"<<req->address.GetCol()<<"\t"<<
    std::dec<<req->address.GetSubArray()<<"\t"<<req->address.GetChannel()<<"\t"<<std::dec<<req->address.GetBank()<<"\t"<<std::dec<<req->address.GetRow()<<"\t"<<std::dec<<
    req->address.GetRank()<<std::endl;
#endif	
	
	
	if( tcPresent( req->address ) )
	{
	    UpdatePosition( req ); //If request address is Present in TagCache, update its MRU info and set the DIRTY bit
	//    tagCacheHits++;
	}
	else
	{
	      /* If there is no space, toss something. */
	    if( tcSetFull( req ) )
	    {
		NVMAddress victim;
		( void )tcChooseVictim( req, &victim );
		( void )tcEvict( victim );
	    }
	//    tagCacheMisses++;

	    /* we are not recording any statistics (such as evicts, fills and hits/misses etc.) because it is only Warmup */
	    ( void )tcInstall( req );   
	}

	/* Mind: Return the request to the parent (to delete it) as it has done its job.
	   Alternate, we can delete it here. 
	   For DRC Warmup, we have to call IssueAtomic of the child here and comment the GetParent()->RequestComplete() 
	*/
	
	GetChild()->IssueAtomic( req );
    
 return true;
}


bool TagCache::IsIssuable( NVMainRequest *request, FailReason * /*fail*/ )
{
    bool rv = false;
    
    rv = GetChild()->drcReady( request );
     
    return rv;
}

bool TagCache::IssueCommand( NVMainRequest *req)
{
 
  // std::cout<<"issue Command tag cache Called"<<std::endl;
   bool tchit = false;
   bool drchit = false;
   NVMainRequest *drcRequest;  /* drcRequest, after updation, will be forwarded to DRC */
   
   ( void )Retranslate( req ); //Address is Retranslated once for every request as translated address is used in almost all functions
      
   tchit = tcPresent( req->address ); 
   drchit=GetChild()->drcPresent( req ); /* Extract the drcPresent information beforehand */
   
   drcRequest = req;
  
   /* SRC Evict and not Present in Tag Cache. If Present in tag cache, only position is updated as happens to every other request */
   if( !tchit && req->tag == SRC_DIRTY_EVICT )
   {
     GetChild()->IssueCommand( req );
     return true;
   }
   
   /* Here are the scenarios: 
    * if tag cache is hit and drc is hit, then we only access the data (setting tag to DRC_ACCESS)
    * if tag cache is miss and drc is hit, then we need to read the tags and data (setting tag to DRC_TAGREAD)
    * if drc is miss (irrespective of the tagcache hit / miss), we read from memory (setting tag to DRC_MEMACCESS)
    */
   
   
   if( tchit )
   {  
      if( drchit )
      {
	/* we have to read data (no tags) from DRC */
	drcRequest->tag = DRC_DATA_ACCESS;  /* Don't need to read tags, only data may be accessed */
      }
      else
      {
	drcRequest->tag = DRC_MEMACCESS_TCHIT;  //This implicitly generates a new request for DRC Fill and set type to write. Since the tag is already present in TC, tag latency should be avoided
      }
      
      if( req->tag == SRC_DIRTY_EVICT )
      {
	drcRequest->tag = SRC_DIRTY_EVICT_TCHIT;
      }
      
      tagCacheHits++;
      UpdatePosition( req ); /* Update LRU information and make flags Dirty */
    }
    
    else
    {
	//Missed in TagCache 
	bool dirtyEvict = false;
      
	if( drchit )
	{
	  drcRequest->tag = DRC_TAGREAD;
	  // Gogal: Set the 
	  //drcRequest->type = READ;
	}
	else
	  drcRequest->tag = DRC_MEMACCESS;
	
	//Installing the missed request in tag cache 
	
	if( tcSetFull( req ) )
	{
	  
	  NVMAddress victim;
	  ( void )tcChooseVictim( req, &victim );
	  dirtyEvict = tcEvict( victim );
	  
	  ( void )tcInstall( req );
	//  tagCacheFills++;
	
	  // Dirty! Write back here
	  
	  if( dirtyEvict )
	  {
	    NVMainRequest *drcTCEvictReq = new NVMainRequest( ); /* This is a subrequest. we have to delete it here (DTC) */
	    drcTCEvictReq->address = victim;
	    drcTCEvictReq->tag= TC_EVICT;		/* We have to handle TC_EVICT in LAMOST? No, because it is handled already because req->type is write? we will see */
	    drcTCEvictReq->type = WRITE;
	    drcTCEvictReq->owner=this;
	    tagCacheEvicts++;
	    GetChild()->IssueCommand( drcTCEvictReq );
	  }
	  
	}
	else /* tagcache set is not full */
	{
	  ( void )tcInstall( req );
//	  tagCacheFills++;
	}
	
	tagCacheMisses++;     
    }

    //drcRequest->owner = this; /* drcRequest is a pointer to req, which is received from a previous module. so, owner is not this. */
    GetChild()->IssueCommand( drcRequest ); 
 
 /* Mind: Theoratically, we should delete req here but we can't as drcRequest is a pointer to it. deleteing req will invalidate drcRequest */
 
    return true;
}


bool TagCache::RequestComplete( NVMainRequest *req )
{

   bool rv = false;
      
   if( req->owner == this )
    {
        rv = true;
        delete req;
    }
    else
    {
     rv = GetParent( )->RequestComplete( req );
    }

    return rv;
}


void TagCache::CalculateStats( )
{
  if( tagCacheHits+tagCacheMisses > 0 )
     tagcache_hitrate = static_cast<float>(tagCacheHits) / static_cast<float>(tagCacheHits+tagCacheMisses);
  
  GetChild( )->CalculateStats( ); /* CalculateStats of DRC */
}

void TagCache::Cycle( ncycle_t /*steps*/ )
{
  /*This is called for sure. should I call the drc cycle here? it is called already becasue requests are in globalEventQueue */
}



/****************** TagCache Specific Functions ************************
 * 
 * All read, write update etc related to tag cache are implemented here
 *   
 **********************************************************************/

uint32_t TagCache::tcFindSet( NVMAddress& addr )
{
   uint64_t row;
   addr.GetTranslatedAddress( &row, NULL, NULL, NULL, NULL, NULL );
   
   uint32_t setID = row % tcSets;
   return setID;
}

bool TagCache::tcSetFull( NVMainRequest *req )
{
    uint32_t set = tcFindSet( req->address );
    bool rv = true;

    for( uint64_t i = 0; i < tcAssoc; i++ )
    {
        /* If there is an invalid entry (e.g., not used) the set isn't full. */
        if( !(tagCache[set][i].flags & TCACHE_ENTRY_VALID) )
        {
            rv = false;
            break;
        }
    }
    
    return rv;
}

bool TagCache::tcInstall( NVMainRequest *req )
{
  uint32_t set = tcFindSet( req->address );
  bool rv = false;
  uint64_t rank, bank, row, chan, sArray;
  req->address.GetTranslatedAddress ( &row, NULL, &bank, &rank, &chan, &sArray );
  
  for( uint64_t i = 0; i < tcAssoc; i++ )
  {
        if( !(tagCache[set][i].flags & TCACHE_ENTRY_VALID))
        {
          tagCache[set][i].bank = bank;
	  tagCache[set][i].rank = rank;
	  tagCache[set][i].chan = chan;
	  tagCache[set][i].row = row;
	  tagCache[set][i].subarray = sArray;
	  tagCache[set][i].address = req->address;
	  tagCache[set][i].flags |= TCACHE_ENTRY_VALID; 
          rv = true;
          break;
        }
    }
    
  return rv;
}

bool TagCache::tcPresent( NVMAddress& Addr )
{
 
  uint32_t set = tcFindSet( Addr );
  bool found = false;
  uint64_t rank, bank, row, chan, sArray;
  Addr.GetTranslatedAddress ( &row, NULL, &bank, &rank, &chan, &sArray );

   
  for( uint64_t i = 0; i < tcAssoc; i++ )
  {
    if( tagCache[set][i].row == row && tagCache[set][i].bank == bank &&
	tagCache[set][i].chan == chan && tagCache[set][i].rank == rank && tagCache[set][i].subarray == sArray &&
	(tagCache[set][i].flags & TCACHE_ENTRY_VALID) )
    {
       found = true;
       break;
    }
  }
 
  return found;
}


bool TagCache::tcChooseVictim( NVMainRequest *req, NVMAddress *victim )
{
   bool rv = false;
   uint32_t set = tcFindSet( req->address );

   *victim = tagCache[set][tcAssoc-1].address; 
   
   if( tagCache[set][tcAssoc-1].flags & TCACHE_ENTRY_DIRTY )
       rv = true;

    return rv;
}


bool TagCache::tcEvict( NVMAddress& victim_addr )
{
    bool rv = false;
    uint64_t row;
    victim_addr.GetTranslatedAddress ( &row, NULL, NULL, NULL, NULL, NULL ); /*R, Col, B, RK, CH, SA */
    uint32_t set = row % tcSets; /* tcFindSet is not called because that takes NVMainRequest and we have NVMAddress */

    for( uint64_t i = 0; i < tcAssoc; i++ )
    { 
      if( /*tagCache[set][i].row == row && tagCache[set][i].bank == bank &&
	  tagCache[set][i].chan == chan && tagCache[set][i].rank == rank &&*/
	  tagCache[set][i].address.GetPhysicalAddress() == victim_addr.GetPhysicalAddress() &&
	  (tagCache[set][i].flags & TCACHE_ENTRY_VALID) )			
        {
            if(tagCache[set][i].flags & TCACHE_ENTRY_DIRTY )
            {
                rv = true;
            }
            else
            {
                rv = false;
            }

            tagCache[set][i].flags = TCACHE_ENTRY_NONE;

            break;
        }
    }

    return rv;
}

bool TagCache::UpdatePosition( NVMainRequest *req )
{
 
    uint32_t set = tcFindSet( req->address );
    bool rv = false;
    uint64_t rank, bank, row, chan, sArray;
    req->address.GetTranslatedAddress ( &row, NULL, &bank, &rank, &chan, &sArray );
    
    for( uint64_t i = 0; i < tcAssoc; i++ )
    {
	if( tagCache[set][i].row == row && tagCache[set][i].bank == bank &&
	    tagCache[set][i].chan == chan && tagCache[set][i].rank == rank && tagCache[set][i].subarray == sArray &&
	    (tagCache[set][i].flags & TCACHE_ENTRY_VALID) )
	  {
		/* Move cache entry to MRU position */
	      TagCacheEntry tmp;

	      tmp.flags = tagCache[set][i].flags;
	      tmp.row  = tagCache[set][i].row;
	      tmp.bank = tagCache[set][i].bank;
	      tmp.rank = tagCache[set][i].rank;
	      tmp.chan = tagCache[set][i].chan;
	      tmp.subarray = tagCache[set][i].subarray;
	      tmp.address = tagCache[set][i].address;

	      for( uint64_t j = i; j >= 1; j-- )  /* In the source code, the condition was j > 1 which is not moving the contents of location 0*/
	      {
		 tagCache[set][j].flags = tagCache[set][j-1].flags;
		 tagCache[set][j].row = tagCache[set][j-1].row;
		 tagCache[set][j].bank = tagCache[set][j-1].bank;
		 tagCache[set][j].rank = tagCache[set][j-1].rank;
		 tagCache[set][j].chan = tagCache[set][j-1].chan;
		 tagCache[set][j].subarray = tagCache[set][j-1].subarray;
		 tagCache[set][j].address = tagCache[set][j-1].address;
	      }
	      
	     tagCache[set][0].flags |= TCACHE_ENTRY_DIRTY;;
	     tagCache[set][0].row = tmp.row;
	     tagCache[set][0].bank = tmp.bank;
	     tagCache[set][0].rank = tmp.rank;
	     tagCache[set][0].chan = tmp.chan;
	     tagCache[set][0].subarray = tmp.subarray;
	     tagCache[set][0].address = tmp.address;
	     
	     rv = true;
	  }
    }

    return rv;
}

void TagCache::PrintTagCache(  )
{
  std::cout<< "********************************Tag Cache Map (Format: Row.Bank.Rank.Chan.flags)*************************************\n";
  
  
  for( uint64_t i = 0; i < tcSets; i++ )
  {
    for( uint64_t j = 0; j < tcAssoc; j++ )
      std::cout<<std::dec<<tagCache[i][j].row<<"."<<std::dec<<tagCache[i][j].bank<<"."<<std::dec<<tagCache[i][j].rank<<"."<<std::dec<<tagCache[i][j].chan<<"."<<tagCache[i][j].flags<<"\t";
  
    std::cout<<std::endl;
  }
  
    std::cout<< "****************************************Tag Cache End*********************************************************\n";
}

