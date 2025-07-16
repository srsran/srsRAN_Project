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

namespace srsran {

template <typename T>
class bounded_bitmap_object_pool
{
  struct obj_reclaimer {
    obj_reclaimer() = default;
    obj_reclaimer(bounded_bitmap_object_pool& parent_, uint64_t pos_) : parent(&parent_), pos(pos_) {}

    void operator()(T* ptr)
    {
      if (ptr != nullptr) {
        parent->reclaim(pos);
      }
    }

    bounded_bitmap_object_pool* parent = nullptr;
    uint64_t                    pos    = 0;
  };

public:
  using ptr = std::unique_ptr<T, obj_reclaimer>;

  template <typename... Args>
  bounded_bitmap_object_pool(size_t min_nof_objects, Args... args)
  {
    unsigned nof_segments = divide_ceil(min_nof_objects, segment::capacity());
    for (unsigned i = 0; i != nof_segments; ++i) {
      segments.emplace_back(std::make_unique<segment>(args...));
    }
  }

  ptr get()
  {
    unsigned offset = 0;
    if (segments.size() > 1) {
      offset = std::hash<std::thread::id>{}(std::this_thread::get_id()) % segments.size();
    }

    for (unsigned i = 0; i != segments.size(); ++i) {
      unsigned idx = (offset + i) % segments.size();
      segment& seg = *segments[idx];

      uint64_t used_bitmap = seg.used_bitmap.load(std::memory_order_acquire);
      if (used_bitmap == static_cast<uint64_t>(-1)) {
        continue; // No free objects in this segment.
      }

      // Find the first free object.
      while (true) {
        unsigned obj_idx = find_first_lsb_one(~used_bitmap);
        if (obj_idx >= segment::capacity()) {
          // No free objects found in this segment.
          break;
        }
        // Mark the object as used.
        used_bitmap = seg.used_bitmap.fetch_or((1ULL << obj_idx), std::memory_order_acq_rel);
        if ((used_bitmap & (1ULL << obj_idx)) == 0) {
          // We successfully marked this position.
          return ptr(seg.objects + obj_idx, obj_reclaimer{*this, idx * segment::capacity() + obj_idx});
        }
      }
    }
    return {nullptr, obj_reclaimer{}}; // No free objects available.
  }

  size_t capacity() const { return segments.size() * segment::capacity(); }

private:
  struct segment {
    std::atomic<uint64_t> used_bitmap{0};
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

  void reclaim(unsigned idx)
  {
    unsigned segment_idx = idx / segment::capacity();
    unsigned obj_idx     = idx % segment::capacity();
    srsran_assert(segment_idx < segments.size(), "Invalid segment index");

    segment& seg  = *segments[segment_idx];
    uint64_t prev = seg.used_bitmap.fetch_and(~(1ULL << obj_idx), std::memory_order_release);
    srsran_assert(prev & (1ULL << obj_idx), "Reclaiming an object that was not allocated");
  }

  std::vector<std::unique_ptr<segment>> segments;
};

} // namespace srsran
