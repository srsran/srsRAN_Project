/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/support/cpu_architecture_info.h"
#include "srsran/support/math/math_utils.h"
#include <atomic>
#include <memory>
#include <sched.h>
#include <utility>

namespace srsran {

namespace detail {

template <typename SegmentType>
class bounded_object_pool_impl
{
public:
  struct obj_reclaimer {
    obj_reclaimer() = default;
    obj_reclaimer(bounded_object_pool_impl& parent_, uint32_t seg_idx_, uint32_t obj_idx_) :
      parent(&parent_), seg_idx(seg_idx_), obj_idx(obj_idx_)
    {
    }

    void operator()(typename SegmentType::value_type* val)
    {
      if (val != nullptr) {
        parent->reclaim(seg_idx, obj_idx);
      }
    }

    bounded_object_pool_impl* parent  = nullptr;
    uint32_t                  seg_idx = static_cast<uint32_t>(-1);
    uint32_t                  obj_idx = static_cast<uint32_t>(-1);
  };

  template <typename Factory>
  bounded_object_pool_impl(unsigned nof_objects, Factory&& factory) : nof_objs(nof_objects)
  {
    report_fatal_error_if_not(capacity() > 0, "Number of objects must be greater than zero");
    const unsigned nof_segments = divide_ceil(capacity(), SegmentType::max_size());

    // Compute the CPU offset scatter coefficient.
    cpu_offset_scatter_coeff =
        std::max(static_cast<size_t>(1U), cpu_architecture_info::get().get_host_nof_available_cpus() / nof_segments);

    // Initialize segments.
    segments.reserve(nof_segments);
    for (unsigned i = 0; i != nof_segments; ++i) {
      unsigned seg_nof_objs = SegmentType::max_size();
      if (i == nof_segments - 1) {
        // Last segment may have fewer objects.
        seg_nof_objs = ((nof_objects - 1) % SegmentType::max_size()) + 1;
      }
      segments.emplace_back(factory(i, seg_nof_objs));
    }
  }

  size_t capacity() const { return nof_objs; }

  size_t size_approx() const
  {
    size_t sum_used = 0;
    for (unsigned i = 0; i != segments.size(); ++i) {
      uint64_t busy_bitmap = segments[i]->busy_bitmap.load(std::memory_order_relaxed);
      sum_used += count_ones(busy_bitmap);
    }
    return segments.size() * SegmentType::max_size() - sum_used;
  }

  std::pair<unsigned, unsigned> get()
  {
    unsigned offset = 0;
    if (segments.size() > 1) {
      offset = (sched_getcpu() * cpu_offset_scatter_coeff) % segments.size();
    }

    for (unsigned i = 0; i != segments.size(); ++i) {
      unsigned     idx = (offset + i) % segments.size();
      SegmentType& seg = *segments[idx];

      uint64_t busy_bitmap = seg.busy_bitmap.load(std::memory_order_acquire);
      if (busy_bitmap == static_cast<uint64_t>(-1)) {
        continue; // No free objects in this segment.
      }

      // Find the first free object.
      while (true) {
        unsigned obj_idx = find_first_lsb_one(~busy_bitmap);
        if (obj_idx >= SegmentType::max_size()) {
          // No free objects found in this segment.
          break;
        }
        // Mark the object as used.
        busy_bitmap = seg.busy_bitmap.fetch_or((1ULL << obj_idx), std::memory_order_acq_rel);
        if ((busy_bitmap & (1ULL << obj_idx)) == 0) {
          // We successfully marked this position.
          return std::make_pair(idx, obj_idx);
        }
      }
    }
    // No free objects available.
    return std::make_pair(static_cast<unsigned>(-1), static_cast<unsigned>(-1));
  }

  void reclaim(unsigned seg_idx, unsigned obj_idx)
  {
    srsran_assert(seg_idx < segments.size(), "Invalid segment index");

    SegmentType& seg  = *segments[seg_idx];
    uint64_t     prev = seg.busy_bitmap.fetch_and(~(1ULL << obj_idx), std::memory_order_release);
    srsran_assert(prev & (1ULL << obj_idx), "Reclaiming an object that was not allocated");
  }

  /// Number of objects that exist in the pool.
  const unsigned nof_objs;
  /// Used to determine the starting segment for allocation based on CPU ID.
  unsigned cpu_offset_scatter_coeff;
  /// List of segments that contain the objects.
  std::vector<std::unique_ptr<SegmentType>> segments;
};

} // namespace detail

/// \brief A bounded thread-safe object pool that uses bitmaps to track the allocation status of objects.
///
/// The objects are stored in a vector of segments, each segment containing a fixed number of objects. Each segment
/// additionally contains a bitmap that tracks which objects are currently allocated within the same segment.
/// The cpu_id is used as a hash to select the starting position in the vector of segments to search for free
/// objects.
template <typename T>
class bounded_object_pool
{
  struct segment {
    using value_type = T;

    std::atomic<uint64_t> busy_bitmap{0};
    unsigned              nof_objects;
    T*                    objects;

    template <typename... Args>
    segment(unsigned nof_objects_, Args... args) : nof_objects(nof_objects_)
    {
      srsran_assert(size() <= max_size(), "Number of objects exceeds segment capacity");
      // Create objs.
      objects = static_cast<T*>(::operator new(size() * sizeof(T)));
      for (unsigned i = 0; i != size(); ++i) {
        new (objects + i) T(args...);
      }
      if (size() < max_size()) {
        // Mark remaining positions as unavailable.
        unsigned remaining        = max_size() - nof_objects;
        uint64_t mark_unavailable = ((1ULL << remaining) - 1) << nof_objects;
        busy_bitmap.fetch_or(mark_unavailable, std::memory_order_relaxed);
      }
    }
    segment(const segment&)            = delete;
    segment(segment&&)                 = delete;
    segment& operator=(const segment&) = delete;
    segment& operator=(segment&&)      = delete;
    ~segment()
    {
      for (unsigned i = 0; i != size(); ++i) {
        objects[i].~T();
      }
      ::operator delete(objects);
    }

    size_t size() const { return nof_objects; }

    static constexpr size_t max_size() { return 64; }
  };

  using obj_reclaimer = typename detail::bounded_object_pool_impl<segment>::obj_reclaimer;

public:
  using ptr = std::unique_ptr<T, obj_reclaimer>;

  /// Creates an object pool with capacity equal to \c nof_objects.
  template <typename... Args>
  bounded_object_pool(size_t nof_objects, Args... args) :
    impl(nof_objects,
         [args...](unsigned seg_idx, unsigned seg_size) { return std::make_unique<segment>(seg_size, args...); })
  {
  }

  /// \brief Retrieves a RAII pointer to an object of the pool.
  ptr get()
  {
    auto [seg_idx, obj_idx] = impl.get();
    if (seg_idx == static_cast<unsigned>(-1)) {
      // No free objects available.
      return ptr{nullptr, obj_reclaimer{}};
    }
    return ptr{impl.segments[seg_idx]->objects + obj_idx, obj_reclaimer{impl, seg_idx, obj_idx}};
  }

  /// Maximum number of objects that can be allocated in the pool.
  size_t capacity() const { return impl.capacity(); }

  /// Gets approximate number of objects currently allocated in the pool.
  size_t size_approx() const { return impl.size_approx(); }

private:
  detail::bounded_object_pool_impl<segment> impl;
};

/// \brief A bounded thread-safe object pool. Unlike \c bounded_object_pool, this pool stores objects in unique_ptr<T>,
/// so it allows storing polymorphic objects.
template <typename T>
class bounded_object_ptr_pool
{
  struct segment {
    using value_type = T;

    std::atomic<uint64_t>           busy_bitmap{0};
    std::vector<std::unique_ptr<T>> objects;

    template <typename... Args>
    segment(unsigned nof_objects, Args... args)
    {
      srsran_assert(nof_objects <= max_size(), "Number of objects exceeds segment capacity");
      // Create objs.
      objects.reserve(nof_objects);
      for (unsigned i = 0; i != nof_objects; ++i) {
        objects.emplace_back(std::make_unique<T>(args...));
      }
      if (nof_objects < max_size()) {
        // Mark remaining positions as unavailable.
        unsigned remaining        = max_size() - nof_objects;
        uint64_t mark_unavailable = ((1ULL << remaining) - 1) << nof_objects;
        busy_bitmap.fetch_or(mark_unavailable, std::memory_order_relaxed);
      }
    }
    segment(unsigned nof_objects, span<std::unique_ptr<T>> objects_)
    {
      srsran_assert(nof_objects <= max_size(), "Number of objects exceeds segment capacity");
      // Create objs.
      objects.reserve(nof_objects);
      for (unsigned i = 0; i != nof_objects; ++i) {
        objects.emplace_back(std::move(objects_[i]));
      }
      if (nof_objects < max_size()) {
        // Mark remaining positions as unavailable.
        unsigned remaining        = max_size() - nof_objects;
        uint64_t mark_unavailable = ((1ULL << remaining) - 1) << nof_objects;
        busy_bitmap.fetch_or(mark_unavailable, std::memory_order_relaxed);
      }
    }

    size_t size() const { return objects.size(); }

    static constexpr size_t max_size() { return 64; }
  };

  using obj_reclaimer = typename detail::bounded_object_pool_impl<segment>::obj_reclaimer;

public:
  using ptr = std::unique_ptr<T, obj_reclaimer>;

  /// Creates an object pool with the objects passed in the ctor.
  bounded_object_ptr_pool(span<std::unique_ptr<T>> objects_) :
    impl(objects_.size(), [&objects_](unsigned seg_idx, unsigned seg_nof_objs) {
      return std::make_unique<segment>(seg_nof_objs, objects_.subspan(seg_idx * segment::max_size(), seg_nof_objs));
    })
  {
  }

  /// Creates an object pool with capacity equal to \c nof_objects.
  template <typename... Args>
  bounded_object_ptr_pool(unsigned nof_objects, Args... args) :
    impl(nof_objects, [args...](unsigned seg_idx, unsigned seg_nof_objs) {
      return std::make_unique<segment>(seg_nof_objs, args...);
    })
  {
  }

  /// \brief Retrieves a RAII pointer to an object of the pool.
  ptr get()
  {
    auto [seg_idx, obj_idx] = impl.get();
    if (seg_idx == static_cast<unsigned>(-1)) {
      // No free objects available.
      return ptr{nullptr, obj_reclaimer{}};
    }
    srsran_sanity_check(obj_idx < impl.segments[seg_idx]->objects.size(), "Invalid object index");
    return ptr{impl.segments[seg_idx]->objects[obj_idx].get(), obj_reclaimer{impl, seg_idx, obj_idx}};
  }

  /// Maximum number of objects that can be allocated in the pool.
  size_t capacity() const { return impl.capacity(); }

  /// Gets approximate number of objects currently allocated in the pool.
  size_t size_approx() const { return impl.size_approx(); }

private:
  detail::bounded_object_pool_impl<segment> impl;
};

} // namespace srsran
