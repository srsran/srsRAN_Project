/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_STABLE_ID_MAP_H
#define SRSGNB_STABLE_ID_MAP_H

#include <array>
#include <memory>

namespace srsgnb {

/// \brief Container type that associates Id types to container elements.
/// Pointer/References/Iterators remain valid throughout the object lifetime.
/// This container is efficient memory-wise, avoiding reserving space for elements T before those elements are
/// actually created.
/// \tparam IdType
/// \tparam T
/// \tparam MAX_SIZE
template <typename IdType, typename T, size_t MAX_SIZE>
class stable_id_map
{
  using container_t = std::array<std::unique_ptr<T>, MAX_SIZE>;

  template <typename Data>
  class iter_impl
  {
    using iter_t = std::
        conditional_t<std::is_const<Data>::value, typename container_t::const_iterator, typename container_t::iterator>;

  public:
    using value_type        = Data;
    using reference         = Data&;
    using pointer           = Data*;
    using difference_type   = std::ptrdiff_t;
    using iterator_category = std::forward_iterator_tag;

    iter_impl() = default;
    iter_impl(iter_t it_, iter_t it_end_) : it(it_), it_end(it_end_) { next_valid_it(); }

    iter_impl<Data>& operator++()
    {
      ++it;
      return *this;
    }

    reference operator*() { return **it; }
    pointer   operator->() { return &(**it); }
    reference operator*() const { return **it; }
    pointer   operator->() const { return &(**it); }

    bool operator==(const iter_impl<Data>& other) const { return it == other.it; }
    bool operator!=(const iter_impl<Data>& other) const { return it != other.it; }

  private:
    void next_valid_it()
    {
      while (it != it_end and (*it) == nullptr) {
        ++it;
      }
    }

    iter_t it;
    iter_t it_end;
  };

public:
  using value_type     = T;
  using iterator       = iter_impl<T>;
  using const_iterator = iter_impl<const T>;

  bool contains(IdType id) const
  {
    srsran_sanity_check(id < MAX_SIZE, "Invalid Id={}", id);
    return elems[id] != nullptr;
  }

  void insert(IdType id_value, std::unique_ptr<T> u)
  {
    srsran_sanity_check(not contains(id_value), "Id={} already exists", id_value);
    elems[id_value] = std::move(u);
    ++nof_elems;
  }

  template <typename... Args>
  void emplace(IdType id_value, Args&&... args)
  {
    srsran_sanity_check(not contains(id_value), "Id={} already exists", id_value);
    elems[id_value] = std::make_unique<T>(std::forward<Args>(args)...);
    ++nof_elems;
  }

  bool erase(IdType id_value)
  {
    if (not contains(id_value)) {
      return false;
    }
    elems[id_value].reset();
    nof_elems--;
    return true;
  }

  void clear()
  {
    for (auto& u : elems) {
      u.reset();
    }
    nof_elems = 0;
  }

  T& operator[](IdType id_value)
  {
    srsran_sanity_check(contains(id_value), "Id={} does not exist", id_value);
    return *elems[id_value];
  }

  const T& operator[](IdType id_value) const
  {
    srsran_sanity_check(contains(id_value), "Id={} does not exist", id_value);
    return *elems[id_value];
  }

  size_t size() const { return nof_elems; }
  bool   empty() const { return nof_elems == 0; }

  iterator       begin() { return {elems.begin(), elems.end()}; }
  iterator       end() { return {elems.end(), elems.end()}; }
  const_iterator begin() const { return {elems.begin(), elems.end()}; }
  const_iterator end() const { return {elems.end(), elems.end()}; }

  iterator lower_bound(IdType id) { return {elems.begin() + id, elems.end()}; }
  const_iterator lower_bound(IdType id) const { return {elems.begin() + id, elems.end()}; }

private:
  size_t                                   nof_elems = 0;
  std::array<std::unique_ptr<T>, MAX_SIZE> elems;
};

} // namespace srsgnb

#endif // SRSGNB_STABLE_ID_MAP_H
