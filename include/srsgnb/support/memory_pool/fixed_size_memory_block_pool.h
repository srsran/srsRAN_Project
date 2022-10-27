/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
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

namespace srsgnb {

/**
 * Concurrent memory pool of segments of equal size.
 * Each worker keeps a separate thread-local memory segment cache that it uses for fast, uncontended allocation &
 * deallocation. When accessing a thread-local cache, no locks are required.
 * When this cache gets depleted, the worker tries to obtain segments from a central memory segment cache.
 * Since there is no stealing of segments between threads, it is possible that a worker cannot allocate while another
 * worker still has blocks in its own cache. To minimize the impact of this event, an upper bound is place on a worker
 * worker cache size. Once a worker reaches that upper bound, it sends half of its stored blocks to the central cache.
 * Note: Taking into account the usage of thread_local, this class is made a singleton
 * Note2: No considerations were made regarding false sharing between threads. It is assumed that the blocks are big
 *        enough to fill a cache line.
 * @tparam NofObjects number of objects in the pool
 */
template <typename IdTag, bool DebugSanitizeAddress = false>
class fixed_size_memory_block_pool
{
  using pool_type = fixed_size_memory_block_pool<IdTag, DebugSanitizeAddress>;

  const static size_t batch_steal_size = 16;

  // ctor only accessible from singleton get_instance()
  explicit fixed_size_memory_block_pool(size_t memory_block_size_, size_t nof_blocks_) :
    mblock_size(align_next(memory_block_size_, alignof(std::max_align_t))), nof_blocks(nof_blocks_)
  {
    srsgnb_assert(nof_blocks > batch_steal_size,
                  "The number of segments in the pool must be larger than the thread cache size");
    srsgnb_assert(mblock_size > free_memory_block_list::min_memory_block_align(), "Segment size is too small");

    // Allocate the required memory for the given number of segments and segment size.
    size_t total_mem = mblock_size * nof_blocks;
    allocated_memory.resize(total_mem);

    // Push all segments to the central cache.
    for (unsigned i = 0; i != nof_blocks; ++i) {
      central_mem_cache.push(static_cast<void*>(allocated_memory.data() + (mblock_size * i)));
    }

    local_growth_thres = nof_blocks / 16;
    local_growth_thres = local_growth_thres < batch_steal_size ? batch_steal_size : local_growth_thres;
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

  /// \brief Instantiation of memory pool singleton.
  static fixed_size_memory_block_pool<IdTag, DebugSanitizeAddress>& get_instance(size_t mem_block_size = 0,
                                                                                 size_t nof_blocks     = 0)
  {
    static fixed_size_memory_block_pool<IdTag, DebugSanitizeAddress> pool(mem_block_size, nof_blocks);
    return pool;
  }

  /// Number of memory segments contained in this memory pool.
  size_t memory_block_size() const { return mblock_size; }

  /// Number of memory segments contained in this memory pool.
  size_t nof_memory_blocks() const { return nof_blocks; }

  /// Allocate a node from the memory pool with the provided size.
  void* allocate_node(size_t sz)
  {
    srsgnb_assert(sz <= mblock_size, "Allocated node size={} exceeds max object size={}", sz, mblock_size);
    worker_ctxt* worker_ctxt = get_worker_cache();

    void* node = worker_ctxt->local_cache.try_pop();
    if (node == nullptr) {
      // fill the thread local cache enough for this and next allocations
      std::array<void*, batch_steal_size> popped_blocks;
      size_t                              n = central_mem_cache.try_pop(popped_blocks);
      for (size_t i = 0; i < n; ++i) {
        worker_ctxt->local_cache.push(static_cast<void*>(popped_blocks[i]));
      }
      node = worker_ctxt->local_cache.try_pop();
    }

    return node;
  }

  void deallocate_node(void* p)
  {
    srsgnb_assert(p != nullptr, "Deallocated nodes must have valid address");

    worker_ctxt* worker_ctxt = get_worker_cache();

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
    worker_ctxt->local_cache.push(p);

    if (worker_ctxt->local_cache.size() >= local_growth_thres) {
      // if local cache reached max capacity, send half of the blocks to central cache
      central_mem_cache.steal_blocks(worker_ctxt->local_cache, worker_ctxt->local_cache.size() / 2);
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

} // namespace srsgnb