/*******************************************************************************
* Copyright (c) 2012-2014, The Microsystems Design Labratory (MDL)
* Department of Computer Science and Engineering, The Pennsylvania State University
* All rights reserved.
* 
* This source code is part of NVMain - A cycle accurate timing, bit accurate
* energy simulator for both volatile (e.g., DRAM) and non-volatile memory
* (e.g., PCRAM). The source code is free and you can redistribute and/or
* modify it by providing that the following conditions are met:
* 
*  1) Redistributions of source code must retain the above copyright notice,
*     this list of conditions and the following disclaimer.
* 
*  2) Redistributions in binary form must reproduce the above copyright notice,
*     this list of conditions and the following disclaimer in the documentation
*     and/or other materials provided with the distribution.
* 
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
* 
* Author list: 
*   Matt Poremba    ( Email: mrp5060 at psu dot edu 
*                     Website: http://www.cse.psu.edu/~poremba/ )
*******************************************************************************/

#include "MemControl/LAMOST/LAMOST.h"
#include "../AbstractDRAMCache.h"
#include "include/NVMHelpers.h"
#include "src/EventQueue.h"
#include "src/SubArray.h"
#include <iostream>
#include <set>
#include <assert.h>

using namespace NVM;

LAMOST::LAMOST( ): locks(*this), FQF(*this), NWB(*this)
{
  /* statistical parameters */
    rb_hits = 0;
    rb_miss = 0;
    drc_hits=0; 
    drc_miss=0;
    drc_evicts=0;
    drc_fills=0;
    drc_dirty_evicts=0;
    missedReq = 0;
    bypassedReq = 0;
    starvation_precharges = 0;
    
    /* Initialize two NVMTransactionQueues */
    InitQueues( 2 );

    functionalCache = NULL;
    useWriteBuffer = true;
    mainMemory = NULL;

    /* Alias */
    drcQueue = &(transactionQueues[0]);
 //   fillQueue = &(transactionQueues[1]);
}

LAMOST::~LAMOST( )
{
}

void LAMOST::SetConfig( Config */*conf*/, bool /*createChildren*/ )
{

}

void LAMOST::RegisterStats( )
{
    AddStat(missedReq);
    AddStat(bypassedReq);
    AddStat(rb_hits);
    AddStat(rb_miss);
    AddStat(rb_hitrate);   
    AddStat(drc_hits);       
    AddStat(drc_miss); 	  
    AddStat(drc_hitrate); 
    AddStat(drc_fills);  
    AddStat(drc_evicts);  
    AddStat(starvation_precharges);

    MemoryController::RegisterStats( );  // Register the statistics for the memory controllers
}

void LAMOST::SetMainMemory( NVMain *mm )
{
    mainMemory = mm;
}

 /* This following two functions are not used in the existing setup. Left for future references*/
void LAMOST::CalculateLatency( NVMainRequest *req, double *average, uint64_t *measured )
{
    (*average) = (( (*average) * static_cast<double>(*measured))
                    + static_cast<double>(req->completionCycle)
                    - static_cast<double>(req->issueCycle))
                 / static_cast<double>((*measured)+1);
    (*measured) += 1;
}

void LAMOST::CalculateQueueLatency( NVMainRequest *req, double *average, uint64_t *measured )
{
    (*average) = (( (*average) * static_cast<double>(*measured))
                    + static_cast<double>(req->issueCycle)
                    - static_cast<double>(req->arrivalCycle))
                 / static_cast<double>((*measured)+1);
    (*measured) += 1;
}

  /* Same as L3 and Tag Cache. May be used for Warmup */
bool LAMOST::IssueAtomic( NVMainRequest */*req*/ )
{
    return true;
}

  /* Unreferenced in the current setup */
bool LAMOST::IssueFunctional( NVMainRequest *req )
{
    uint64_t rank, bank;
    req->address.GetTranslatedAddress( NULL, NULL, &bank, &rank, NULL, NULL );

    return functionalCache[rank][bank]->Present( req->address );
}


bool LAMOST::IsIssuable( NVMainRequest * /*request*/, FailReason * /*fail*/ )
{
    bool rv = true;

   /* Limit the number of commands in the queue. This will stall the caches/CPU. */
   
    if( drcQueue->size( ) >= drcQueueSize )
    {
        rv = false;
    }

    return rv;
}

/* :) */
bool LAMOST::IssueCommand( NVMainRequest */*req*/ )
{
  return false;
}

 /* This function is implemented to check drc hit/miss from tag cache without coming to LAMOST */
bool LAMOST::PresentInCache(NVMainRequest */*req*/)
{
    return false;
}


bool LAMOST::RequestComplete( NVMainRequest */*req*/ )
{
   return false;
}


bool LAMOST::FillQueueFull::operator() ( NVMainRequest * /*request*/ )
{
  
  return false;
}

bool LAMOST::BankLocked::operator() ( NVMainRequest *request )
{
    bool rv = false;
    uint64_t bank, rank;
    request->address.GetTranslatedAddress( NULL, NULL, &bank, &rank, NULL, NULL );

    if( memoryController.bankLocked[rank][bank] == false
        && !memoryController.FQF( request ) )
        rv = true;

    return rv;
}

bool LAMOST::NoWriteBuffering::operator() ( NVMainRequest * /*request*/ )
{
    return !memoryController.useWriteBuffer;
}

void LAMOST::Cycle( ncycle_t steps)
{
  
    NVMainRequest *nextRequest = NULL;


    /* Check request queue. */
    if( FindStarvedRequest( *drcQueue, &nextRequest, locks ) )
    {
        rb_miss++;
        starvation_precharges++;
    }
    else if( FindRowBufferHit( *drcQueue, &nextRequest, locks ) )
    {
	rb_hits++;
    }
    else if( FindOldestReadyRequest( *drcQueue, &nextRequest, locks ) )
    {
        rb_miss++;
    }
    else if( FindClosedBankRequest( *drcQueue, &nextRequest, locks ) )
    {
        rb_miss++;
    }
    else
    {
	nextRequest = NULL;
    }

    // Issue any commands in the command queues
    CycleCommandQueues( ); /* Mind: why is it needed? */
    
    MemoryController::Cycle(steps);  /* Mind: same goes for this one, why we need it? */ 
}


void LAMOST::CalculateStats( )
{
	
    drc_hitrate = 0.0;  // Gogal: drc hit rate added for LH-Cache
    if( drc_hits+drc_miss > 0 )
        drc_hitrate = static_cast<float>(drc_hits) / static_cast<float>(drc_miss+drc_hits);

    rb_hitrate = 0.0;  // Gogal: row buffer hit rate added 
    if( rb_hits+rb_miss > 0 )
        rb_hitrate = static_cast<float>(rb_hits) / static_cast<float>(rb_miss+rb_hits);

    MemoryController::CalculateStats( );
}
