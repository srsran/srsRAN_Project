/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SRSVEC_ALIGNED_VEC_H
#define SRSGNB_SRSVEC_ALIGNED_VEC_H

#include "srsgnb/adt/span.h"

namespace srsgnb {
namespace srsvec {

namespace detail {
void* mem_alloc(std::size_t size);
void  mem_free(void* ptr);
} // namespace detail

/// Type to store a dynamic amount of aligned contiguous elements
template <typename T>
class aligned_vec : public span<T>
{
  void dealloc() { detail::mem_free(this->data()); }

public:
  aligned_vec<T>& operator=(aligned_vec<T>&& other) = delete;
  aligned_vec<T>& operator=(const aligned_vec<T>& other) = delete;
  aligned_vec(const aligned_vec<T>& other)               = delete;
  aligned_vec(aligned_vec<T>&& other) noexcept           = delete;

  aligned_vec() : span<T>(nullptr, 0UL){};
  explicit aligned_vec(std::size_t size) { resize(size); }

  void resize(unsigned new_size)
  {
    if (new_size == this->size()) {
      return;
    }
    dealloc();

    T*      ptr_    = (T*)detail::mem_alloc(sizeof(T) * new_size);
    span<T> o       = span<T>(ptr_, new_size);
    *(span<T>*)this = o;
  }

  ~aligned_vec() { dealloc(); }
};

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_ALIGNED_VEC_H