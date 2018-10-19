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
*   Tao Zhang       ( Email: tzz106 at cse dot psu dot edu 
*                     Website: http://www.cse.psu.edu/~tzz106/ )
*******************************************************************************/

#ifndef __TRANSLATIONMETHOD_H__
#define __TRANSLATIONMETHOD_H__

#include <stdint.h>
#include <string>

namespace NVM {

enum MemoryPartition 
{ 
    MEM_ROW = 0, 
    MEM_COL = 1, 
    MEM_BANK = 2, 
    MEM_RANK = 3, 
    MEM_CHANNEL = 4,
    MEM_SUBARRAY = 5,
    MEM_UNKNOWN = 100
};

class TranslationMethod
{
  public:
    TranslationMethod( );
    ~TranslationMethod( );

    void SetAddressMappingScheme( std::string scheme );
    void SetBitWidths( unsigned int rowBits, unsigned int colBits, unsigned int bankBits, 
          	     unsigned int rankBits, unsigned int channelBits, unsigned int subarrayBits );
    void SetOrder( int row, int col, int bank, int rank, int channel, int subarray );
    void SetCount( uint64_t rows, uint64_t cols, uint64_t banks, 
                   uint64_t ranks, uint64_t channels, uint64_t subarrays );

    void GetBitWidths( unsigned int *rowBits, unsigned int *colBits, unsigned int *bankBits,
          	     unsigned int *rankBits, unsigned int *channelBits, unsigned int *subarrayBits );
    void GetOrder( int *row, int *col, int *bank, int *rank, int *channel, int *subarrayBits );
    void GetCount( uint64_t *rows, uint64_t *cols, uint64_t *banks, 
                   uint64_t *ranks, uint64_t *channels, uint64_t *subarrays );
                   
    void PrintParams(unsigned int rowBits, unsigned int colBits, unsigned int bankBits,
				      unsigned int rankBits, unsigned int channelBits, unsigned int subarrayBits);

  private:
    unsigned int bitWidths[6];
    uint64_t count[6];
    int order[6];
};

};

#endif