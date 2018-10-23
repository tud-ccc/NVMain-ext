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
