/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "memory_block_list.h"
#include "srsgnb/support/error_handling.h"
#include "srsgnb/support/srsgnb_assert.h"
#include <mutex>
#include <thread>
#include <vector>

namespace srsran {

/**
 * Concurrent memory pool of memory blocks of equal size. This pool is thread-safe.
 * Each worker keeps a separate thread-local memory block cache that it uses for fast, uncontended allocation and
 * deallocation. When accessing a thread-local cache, no locks are required.
 *
 * When the local cache gets depleted, the worker tries to obtain a batch of segments from a central memory block cache.
 * If the central cache is also depleted, the allocation fails.
 *
 * Since there is no stealing of segments between workers' local caches, it is possible that a worker cannot allocate
 * while another worker still has blocks in its own cache. To minimize the impact of this event, an upper bound is
 * placed on a worker local cache size. Once a worker reaches that upper bound, it sends half of its stored blocks to
 * the central cache.
 *
 * Note1: Notice that the same memory block might be allocated and deallocated in different workers. So, it can happen
 * that a worker is allocating many blocks and another worker just deallocating. The latter worker has to keep
 * returning blocks back to the central cache every time its local cache grows beyond a given upper bound.
 *
 * Note2: Taking into account the usage of thread_local, this class is made a singleton. To be able to instantiate
 *       different pools, the user should use different IdTag types.
 *
 * Note3: No considerations were made regarding false sharing between workers. It is assumed that the blocks are big
 *        enough to fill a cache line.
 *
 * \tparam IdTag We use a ID type tag to be able to intantiate different pool objects.
 * \tparam DebugSanitizeAddress when set to true, the pool verifies that the addresses allocated and deallocated are
 * valid.
 */
template <typename IdTag, bool DebugSanitizeAddress = false>
class fixed_size_memory_block_pool
{
  using pool_type = fixed_size_memory_block_pool<IdTag, DebugSanitizeAddress>;

  /// The number of blocks a worker tries to steal from the central memory block cache in a single batch.
  constexpr static size_t batch_steal_size = 16;

  /// Ctor of the memory pool. It is set as private because the class works as a singleton.
  explicit fixed_size_memory_block_pool(size_t nof_blocks_, size_t memory_block_size_) :
    mblock_size(align_next(memory_block_size_, alignof(std::max_align_t))), nof_blocks(nof_blocks_)
  {
    srsgnb_assert(nof_blocks > batch_steal_size,
                  "The number of segments in the pool must be larger than the thread cache size ({} <= {})",
                  nof_blocks,
                  (size_t)batch_steal_size);
    srsgnb_assert(mblock_size > free_memory_block_list::min_memory_block_align(),
                  "Segment size is too small ({} <= {})",
                  mblock_size,
                  free_memory_block_list::min_memory_block_align());

    // Allocate the required memory for the given number of segments and segment size.
    size_t total_mem = mblock_size * nof_blocks;
    allocated_memory.resize(total_mem);

    // Push all segments to the central cache.
    for (unsigned i = 0; i != nof_blocks; ++i) {
      central_mem_cache.push(static_cast<void*>(allocated_memory.data() + (mblock_size * i)));
    }

    local_growth_thres = nof_blocks / 16;
    local_growth_thres = std::max((size_t)local_growth_thres, (size_t)batch_steal_size);
  }

public:
  fixed_size_memory_block_pool(const fixed_size_memory_block_pool&)            = delete;
  fixed_size_memory_block_pool(fixed_size_memory_block_pool&&)                 = delete;
  fixed_size_memory_block_pool& operator=(const fixed_size_memory_block_pool&) = delete;
  fixed_size_memory_block_pool& operator=(fixed_size_memory_block_pool&&)      = delete;

  ~fixed_size_memory_block_pool()
  {
    std::lock_guard<std::mutex> lock(mutex);
    allocated_memory.clear();
  }

  /// \brief Get instance of a memory pool singleton.
  static fixed_size_memory_block_pool<IdTag, DebugSanitizeAddress>& get_instance(size_t nof_blocks     = 0,
                                                                                 size_t mem_block_size = 0)
  {
    static fixed_size_memory_block_pool<IdTag, DebugSanitizeAddress> pool(nof_blocks, mem_block_size);
    return pool;
  }

  /// Memory block size in bytes.
  size_t memory_block_size() const { return mblock_size; }

  /// Number of memory blocks contained in this memory pool.
  size_t nof_memory_blocks() const { return nof_blocks; }

  /// Allocate a node from the memory pool with the provided size.
  void* allocate_node(size_t sz) noexcept
  {
    srsgnb_assert(sz <= mblock_size, "Allocated node size={} exceeds max object size={}", sz, mblock_size);
    worker_ctxt* w_ctx = get_worker_cache();

    void* node = w_ctx->local_cache.try_pop();
    if (node == nullptr) {
      // fill the thread local cache enough for this and next allocations
      std::array<void*, batch_steal_size> popped_blocks;
      size_t                              n = central_mem_cache.try_pop(popped_blocks);
      for (size_t i = 0; i < n; ++i) {
        w_ctx->local_cache.push(static_cast<void*>(popped_blocks[i]));
      }
      node = w_ctx->local_cache.try_pop();
    }

    return node;
  }

  /// Deallocate node by returning it back to the memory pool.
  void deallocate_node(void* p)
  {
    srsgnb_assert(p != nullptr, "Deallocated nodes must have valid address");

    worker_ctxt* w_ctx = get_worker_cache();

    if (DebugSanitizeAddress) {
      // For debug purposes.
      std::lock_guard<std::mutex> lock(mutex);
      bool                        found = false;
      for (unsigned i = 0; i != nof_blocks; ++i) {
        if (allocated_memory.data() + i * mblock_size == static_cast<uint8_t*>(p)) {
          found = true;
        }
      }
      if (not found) {
        report_fatal_error("Error dealloccating block with address {:#x}", p);
      }
    }

    // push to local memory block cache.
    w_ctx->local_cache.push(p);

    if (w_ctx->local_cache.size() >= local_growth_thres) {
      // if local cache reached max capacity, send half of the blocks to central cache
      central_mem_cache.steal_blocks(w_ctx->local_cache, w_ctx->local_cache.size() / 2);
    }
  }

  void print_all_buffers()
  {
    auto* worker = get_worker_cache();
    fmt::print("There are {}/{} buffers in central memory block cache. This thread contains {} in its local cache.\n",
               central_mem_cache.size(),
               nof_memory_blocks(),
               worker->local_cache.size());
  }

private:
  struct worker_ctxt {
    std::thread::id        id;
    free_memory_block_list local_cache;

    worker_ctxt() : id(std::this_thread::get_id()) {}
    ~worker_ctxt()
    {
      concurrent_free_memory_block_list& central_cache = pool_type::get_instance().central_mem_cache;
      central_cache.steal_blocks(local_cache, local_cache.size());
    }
  };

  worker_ctxt* get_worker_cache()
  {
    thread_local worker_ctxt worker_cache;
    return &worker_cache;
  }

  const size_t mblock_size;
  const size_t nof_blocks;

  size_t local_growth_thres = 0;

  concurrent_free_memory_block_list central_mem_cache;
  std::mutex                        mutex;
  std::vector<uint8_t>              allocated_memory;
};

} // namespace srsran