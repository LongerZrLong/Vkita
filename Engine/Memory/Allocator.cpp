#include "Allocator.h"

#include <cstring>

#include "Common/Assert.h"

#ifndef ALIGN
#define ALIGN(x, a)         (((x) + ((a) - 1)) & ~((a) - 1))
#endif

namespace VKT {

    Allocator::Allocator()
        : m_PageList(nullptr), m_FreeList(nullptr),
          m_DataSize(0), m_PageSize(0), m_BlockSize(0), m_AlignmentSize(0), m_NumBlocksPerPage(0), m_NumPages(0),
          m_NumBlocks(0), m_NumFreeBlocks(0)
    {
    }

    Allocator::Allocator(size_t dataSize, size_t pageSize, size_t alignment)
        : m_PageList(nullptr), m_FreeList(nullptr)
    {
        Reset(dataSize, pageSize, alignment);
    }

    Allocator::~Allocator()
    {
        FreeAll();
    }

    void Allocator::Reset(size_t dataSize, size_t pageSize, size_t alignment)
    {
        FreeAll();

        m_DataSize = dataSize;
        m_PageSize = pageSize;

        size_t minimalSize = (sizeof(BlockHeader) > m_DataSize) ? sizeof(BlockHeader) : m_DataSize;

        // this magic only works when alignment is 2^n, which should generally be the case
        // because most CPU/GPU also requires the alignment be in 2^n
        // but still we use assert to guarantee it
#ifndef NDEBUG
        VKT_CORE_ASSERT(alignment > 0 && ((alignment & (alignment-1))) == 0,
                        "VKT::Allocator::Reset: alignment = {}, which is not in 2^n", alignment);
#endif

        m_BlockSize = ALIGN(minimalSize, alignment);

        m_AlignmentSize = m_BlockSize - minimalSize;

        m_NumBlocksPerPage = (m_PageSize - sizeof(PageHeader)) / m_BlockSize;
    }

    void *Allocator::Allocate()
    {
        if (!m_FreeList)
        {
            // allocate a new page
            auto *pNewPage = reinterpret_cast<PageHeader*>(malloc(m_PageSize));
            ++m_NumPages;
            m_NumBlocks += m_NumBlocksPerPage;
            m_NumFreeBlocks += m_NumBlocksPerPage;

#ifndef NDEBUG
            FillFreePage(pNewPage);
#endif

            if (m_PageList)
            {
                pNewPage->pNext = m_PageList;
            }

            m_PageList = pNewPage;

            BlockHeader *pBlock = pNewPage->Blocks();
            // link each block in the page
            for (uint32_t i = 0; i < m_NumBlocksPerPage - 1; i++)
            {
                pBlock->pNext = NextBlock(pBlock);
                pBlock = NextBlock(pBlock);
            }
            pBlock->pNext = nullptr;

            m_FreeList = pNewPage->Blocks();
        }

        BlockHeader *freeBlock = m_FreeList;
        m_FreeList = m_FreeList->pNext;
        --m_NumFreeBlocks;

#ifndef NDEBUG
        FillAllocatedBlock(freeBlock);
#endif

        return reinterpret_cast<void*>(freeBlock);
    }

    void Allocator::Free(void *p)
    {
        auto* pBlock = reinterpret_cast<BlockHeader*>(p);

#ifndef NDEBUG
        FillFreeBlock(pBlock);
#endif

        pBlock->pNext = m_FreeList;
        m_FreeList = pBlock;
        ++m_NumFreeBlocks;
    }

    void Allocator::FreeAll()
    {
        PageHeader *pPage = m_PageList;
        while (pPage)
        {
            PageHeader *_p = pPage;
            pPage = pPage->pNext;

            delete[] reinterpret_cast<uint8_t*>(_p);
        }

        m_PageList = nullptr;
        m_FreeList = nullptr;

        m_NumPages        = 0;
        m_NumBlocks       = 0;
        m_NumFreeBlocks   = 0;
    }

#ifndef NDEBUG

    void Allocator::FillFreePage(PageHeader *pPage)
    {
        // page header
        pPage->pNext = nullptr;

        // blocks
        BlockHeader *pBlock = pPage->Blocks();
        for (uint32_t i = 0; i < m_NumBlocksPerPage; i++)
        {
            FillFreeBlock(pBlock);
            pBlock = NextBlock(pBlock);
        }
    }

    void Allocator::FillFreeBlock(BlockHeader *pBlock)
    {
        // block header + data
        std::memset(pBlock, PATTERN_FREE, m_BlockSize - m_AlignmentSize);

        // alignment
        std::memset(reinterpret_cast<uint8_t*>(pBlock) + m_BlockSize - m_AlignmentSize, PATTERN_ALIGN, m_AlignmentSize);
    }

    void Allocator::FillAllocatedBlock(BlockHeader *pBlock)
    {
        // block header + data
        std::memset(pBlock, PATTERN_ALLOC, m_BlockSize - m_AlignmentSize);

        // alignment
        std::memset(reinterpret_cast<uint8_t*>(pBlock) + m_BlockSize - m_AlignmentSize, PATTERN_ALIGN, m_AlignmentSize);
    }

#endif

    BlockHeader *Allocator::NextBlock(BlockHeader *pBlock)
    {
        return reinterpret_cast<BlockHeader*>(reinterpret_cast<uint8_t*>(pBlock) + m_BlockSize);
    }
}