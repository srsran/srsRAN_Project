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
#include "srsran/support/math/math_utils.h"
#include <atomic>
#include <memory>
#include <thread>
#include <utility>

namespace srsran {

namespace detail {

template <typename SegmentType>
class bounded_object_pool_impl
{
public:
  struct obj_reclaimer {
    obj_reclaimer() = default;
    obj_reclaimer(bounded_object_pool_impl& parent_, uint64_t pos_) : parent(&parent_), pos(pos_) {}

    void operator()(typename SegmentType::value_type* val)
    {
      if (val != nullptr) {
        parent->reclaim(pos);
      }
    }

    bounded_object_pool_impl* parent = nullptr;
    uint64_t                  pos    = 0;
  };

  size_t capacity() const { return segments.size() * SegmentType::capacity(); }

  size_t size_approx() const
  {
    size_t sum_used = 0;
    for (unsigned i = 0; i != segments.size(); ++i) {
      uint64_t busy_bitmap = segments[i]->busy_bitmap.load(std::memory_order_relaxed);
      sum_used += count_ones(busy_bitmap);
    }
    return capacity() - sum_used;
  }

  std::pair<unsigned, unsigned> get()
  {
    unsigned offset = 0;
    if (segments.size() > 1) {
      offset = std::hash<std::thread::id>{}(std::this_thread::get_id()) % segments.size();
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
        if (obj_idx >= SegmentType::capacity()) {
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

  void reclaim(unsigned idx)
  {
    unsigned segment_idx = idx / SegmentType::capacity();
    unsigned obj_idx     = idx % SegmentType::capacity();
    srsran_assert(segment_idx < segments.size(), "Invalid segment index");

    SegmentType& seg  = *segments[segment_idx];
    uint64_t     prev = seg.busy_bitmap.fetch_and(~(1ULL << obj_idx), std::memory_order_release);
    srsran_assert(prev & (1ULL << obj_idx), "Reclaiming an object that was not allocated");
  }

  std::vector<std::unique_ptr<SegmentType>> segments;
};

} // namespace detail

/// \brief A bounded thread-safe object pool that uses bitmaps to track the allocation status of objects.
///
/// The objects are stored in a vector of segments, each segment containing a fixed number of objects. Each segment
/// additionally contains a bitmap that tracks which objects are currently allocated within the same segment.
/// The thread_id is used as a hash to select the starting position in the vector of segments to search for free
/// objects.
template <typename T>
class bounded_object_pool
{
  struct segment {
    using value_type = T;

    std::atomic<uint64_t> busy_bitmap{0};
    T*                    objects;

    template <typename... Args>
    segment(Args... args)
    {
      // Create objs.
      objects = static_cast<T*>(::operator new(capacity() * sizeof(T)));
      for (unsigned i = 0; i != capacity(); ++i) {
        new (objects + i) T(args...);
      }
    }
    segment(const segment&)            = delete;
    segment(segment&&)                 = delete;
    segment& operator=(const segment&) = delete;
    segment& operator=(segment&&)      = delete;
    ~segment()
    {
      for (unsigned i = 0; i != capacity(); ++i) {
        objects[i].~T();
      }
      ::operator delete(objects);
    }

    static constexpr size_t capacity() { return 64; }
  };

  using obj_reclaimer = typename detail::bounded_object_pool_impl<segment>::obj_reclaimer;

public:
  using ptr = std::unique_ptr<T, obj_reclaimer>;

  template <typename... Args>
  bounded_object_pool(size_t min_nof_objects, Args... args)
  {
    unsigned nof_segments = divide_ceil(min_nof_objects, segment::capacity());
    for (unsigned i = 0; i != nof_segments; ++i) {
      impl.segments.emplace_back(std::make_unique<segment>(args...));
    }
  }

  /// \brief Retrieves a RAII pointer to an object of the pool.
  ptr get()
  {
    auto [segment_idx, obj_idx] = impl.get();
    if (segment_idx == static_cast<unsigned>(-1)) {
      // No free objects available.
      return ptr{nullptr, obj_reclaimer{}};
    }
    return ptr{impl.segments[segment_idx]->objects + obj_idx,
               obj_reclaimer{impl, segment_idx * segment::capacity() + obj_idx}};
  }

  /// Maximum number of objects that can be allocated in the pool.
  size_t capacity() const { return impl.capacity(); }

  /// Gets approximate number of objects currently allocated in the pool.
  size_t size_approx() const { return impl.size_approx(); }

private:
  void reclaim(unsigned idx) { impl.reclaim(idx); }

  detail::bounded_object_pool_impl<segment> impl;
};

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
      // Create objs.
      objects.reserve(nof_objects);
      for (unsigned i = 0; i != nof_objects; ++i) {
        objects.emplace_back(T(args...));
      }
      if (nof_objects < capacity()) {
        // Mark remaining positions as unavailable.
        unsigned remaining = segment::capacity() - nof_objects;
        impl.back()->busy_bitmap.fetch_or((1ULL << remaining) - 1, std::memory_order_relaxed);
      }
    }

    static constexpr size_t capacity() { return 64; }
  };

  using obj_reclaimer = typename detail::bounded_object_pool_impl<segment>::obj_reclaimer;

public:
  using ptr = std::unique_ptr<std::unique_ptr<T>, obj_reclaimer>;

  bounded_object_ptr_pool(span<std::unique_ptr<T>> objects_)
  {
    unsigned nof_segments = divide_ceil(objects_.size(), segment::capacity());
    for (unsigned i = 0; i != nof_segments; ++i) {
      impl.segments.emplace_back(std::make_unique<segment>());
    }
  }

  /// \brief Retrieves a RAII pointer to an object of the pool.
  ptr get()
  {
    auto [segment_idx, obj_idx] = impl.get();
    if (segment_idx == static_cast<unsigned>(-1)) {
      // No free objects available.
      return ptr{nullptr, obj_reclaimer{}};
    }
    return ptr{impl.segments[segment_idx]->objects[obj_idx].get(),
               obj_reclaimer{impl, segment_idx * segment::capacity() + obj_idx}};
  }

  /// Maximum number of objects that can be allocated in the pool.
  size_t capacity() const { return impl.capacity(); }

  /// Gets approximate number of objects currently allocated in the pool.
  size_t size_approx() const { return impl.size_approx(); }

private:
  void reclaim(unsigned idx) { impl.reclaim(idx); }

  detail::bounded_object_pool_impl<segment> impl;
};

} // namespace srsran
