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