

#ifndef __MEMCONTROL_ABSTRACTDRAMCACHE_H__
#define __MEMCONTROL_ABSTRACTDRAMCACHE_H__


#include "src/MemoryController.h"


namespace NVM {


class NVMain;

class AbstractDRAMCache : public MemoryController
{
	protected:
	
    uint64_t drc_hits;
    uint64_t drc_miss;
    uint64_t drc_evicts, drc_fills;
    uint64_t drc_dirty_evicts;
    uint64_t rb_hits, rb_miss;
    uint64_t starvation_precharges;
    uint64_t bypassedReq;
    double drc_hitrate=0;
    double rb_hitrate=0;
    bool DRC_bypass;
    std::map<NVMainRequest *, NVMainRequest *> outstandingFills;
    
        
 public:
  AbstractDRAMCache( ) { }
  virtual ~AbstractDRAMCache( ) { };
   
  /*
   *  DRAM cache always has some backing memory
   *  behind it for misses, so we need to set this
   *  here, since the backing memory is shared by
   *  all DRAM caches on every channel.
   */
  virtual void SetMainMemory( NVMain *mm ) = 0;
  virtual bool IssueFunctional( NVMainRequest *req ) = 0;
  virtual bool IsIssuable( NVMainRequest *request, FailReason *reason = NULL ) = 0;
  virtual bool PresentInCache(NVMainRequest *req) = 0;
  
  

};



};


#endif

