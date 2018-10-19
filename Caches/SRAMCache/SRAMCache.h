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

#ifndef __MEMCONTROL_SRAMCACHE_H__
#define __MEMCONTROL_SRAMCACHE_H__

#include "src/MemoryController.h"
#include "Caches/CacheBank/CacheBank.h"
#include "Caches/TagCache/TagCache.h"
#include "MemControl/DRAMCache/DRAMCache.h"
#include "src/AddressTranslator.h"
#include "MemControl/LAMOST/LAMOST.h"

namespace NVM {

#define SRC_READ        tagGen->CreateTag("SRC_READ")
#define SRC_WRITE       tagGen->CreateTag("SRC_WRITE")

#define SS0 0
#define SS1 1
#define FollowerSet 2

#define PA 1  
#define PB 64 
#define PB_INSERTION_RATE_INVERSE 64
#define NUM_POLICIES 2  
#define SAMPLING_FREQUENCY 128    
  
  
class NVMain;

class SRAMCache : public NVMObject
{
  public:
    SRAMCache( );
    ~SRAMCache( );
    
    void SetConfig( Config *conf, bool createChildren = true );
    void Retranslate( NVMainRequest *req );
    bool QueueFull();
    bool IssueAtomic( NVMainRequest *req );
    bool IssueCommand( NVMainRequest *req );
    bool IsIssuable( NVMainRequest *request, FailReason *reason = NULL );
    bool IssueFunctional( NVMainRequest *req );
    bool RequestComplete( NVMainRequest *req );
    void RegisterStats( );
    void Cycle( ncycle_t );
    void CalculateStats( );
    bool BypassLevel(NVMAddress& addr );
    int FindSetType(uint64_t setID, uint16_t& core );
    
private:
    ncounter_t src_hits, src_misses;
    ncounter_t src_evicts, src_fills;
    ncounter_t src_dirty_evicts;
    ncounter_t missedReq, bypassedReq;
    ncounter_t numCores;
    double src_hitrate, src_missrate;
    uint64_t srcBanks, srcSets, srcAssoc, srcCM;
    uint64_t srcDataLatency, srcTagLatency;
    uint64_t stateTimer;
    uint64_t srcQueueSize;
    TagCache *tagCache;
    DRAMCache *DRC;
    CacheBank *L3Cache;
    NVMain *nvmain;
    bool SRAM_bypass;
     
    uint64_t *policyCounter,*probCounter_pb, *probCounter_follower, *adaptProbability;
    
    bool tcEnable;
    std::queue<NVMainRequest *>/*NVMTransactionQueue*/ srcQueue;
    std::queue<NVMainRequest *> drcWaitQueue;
};

};

#endif