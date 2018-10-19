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

#ifndef __CACHES_TagCache_H__
#define __CACHES_TagCache_H__

#include "src/MemoryController.h"
#include "Caches/CacheBank/CacheBank.h"
#include "Decoders/DRCDecoder/DRCDecoder.h"
#include "include/NVMHelpers.h"
#include "NVM/nvmain.h"
#include "MemControl/DRAMCache/DRAMCache.h"
#include "src/EventQueue.h"
#include "MemControl/LAMOST/LAMOST.h"

#include <iostream>
#include <sstream>
#include <cassert>


namespace NVM {

class NVMain;
class Config;

/* Must be powers of two! */
enum { TCACHE_ENTRY_NONE = 0,
       TCACHE_ENTRY_VALID = 1,
       TCACHE_ENTRY_DIRTY = 2,
       TCACHE_ENTRY_EXAMPLE = 4
};

struct TagCacheEntry
{
    uint64_t flags;
    uint64_t row;
    uint64_t bank;
    uint64_t rank; 
    uint64_t chan;
    uint64_t subarray;
    NVMAddress address;
};


class TagCache: public NVMObject
{
  public:
    TagCache( );
    ~TagCache( );

    void SetConfig( Config *conf, bool createChildren = true );
    bool IssueAtomic( NVMainRequest *req );
    bool IssueCommand( NVMainRequest *req );
    bool RequestComplete( NVMainRequest *req );
    void RegisterStats( );
    void CalculateStats( );
    void Cycle( ncycle_t /*steps*/ );
    void Retranslate( NVMainRequest *req );
    bool IsIssuable( NVMainRequest *request, FailReason *reason = NULL );
    bool tcPresent( NVMAddress& addr );
    bool tcInstall( NVMainRequest *req );
    bool tcSetFull( NVMainRequest *req );
    bool tcEvict( NVMAddress& victim_addr );
    bool UpdatePosition( NVMainRequest *req );
    bool tcChooseVictim( NVMainRequest *req, NVMAddress *victim );
    uint32_t tcFindSet( NVMAddress& addr );
    void PrintTagCache(  );
    
  private:
    TagCacheEntry **tagCache;
    NVMain *nvmain;
    DRAMCache *DRC;
    Config *config;
    uint64_t tcSets;	
    uint64_t tcAssoc;
    
    /* Stats. */
    ncounter_t tagCacheFills;
    ncounter_t tagCacheHits;
    ncounter_t tagCacheMisses;
    ncounter_t tagCacheEvicts;
    double tagcache_hitrate;
    
};

};

#endif
