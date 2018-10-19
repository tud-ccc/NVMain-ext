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


#ifndef __MEMCONTROL_LAMOST_H__
#define __MEMCONTROL_LAMOST_H__

#include "Caches/CacheBank/CacheBank.h"
#include "MemControl/DRAMCache/DRAMCache.h"
#include "NVM/nvmain.h"

namespace NVM {

#define DRC_TAGREAD tagGen->CreateTag("DRC_TAGREAD")
#define DRC_MEMREAD  tagGen->CreateTag("DRC_MEMREAD")
#define DRC_FILL     tagGen->CreateTag("DRC_FILL")
#define DRC_WRITE_FILL     tagGen->CreateTag("DRC_WRITE_FILL")
#define DRC_DATA_ACCESS   tagGen->CreateTag("DRC_DATA_ACCESS")
#define DRC_MEMACCESS   tagGen->CreateTag("DRC_MEMACCESS")
#define DRC_MEMACCESS_TCHIT   tagGen->CreateTag("DRC_MEMACCESS_TCHIT")
//#define DRC_MEMREAD_TCHIT   tagGen->CreateTag("DRC_MEMREAD_TCHIT")
#define DRC_EVICT   tagGen->CreateTag("DRC_EVICT")
#define TC_EVICT   tagGen->CreateTag("TC_EVICT")
#define SRC_DIRTY_EVICT   tagGen->CreateTag("SRC_DIRTY_EVICT")
#define SRC_DIRTY_EVICT_TCHIT   tagGen->CreateTag("SRC_DIRTY_EVICT_TCHIT")
#define SRC_FILL   tagGen->CreateTag("SRC_FILL")
#define DRC_TAGREAD_WITHOUT_DATA   tagGen->CreateTag("DRC_TAGREAD_WITHOUT_DATA") 
  
class NVMain;

class LAMOST : public AbstractDRAMCache 
{
  friend class DRAMCache;
  public:
    LAMOST( );
    virtual ~LAMOST( );

    void SetConfig( Config *conf, bool createChildren = true );
    void SetMainMemory( NVMain *mm );
    bool IssueAtomic( NVMainRequest *req );
    bool IsIssuable( NVMainRequest *request, FailReason *reason = NULL );
    bool IssueCommand( NVMainRequest *req );
    bool PresentInCache(NVMainRequest *req);
    bool IssueFunctional( NVMainRequest *req );
    bool RequestComplete( NVMainRequest *req );
    void Cycle( ncycle_t );
    void RegisterStats( );
    void CalculateStats( );

  protected:
    /* Predicate to determine if a bank is locked. */
    class BankLocked : public SchedulingPredicate
    {
        friend class LAMOST;

      private:
        LAMOST &memoryController;

      public:
        BankLocked( LAMOST &_memoryController ) 
            : memoryController(_memoryController) { }

        bool operator() ( NVMainRequest* );
    };

    /* Predicate to determine if fill queue is full. */
    class FillQueueFull : public SchedulingPredicate
    {
        friend class LAMOST;

      private:
        LAMOST &memoryController;
        bool draining;

      public:
        FillQueueFull( LAMOST &_memoryController ) 
            : memoryController(_memoryController), draining(false) { }

        bool operator() ( NVMainRequest* );
    };

    /* Predicate to determine if fill queue is full. */
    class NoWriteBuffering : public SchedulingPredicate
    {
        friend class LAMOST;

      private:
        LAMOST &memoryController;

      public:
        NoWriteBuffering( LAMOST &_memoryController ) 
            : memoryController(_memoryController) { }

        bool operator() ( NVMainRequest* );
    };

    void CalculateLatency( NVMainRequest *req, double *average, 
            uint64_t *measured );
    void CalculateQueueLatency( NVMainRequest *req, double *average, 
            uint64_t *measured );

private:
  
    NVMTransactionQueue *drcQueue;
//    NVMTransactionQueue *fillQueue;
    uint64_t drcQueueSize;
    ncounter_t missedReq;
    bool tcflag;   
    bool perfectFills;
    bool **bankLocked;
    BankLocked locks;
    FillQueueFull FQF;
    NoWriteBuffering NWB;
    bool useWriteBuffer;
    NVMain *mainMemory;
    CacheBank ***functionalCache;
    
};

};

#endif
