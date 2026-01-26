/*
 *
 * Copyright 2021-2026 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/adt/detail/intrusive_ptr.h"
#include "srsran/adt/noop_functor.h"
#include "srsran/support/cpu_architecture_info.h"
#include "srsran/support/math/math_utils.h"
#include <atomic>
#include <memory>
#include <sched.h>
#include <utility>

namespace srsran {

namespace detail {

/// Helper class to collect pool metrics.
template <bool Enabled = false>
class bounded_object_pool_metrics_collector
{
public:
  static bool metrics_active() { return false; }
  uint64_t    nof_alloc_reattempts() const { return 0; }
  uint64_t    nof_scanned_segments() const { return 0; }
  void        increment_alloc_reattempts() {}
  void        increment_scanned_segments() {}
};

/// Specialization when metrics collection is enabled.
template <>
class bounded_object_pool_metrics_collector<true>
{
  std::atomic<uint64_t> failed_fetch_ors{0};
  std::atomic<uint64_t> scanned_segment_counter{0};

public:
  static bool metrics_active() { return true; }
  uint64_t    nof_alloc_reattempts() const { return failed_fetch_ors.load(std::memory_order_relaxed); }
  uint64_t    nof_scanned_segments() const { return scanned_segment_counter.load(std::memory_order_relaxed); }
  void        increment_alloc_reattempts() { failed_fetch_ors.fetch_add(1, std::memory_order_relaxed); }
  void        increment_scanned_segments() { scanned_segment_counter.fetch_add(1, std::memory_order_relaxed); }
};

/// Helper class that implements the bounded object pool logic.
template <typename SegmentType, typename OnClear = noop_operation, bool MetricsEnabled = false>
class bounded_object_pool_impl : public bounded_object_pool_metrics_collector<MetricsEnabled>
{
public:
  using metrics_collector_type = bounded_object_pool_metrics_collector<MetricsEnabled>;

  /// Reclaimer functor that calls on_clear and reclaims the object back to the pool.
  struct obj_reclaimer {
    obj_reclaimer() = default;
    obj_reclaimer(bounded_object_pool_impl& parent_, uint32_t seg_idx_, uint32_t obj_idx_) :
      parent(&parent_), seg_idx(seg_idx_), obj_idx(obj_idx_)
    {
    }

    void operator()(typename SegmentType::value_type* val)
    {
      if (val != nullptr) {
        // Call on_clear before reclaiming the object.
        parent->on_clear(*val);
        parent->reclaim(seg_idx, obj_idx);
      }
    }

    bounded_object_pool_impl* parent  = nullptr;
    uint32_t                  seg_idx = static_cast<uint32_t>(-1);
    uint32_t                  obj_idx = static_cast<uint32_t>(-1);
  };

  /// Base segment structure that holds the bitmap and number of objects.
  struct base_segment {
    /// Bitmap to track allocated objects in the segment.
    std::atomic<uint64_t> busy_bitmap{0};
    /// Number of objects in the segment.
    unsigned nof_objs_in_segment;

    base_segment(unsigned nof_objects_) : nof_objs_in_segment(nof_objects_)
    {
      srsran_assert(size() > 0 and size() <= max_size(), "Number of objects exceeds segment capacity");

      if (this->size() < this->max_size()) {
        // Mark remaining positions of the bitmap as unavailable.
        auto       remaining        = max_size() - size();
        const auto mark_unavailable = mask_msb_ones<uint64_t>(remaining);
        this->busy_bitmap.fetch_or(mark_unavailable, std::memory_order_relaxed);
      }
    }
    base_segment(const base_segment&)            = delete;
    base_segment(base_segment&&)                 = delete;
    base_segment& operator=(const base_segment&) = delete;
    base_segment& operator=(base_segment&&)      = delete;

    size_t size() const { return nof_objs_in_segment; }

    static size_t max_size() { return 64; }
  };

  template <typename Factory, typename OnClearFunc = OnClear>
  bounded_object_pool_impl(unsigned nof_objects, const Factory& factory, OnClearFunc&& on_clear_) :
    nof_objs(nof_objects), on_clear(std::forward<OnClearFunc>(on_clear_))
  {
    static_assert(std::is_convertible_v<OnClear, std::function<void(typename SegmentType::value_type&)>>,
                  "OnClear must be callable with SegmentType::value_type& argument");
    report_fatal_error_if_not(capacity() > 0, "Number of objects must be greater than zero");
    const unsigned nof_segments = divide_ceil(capacity(), base_segment::max_size());

    // Compute the CPU offset scatter coefficient.
    const unsigned nof_available_cpus = cpu_architecture_info::get().get_host_nof_available_cpus();
    cpu_seg_offset_dist_coeff         = std::max(1U, nof_segments / nof_available_cpus);
    cpu_bit_shift_dist_coeff = std::max(1U, static_cast<unsigned>(base_segment::max_size()) / nof_available_cpus);

    // Initialize segments.
    segments.reserve(nof_segments);
    for (unsigned i = 0; i != nof_segments; ++i) {
      unsigned seg_nof_objs = base_segment::max_size();
      if (i == nof_segments - 1) {
        // Last segment may have fewer objects than the ones represented by the max bitmap size.
        seg_nof_objs = ((nof_objects - 1) % base_segment::max_size()) + 1;
      }
      segments.emplace_back(factory(i, seg_nof_objs));
    }
  }

  /// Maximum number of objects that can be allocated in the pool.
  [[nodiscard]] size_t capacity() const { return nof_objs; }

  /// Estimation of the current number of objects in the pool that can be allocated.
  [[nodiscard]] size_t size_approx() const
  {
    size_t sum_used = 0;
    for (unsigned i = 0; i != segments.size(); ++i) {
      uint64_t busy_bitmap = segments[i]->busy_bitmap.load(std::memory_order_relaxed);
      sum_used += count_ones(busy_bitmap);
    }
    return segments.size() * base_segment::max_size() - sum_used;
  }

  /// \brief Retrieves the segment and object index of a free object in the pool.
  std::pair<unsigned, unsigned> get()
  {
    const unsigned cpuid      = std::max(::sched_getcpu(), 0);
    unsigned       seg_offset = 0;
    if (segments.size() > 1) {
      seg_offset = (cpuid * cpu_seg_offset_dist_coeff) % segments.size();
    }
    const unsigned bit_shift = (cpuid * cpu_bit_shift_dist_coeff) % base_segment::max_size();

    for (unsigned i = 0; i != segments.size(); ++i) {
      unsigned     idx = (seg_offset + i) % segments.size();
      SegmentType& seg = *segments[idx];

      uint64_t busy_bitmap = seg.busy_bitmap.load(std::memory_order_acquire);
      if (busy_bitmap == static_cast<uint64_t>(-1)) {
        // No free objects in this segment.
        metrics_collector_type::increment_scanned_segments();
        continue;
      }

      // Find the first free object.
      while (true) {
        // Convert the busy bitmap into an available bitmap, but circularly rotated by the bit_shift amount.
        uint64_t available_rotated = ~((busy_bitmap >> bit_shift) | (busy_bitmap << (64 - bit_shift)));

        // Find the first bit set to one in the available bitmap.
        uint64_t obj_idx = find_first_lsb_one(available_rotated);
        if (obj_idx >= base_segment::max_size()) {
          // No free objects found in this segment.
          metrics_collector_type::increment_scanned_segments();
          break;
        }
        // Rotate the object index back to the original position.
        obj_idx = (obj_idx + bit_shift) % base_segment::max_size();

        // Mark the object as used and verify that it was not marked as used previously.
        busy_bitmap = seg.busy_bitmap.fetch_or((1ULL << obj_idx), std::memory_order_acq_rel);
        if ((busy_bitmap & (1ULL << obj_idx)) == 0) {
          // We successfully marked this position.
          return std::make_pair(idx, obj_idx);
        }
        metrics_collector_type::increment_alloc_reattempts();
      }
    }
    // No free objects available.
    return std::make_pair(static_cast<unsigned>(-1), static_cast<unsigned>(-1));
  }

  /// Reclaims an object back to the pool.
  /// \param seg_idx Segment index where the object is stored.
  /// \param obj_idx Object index within the segment.
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
  unsigned cpu_seg_offset_dist_coeff;
  /// Used to determine the bit shift distance between contiguous CPUs.
  unsigned cpu_bit_shift_dist_coeff;
  /// Functor called to clear objects before reclaiming them.
  OnClear on_clear;

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
template <typename T, typename OnClear = noop_operation, bool MetricsEnabled = false>
class bounded_object_pool
{
  /// Segment structure that holds objects in a C-style dynamic array.
  struct segment : public detail::bounded_object_pool_impl<T>::base_segment {
    using base_type  = typename detail::bounded_object_pool_impl<T>::base_segment;
    using value_type = T;

    /// Array of objects in the segment.
    T* objects;

    template <typename... Args>
    segment(unsigned nof_objects_, const Args&... args) : base_type(nof_objects_)
    {
      // Create objs.
      const auto sz = this->size();
      objects       = static_cast<T*>(::operator new(sz * sizeof(T)));
      for (unsigned i = 0; i != sz; ++i) {
        new (objects + i) T(args...);
      }
    }
    ~segment()
    {
      for (unsigned i = 0, sz = this->size(); i != sz; ++i) {
        objects[i].~T();
      }
      ::operator delete(objects);
    }
  };

  using pool_impl_type = detail::bounded_object_pool_impl<segment, OnClear, MetricsEnabled>;
  using obj_reclaimer  = typename pool_impl_type::obj_reclaimer;

public:
  using value_type = T;
  /// Unique pointer type that holds an object from the pool and reclaims it when the pointer goes out of scope.
  using ptr = std::unique_ptr<T, obj_reclaimer>;

  /// Creates an object pool with capacity equal to \c nof_objects.
  template <typename... Args>
  bounded_object_pool(size_t nof_objects, const Args&... args) : bounded_object_pool(OnClear{}, nof_objects, args...)
  {
  }
  template <typename OnClearFunc,
            typename... Args,
            std::enable_if_t<std::is_convertible_v<OnClearFunc, OnClear>, int> = 0>
  bounded_object_pool(OnClearFunc&& on_clear, size_t nof_objects, const Args&... args) :
    impl(
        nof_objects,
        [&args...](unsigned /* unused */, unsigned seg_size) { return std::make_unique<segment>(seg_size, args...); },
        std::forward<OnClearFunc>(on_clear))
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

  /// Collect metrics from the pool.
  bool     are_metrics_active() const { return impl.metrics_active(); }
  uint64_t nof_alloc_reattempts() const { return impl.nof_alloc_reattempts(); }
  uint64_t nof_scanned_segments() const { return impl.nof_scanned_segments(); }

private:
  pool_impl_type impl;
};

/// \brief A bounded thread-safe object pool. Unlike \c bounded_object_pool, this pool stores objects in unique_ptr<T>,
/// so it allows storing polymorphic objects.
template <typename T, typename OnClear = noop_operation, bool MetricsEnabled = false>
class bounded_unique_object_pool
{
  struct segment : public detail::bounded_object_pool_impl<T>::base_segment {
    using base_type  = typename detail::bounded_object_pool_impl<T>::base_segment;
    using value_type = T;

    /// Vector of unique pointers to objects in the segment.
    std::vector<std::unique_ptr<T>> objects;

    template <typename... Args>
    segment(unsigned nof_objects, const Args&... args) : base_type(nof_objects)
    {
      // Create obj.
      objects.reserve(nof_objects);
      for (unsigned i = 0; i != nof_objects; ++i) {
        objects.emplace_back(std::make_unique<T>(args...));
      }
    }
    segment(span<std::unique_ptr<T>> objects_) : base_type(objects_.size())
    {
      // Move already created objects to segment.
      objects.reserve(this->size());
      for (unsigned i = 0; i != this->size(); ++i) {
        objects.emplace_back(std::move(objects_[i]));
      }
    }
  };

  using pool_impl_type = detail::bounded_object_pool_impl<segment, OnClear, MetricsEnabled>;
  using obj_reclaimer  = typename pool_impl_type::obj_reclaimer;

public:
  using value_type             = T;
  using ptr                    = std::unique_ptr<T, obj_reclaimer>;
  using metrics_collector_type = detail::bounded_object_pool_metrics_collector<true>;

  /// Creates an object pool with the objects passed in the ctor.
  bounded_unique_object_pool(span<std::unique_ptr<T>> objects_) : bounded_unique_object_pool(OnClear{}, objects_) {}
  template <typename ClearFunc = OnClear, std::enable_if_t<std::is_convertible_v<ClearFunc, OnClear>, int> = 0>
  bounded_unique_object_pool(ClearFunc&& on_clear, span<std::unique_ptr<T>> objects_) :
    impl(
        objects_.size(),
        [&objects_](unsigned seg_idx, unsigned seg_nof_objs) {
          return std::make_unique<segment>(objects_.subspan(seg_idx * segment::max_size(), seg_nof_objs));
        },
        std::forward<ClearFunc>(on_clear))
  {
  }

  /// Creates an object pool with capacity equal to \c nof_objects.
  template <typename... Args>
  bounded_unique_object_pool(unsigned nof_objects, const Args&... args) :
    bounded_unique_object_pool(OnClear{}, nof_objects, args...)
  {
  }
  template <typename ClearFunc = OnClear,
            typename... Args,
            std::enable_if_t<std::is_convertible_v<ClearFunc, OnClear>, int> = 0>
  bounded_unique_object_pool(ClearFunc&& on_clear, size_t nof_objects, const Args&... args) :
    impl(
        nof_objects,
        [&args...](unsigned /* unused */, unsigned seg_nof_objs) {
          return std::make_unique<segment>(seg_nof_objs, args...);
        },
        std::forward<ClearFunc>(on_clear))
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

  /// Collect metrics from the pool.
  bool     are_metrics_active() const { return impl.metrics_active(); }
  uint64_t nof_alloc_reattempts() const { return impl.nof_alloc_reattempts(); }
  uint64_t nof_scanned_segments() const { return impl.nof_scanned_segments(); }

private:
  pool_impl_type impl;
};

/// \brief A bounded thread-safe object pool. Unlike \c bounded_unique_object_pool, this pool returns objects
/// whose lifetime is reference counted.
template <typename T, typename OnClear = noop_operation, bool MetricsEnabled = false>
class bounded_rc_object_pool
{
  /// Control block that holds the actual object and reference counting logic.
  struct obj_control_block {
    /// Actual pool object data.
    std::unique_ptr<T> obj;
    /// Parent pool.
    bounded_rc_object_pool& parent;
    /// Segment where object is saved.
    uint32_t seg_idx{std::numeric_limits<uint32_t>::max()};
    /// Offset within the segment where object is saved.
    uint32_t obj_idx{std::numeric_limits<uint32_t>::max()};
    /// Intrusive ptr reference counter.
    intrusive_ptr_atomic_ref_counter ref_count;

    obj_control_block(bounded_rc_object_pool& parent_) : parent(parent_) {}

    T& operator*() { return *obj; }

    friend void intrusive_ptr_inc_ref(obj_control_block* ptr) { ptr->ref_count.inc_ref(); }
    friend void intrusive_ptr_dec_ref(obj_control_block* ptr)
    {
      if (ptr->ref_count.dec_ref()) {
        ptr->reclaim();
      }
    }
    friend bool intrusive_ptr_is_unique(obj_control_block* ptr) { return ptr->ref_count.unique(); }

  private:
    void reclaim()
    {
      // Return object back to the pool.
      obj_reclaimer reclaimer{parent.impl, seg_idx, obj_idx};
      reclaimer(obj.get());
    }
  };

  struct segment : public detail::bounded_object_pool_impl<obj_control_block>::base_segment {
    using base_type  = typename detail::bounded_object_pool_impl<obj_control_block>::base_segment;
    using value_type = T;

    /// Array of control blocks in the segment.
    obj_control_block* objects;

    segment(bounded_rc_object_pool& parent, unsigned nof_objects_) : base_type(nof_objects_)
    {
      // Create objs.
      auto sz = this->size();
      objects = static_cast<obj_control_block*>(::operator new(sz * sizeof(obj_control_block)));
      for (unsigned i = 0; i != sz; ++i) {
        new (objects + i) obj_control_block{parent};
      }
    }
    ~segment()
    {
      for (unsigned i = 0, sz = this->size(); i != sz; ++i) {
        objects[i].~obj_control_block();
      }
      ::operator delete(objects);
    }
  };

  using pool_impl_type = detail::bounded_object_pool_impl<segment, OnClear, MetricsEnabled>;
  using obj_reclaimer  = typename pool_impl_type::obj_reclaimer;

public:
  using value_type             = T;
  using metrics_collector_type = detail::bounded_object_pool_metrics_collector<true>;

  /// Pointer type that holds a reference-counted object from the pool.
  class ptr
  {
    explicit ptr(intrusive_ptr<obj_control_block> ptr_) : rc(std::move(ptr_)) {}

  public:
    using value_type = T;

    ptr()                                = default;
    ptr(const ptr& other)                = delete;
    ptr(ptr&& other) noexcept            = default;
    ptr& operator=(const ptr& other)     = delete;
    ptr& operator=(ptr&& other) noexcept = default;

    T*       operator->() { return rc->obj.get(); }
    const T* operator->() const { return rc->obj.get(); }
    T&       operator*() { return *rc->obj; }
    const T& operator*() const { return *rc->obj; }
    T*       get() { return rc->obj.get(); }
    const T* get() const { return rc->obj.get(); }

    bool unique() const { return rc.unique(); }

    void reset() { return rc.reset(); }

    /// Creates a copy of the pointer, increasing the reference count.
    ptr clone() const { return ptr{rc}; }

    bool operator==(const ptr& other) const { return rc == other.rc; }
    bool operator==(std::nullptr_t) const { return rc == nullptr; }
    bool operator!=(const ptr& other) const { return rc != other.rc; }
    bool operator!=(std::nullptr_t) const { return rc != nullptr; }

    operator bool() const { return rc != nullptr; }

  private:
    friend class bounded_rc_object_pool;

    intrusive_ptr<obj_control_block> rc;
  };

  /// Creates an object pool with the objects passed in the ctor.
  bounded_rc_object_pool(span<std::unique_ptr<T>> objects_) : bounded_rc_object_pool(OnClear{}, objects_) {}
  template <typename ClearFunc = OnClear, std::enable_if_t<std::is_convertible_v<ClearFunc, OnClear>, int> = 0>
  bounded_rc_object_pool(ClearFunc&& on_clear, span<std::unique_ptr<T>> objects_) :
    impl(
        objects_.size(),
        [this, &objects_](unsigned seg_idx, unsigned seg_nof_objs) {
          auto     seg    = std::make_unique<segment>(*this, seg_nof_objs);
          unsigned offset = seg_idx * segment::max_size();
          for (unsigned i = 0, sz = seg_nof_objs; i != sz; ++i) {
            seg->objects[i].obj     = std::move(objects_[offset + i]);
            seg->objects[i].seg_idx = seg_idx;
            seg->objects[i].obj_idx = i;
          }
          return seg;
        },
        std::forward<ClearFunc>(on_clear))
  {
  }

  /// \brief Creates an object pool with capacity equal to \c nof_objects, filled with objects constructed using the
  /// provided \c args.
  template <typename... Args>
  bounded_rc_object_pool(unsigned nof_objects, const Args&... args) :
    bounded_rc_object_pool(OnClear{}, nof_objects, args...)
  {
  }
  template <typename ClearFunc = OnClear,
            typename... Args,
            std::enable_if_t<std::is_convertible_v<ClearFunc, OnClear>, int> = 0>
  bounded_rc_object_pool(ClearFunc&& on_clear, size_t nof_objects, const Args&... args) :
    impl(
        nof_objects,
        [this, &args...](unsigned seg_idx, unsigned seg_nof_objs) {
          auto seg = std::make_unique<segment>(*this, seg_nof_objs);
          for (unsigned i = 0, sz = seg_nof_objs; i != sz; ++i) {
            seg->objects[i].obj     = std::make_unique<T>(args...);
            seg->objects[i].seg_idx = seg_idx;
            seg->objects[i].obj_idx = i;
          }
          return seg;
        },
        std::forward<ClearFunc>(on_clear))
  {
  }

  /// \brief Retrieves a RAII pointer to an object of the pool.
  ptr get()
  {
    auto [seg_idx, obj_idx] = impl.get();
    if (seg_idx == static_cast<unsigned>(-1)) {
      // No free objects available.
      return ptr{nullptr};
    }
    return ptr{intrusive_ptr<obj_control_block>{impl.segments[seg_idx]->objects + obj_idx}};
  }

  /// Maximum number of objects that can be allocated in the pool.
  size_t capacity() const { return impl.capacity(); }

  /// Gets approximate number of objects currently allocated in the pool.
  size_t size_approx() const { return impl.size_approx(); }

  /// Collect metrics from the pool.
  bool     are_metrics_activate() const { return impl.metrics_active(); }
  uint64_t nof_alloc_reattempts() const { return impl.nof_alloc_reattempts(); }
  uint64_t nof_scanned_segments() const { return impl.nof_scanned_segments(); }

private:
  pool_impl_type impl;
};

} // namespace srsran
