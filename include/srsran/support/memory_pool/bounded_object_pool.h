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

  size_t size_approx() const
  {
    size_t sum_used = 0;
    for (unsigned i = 0; i != segments.size(); ++i) {
      uint64_t busy_bitmap = segments[i]->busy_bitmap.load(std::memory_order_relaxed);
      sum_used += count_ones(busy_bitmap);
    }
    return segments.size() * SegmentType::capacity() - sum_used;
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
/// The capacity of this pool is always increased to the closest power of two that is greater than or equal to the
/// provided \c min_nof_objects.
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

  /// Creates an object pool with capacity of a power of two that is greater than or equal to \c min_nof_objects.
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
  size_t capacity() const { return impl.segments.size() * segment::capacity(); }

  /// Gets approximate number of objects currently allocated in the pool.
  size_t size_approx() const { return impl.size_approx(); }

private:
  void reclaim(unsigned idx) { impl.reclaim(idx); }

  detail::bounded_object_pool_impl<segment> impl;
};

/// \brief A bounded thread-safe object pool. Unlike \c bounded_object_pool, this pool stores objects in unique_ptr<T>,
/// and its capacity is not necessarily a power of two.
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
      srsran_assert(nof_objects <= capacity(), "Number of objects exceeds segment capacity");
      // Create objs.
      objects.reserve(nof_objects);
      for (unsigned i = 0; i != nof_objects; ++i) {
        objects.emplace_back(std::make_unique<T>(args...));
      }
      if (nof_objects < capacity()) {
        // Mark remaining positions as unavailable.
        unsigned remaining        = capacity() - nof_objects;
        uint64_t mark_unavailable = ((1ULL << remaining) - 1) << nof_objects;
        busy_bitmap.fetch_or(mark_unavailable, std::memory_order_relaxed);
      }
    }
    segment(unsigned nof_objects, span<std::unique_ptr<T>> objects_)
    {
      srsran_assert(nof_objects <= capacity(), "Number of objects exceeds segment capacity");
      // Create objs.
      objects.reserve(nof_objects);
      for (unsigned i = 0; i != nof_objects; ++i) {
        objects.emplace_back(std::move(objects_[i]));
      }
      if (nof_objects < capacity()) {
        // Mark remaining positions as unavailable.
        unsigned remaining        = capacity() - nof_objects;
        uint64_t mark_unavailable = ((1ULL << remaining) - 1) << nof_objects;
        busy_bitmap.fetch_or(mark_unavailable, std::memory_order_relaxed);
      }
    }

    static constexpr size_t capacity() { return 64; }
  };

  using obj_reclaimer = typename detail::bounded_object_pool_impl<segment>::obj_reclaimer;

public:
  using ptr = std::unique_ptr<T, obj_reclaimer>;

  bounded_object_ptr_pool(span<std::unique_ptr<T>> objects_) : nof_objs(objects_.size())
  {
    unsigned nof_segments = divide_ceil(objects_.size(), segment::capacity());
    for (unsigned i = 0; i != nof_segments; ++i) {
      unsigned rem_objs = i == nof_segments - 1 ? objects_.size() % segment::capacity() : segment::capacity();
      impl.segments.emplace_back(
          std::make_unique<segment>(rem_objs, objects_.subspan(i * segment::capacity(), rem_objs)));
    }
  }
  template <typename... Args>
  bounded_object_ptr_pool(unsigned nof_objects, Args... args) : nof_objs(nof_objects)
  {
    unsigned nof_segments = divide_ceil(nof_objects, segment::capacity());
    for (unsigned i = 0; i != nof_segments; ++i) {
      unsigned rem_objs = i == nof_segments - 1 ? nof_objects % segment::capacity() : segment::capacity();
      impl.segments.emplace_back(std::make_unique<segment>(rem_objs, args...));
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
    fmt::print("Allocated object at segment {} index {}\n", segment_idx, obj_idx);
    srsran_sanity_check(obj_idx < impl.segments[segment_idx]->objects.size(), "Invalid object index");
    return ptr{impl.segments[segment_idx]->objects[obj_idx].get(),
               obj_reclaimer{impl, segment_idx * segment::capacity() + obj_idx}};
  }

  /// Maximum number of objects that can be allocated in the pool.
  size_t capacity() const { return nof_objs; }

  /// Gets approximate number of objects currently allocated in the pool.
  size_t size_approx() const { return impl.size_approx(); }

private:
  void reclaim(unsigned idx) { impl.reclaim(idx); }

  const unsigned                            nof_objs;
  detail::bounded_object_pool_impl<segment> impl;
};

} // namespace srsran
