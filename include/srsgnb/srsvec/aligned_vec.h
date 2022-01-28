/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
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
private:
  bool owner = false; // Indicates if the data can be deallocated at destructors

  void dealloc()
  {
    if (this->data() != nullptr and owner) {
      detail::mem_free(this->data());
    }
  }
  aligned_vec<T>& operator=(aligned_vec<T>&& other) = default;
  explicit aligned_vec(std::size_t size, bool owner_) :
    span<T>((T*)detail::mem_alloc(sizeof(T) * size), size), owner(owner_)
  {}

public:
  aligned_vec<T>& operator=(const aligned_vec<T>& other) = delete;
  aligned_vec(const aligned_vec<T>& other)               = delete;
  aligned_vec(aligned_vec<T>&& other) noexcept           = delete;

  aligned_vec() = default;
  explicit aligned_vec(std::size_t size) : aligned_vec(size, true) {}

  void resize(unsigned new_size)
  {
    if (new_size == this->size()) {
      return;
    }
    dealloc();

    *this       = aligned_vec<T>(new_size, false);
    this->owner = true;
  }

  ~aligned_vec() { dealloc(); }
};

} // namespace srsvec
} // namespace srsgnb

#endif // SRSGNB_SRSVEC_ALIGNED_VEC_H