#pragma once

#include <cstddef>
#include <cstdint>

namespace VKT {

    struct BlockHeader
    {
        // union-ed with data
        BlockHeader *pNext;
    };

    struct PageHeader
    {
        PageHeader *pNext;
        BlockHeader *Blocks()
        {
            return reinterpret_cast<BlockHeader*>(this + 1);
        }
    };

    class Allocator
    {
    public:
        // debug patterns
        static const uint8_t PATTERN_ALIGN = 0xFC;
        static const uint8_t PATTERN_ALLOC = 0xFD;
        static const uint8_t PATTERN_FREE  = 0xFE;

        Allocator();
        Allocator(size_t dataSize, size_t pageSize, size_t alignment);
        ~Allocator();

        // resets the allocator to a new configuration
        void Reset(size_t dataSize, size_t pageSize, size_t alignment);

        // alloc and free blocks
        void *Allocate();
        void  Free(void *p);
        void  FreeAll();

    private:
#ifndef NDEBUG
        // fill a free page with debug patterns
        void FillFreePage(PageHeader *pPage);

        // fill a block with debug patterns
        void FillFreeBlock(BlockHeader *pBlock);

        // fill an allocated block with debug patterns
        void FillAllocatedBlock(BlockHeader *pBlock);
#endif

        // gets the next block
        BlockHeader *NextBlock(BlockHeader *pBlock);

        // the page list
        PageHeader *m_PageList;

        // the free block list
        BlockHeader *m_FreeList;

        size_t      m_DataSize;
        size_t      m_PageSize;
        size_t      m_AlignmentSize;
        size_t      m_BlockSize;
        uint32_t    m_NumBlocksPerPage;

        // statistics
        uint32_t    m_NumPages;
        uint32_t    m_NumBlocks;
        uint32_t    m_NumFreeBlocks;

        // disable copy & assignment
        Allocator(const Allocator &clone);
        Allocator &operator=(const Allocator &rhs);

    };
}

