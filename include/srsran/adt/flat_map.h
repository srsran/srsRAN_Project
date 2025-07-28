/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "srsran/support/srsran_assert.h"
#include <algorithm>
#include <stdexcept>
#include <vector>

namespace srsran {
namespace detail {

template <typename Comp>
struct flat_compare {
protected:
  flat_compare() = default;
  flat_compare(const Comp& comp_) : key_comp(comp_) {}

  Comp key_comp;
};

/// Helper representation of a key, value pair of flat_map that can be used during sorting.
template <typename T1, typename T2>
struct sort_ref {
  T1* first;
  T2* second;

  sort_ref(T1* first_, T2* second_) noexcept : first(first_), second(second_) {}
  sort_ref(sort_ref&&) noexcept = default;
  sort_ref& operator=(sort_ref&& other) noexcept
  {
    // Used when swapping sort_ref inside the std::sort.
    *first  = std::move(*other.first);
    *second = std::move(*other.second);
    return *this;
  }

  friend void swap(sort_ref lhs, sort_ref rhs) noexcept
  {
    std::swap(*lhs.first, *rhs.first);
    std::swap(*lhs.second, *rhs.second);
  }
};

template <typename T1, typename T2>
struct sort_value {
  T1 first;
  T2 second;

  sort_value(const T1& first_, const T2& second_) noexcept : first(first_), second(second_) {}
  sort_value(const sort_ref<T1, T2>& ref) noexcept : first(*ref.first), second(*ref.second) {}
  sort_value(sort_ref<T1, T2>&& ref) noexcept : first(std::move(*ref.first)), second(std::move(*ref.second)) {}
  sort_value& operator=(const sort_ref<T1, T2>& ref) noexcept
  {
    first  = *ref.first;
    second = *ref.second;
  }

  operator sort_ref<T1, T2>() { return sort_ref<T1, T2>(&first, &second); }
};

/// Helper iterator to sort both the keys and values container of the flat_map.
template <typename KeyContainer, typename MappedContainer>
struct sort_iter {
  typename KeyContainer::iterator    first;
  typename MappedContainer::iterator second;

  using value_type        = sort_value<typename KeyContainer::value_type, typename MappedContainer::value_type>;
  using difference_type   = std::ptrdiff_t;
  using pointer           = void;
  using reference         = sort_ref<typename KeyContainer::value_type, typename MappedContainer::value_type>;
  using iterator_category = std::random_access_iterator_tag;

  sort_iter(typename KeyContainer::iterator first_, typename MappedContainer::iterator second_) :
    first(first_), second(second_)
  {
  }

  sort_iter& operator++()
  {
    ++first;
    ++second;
    return *this;
  }

  sort_iter operator+(difference_type n) const { return sort_iter{first + n, second + n}; }

  difference_type operator-(const sort_iter& other) const { return first - other.first; }
  sort_iter&      operator--()
  {
    first -= 1;
    second -= 1;
    return *this;
  }
  sort_iter& operator-=(difference_type n)
  {
    first -= n;
    second -= n;
    return *this;
  }
  sort_iter operator-(difference_type n) const
  {
    sort_iter tmp = *this;
    tmp -= n;
    return tmp;
  }

  bool operator==(const sort_iter& other) const { return first == other.first; }
  bool operator!=(const sort_iter& other) const { return first != other.first; }
  bool operator<(const sort_iter& other) const { return first < other.first; }

  friend void iter_swap(sort_iter it1, sort_iter it2) noexcept
  {
    std::iter_swap(*it1.first, *it2.first);
    std::iter_swap(*it1.second, *it2.second);
  }

  reference operator*() const { return {&*first, &*second}; }
};

} // namespace detail
} // namespace srsran

namespace srsran {

template <typename Key,
          typename T,
          typename Compare         = std::less<Key>,
          typename KeyContainer    = std::vector<Key>,
          typename MappedContainer = std::vector<T>>
class flat_map : private detail::flat_compare<Compare>
{
  struct containers {
    KeyContainer    keys;
    MappedContainer values;

    containers() = default;
    containers(KeyContainer keys_, MappedContainer values_) : keys(std::move(keys_)), values(std::move(values_)) {}
  };

  template <bool IsConst>
  class iter_impl
  {
    using container_type = std::conditional_t<IsConst, const containers, containers>;

  public:
    using value_type        = std::pair<const Key, T>;
    using reference         = std::conditional_t<IsConst, std::pair<const Key&, const T&>, std::pair<const Key&, T&>>;
    using iterator_type     = iter_impl<IsConst>;
    using iterator_category = std::input_iterator_tag;
    using difference_type   = ptrdiff_t;

    struct pointer {
      reference        p;
      const reference* operator->() const noexcept { return &p; }
    };

    iter_impl()                                     = default;
    iter_impl(const iter_impl&) noexcept            = default;
    iter_impl(iter_impl&&) noexcept                 = default;
    iter_impl& operator=(const iter_impl&) noexcept = default;
    iter_impl& operator=(iter_impl&&) noexcept      = default;

    template <typename Iter                                                              = iter_impl<false>,
              std::enable_if_t<std::is_same_v<Iter, iter_impl<false>> and IsConst, bool> = true>
    iter_impl(const Iter& other) : parent(other.parent), index(other.index)
    {
    }

    reference operator*() const noexcept { return reference{parent->keys[index], parent->values[index]}; }
    pointer   operator->() const { return pointer{operator*()}; }

    reference operator[](std::size_t i) const noexcept { return *(*this + i); }

    iter_impl& operator++()
    {
      ++index;
      return *this;
    }
    iter_impl& operator--()
    {
      --index;
      return *this;
    }
    iter_impl operator++(int)
    {
      iter_impl tmp = *this;
      ++index;
      return tmp;
    }
    iter_impl operator--(int)
    {
      iter_impl tmp = *this;
      --index;
      return tmp;
    }
    iter_impl& operator+=(difference_type i)
    {
      index += i;
      return *this;
    }
    iter_impl& operator-=(difference_type i)
    {
      index -= i;
      return *this;
    }

    friend iter_impl operator+(iter_impl it, difference_type n) noexcept
    {
      it += n;
      return it;
    }
    friend iter_impl operator-(iter_impl it, difference_type n) noexcept
    {
      it -= n;
      return it;
    }
    friend iter_impl operator+(difference_type n, iter_impl it) noexcept
    {
      it += n;
      return it;
    }
    friend difference_type operator-(const iter_impl& lhs, const iter_impl& rhs) noexcept
    {
      srsran_assert(lhs.parent == rhs.parent, "Subtracting iterators from different containers");
      return lhs.index - rhs.index;
    }
    friend bool operator==(const iter_impl& lhs, const iter_impl& rhs) noexcept
    {
      srsran_assert(lhs.parent == rhs.parent, "Comparing iterators from different containers");
      return lhs.index == rhs.index;
    }
    friend bool operator!=(const iter_impl& lhs, const iter_impl& rhs) noexcept { return not(lhs == rhs); }
    friend bool operator<(const iter_impl& lhs, const iter_impl& rhs) noexcept
    {
      srsran_assert(lhs.parent == rhs.parent, "Comparing iterators from different containers");
      return lhs.index < rhs.index;
    }
    friend bool operator<=(const iter_impl& lhs, const iter_impl& rhs) noexcept { return (lhs < rhs) || (lhs == rhs); }
    friend bool operator>=(const iter_impl& lhs, const iter_impl& rhs) noexcept { return not(lhs < rhs); }
    friend bool operator>(const iter_impl& lhs, const iter_impl& rhs) noexcept
    {
      return not(lhs < rhs) and not(lhs == rhs);
    }
    friend void iter_swap(iter_impl it1, iter_impl it2)
    {
      std::iter_swap(it1.parent->keys.begin() + it1.index, it2.parent->keys.begin() + it2.index);
      std::iter_swap(it1.parent->values.begin() + it1.index, it2.parent->values.begin() + it2.index);
    }

  private:
    friend flat_map;
    friend iter_impl<not IsConst>;

    iter_impl(container_type* parent_, typename KeyContainer::const_iterator it) :
      parent(parent_), index(it - parent_->keys.cbegin())
    {
    }

    container_type* parent = nullptr;
    std::size_t     index  = -1;
  };

public:
  using key_type               = Key;
  using mapped_type            = T;
  using key_compare            = Compare;
  using value_type             = std::pair<const key_type, mapped_type>;
  using reference              = std::pair<const value_type&, mapped_type&>;
  using const_reference        = std::pair<const value_type&, const mapped_type&>;
  using size_type              = std::size_t;
  using difference_type        = std::ptrdiff_t;
  using key_container_type     = KeyContainer;
  using mapped_container_type  = MappedContainer;
  using iterator               = iter_impl<false>;
  using const_iterator         = iter_impl<true>;
  using reverse_iterator       = std::reverse_iterator<iterator>;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  flat_map() = default;
  explicit flat_map(const key_compare& comp) : detail::flat_compare<Compare>(comp) {}
  flat_map(key_container_type keys_, mapped_container_type values_, const key_compare& comp = key_compare()) :
    detail::flat_compare<Compare>(comp), conts(std::move(keys_), std::move(values_))
  {
    srsran_assert(conts.keys.size() == conts.values.size(), "Key and value containers must have the same size");
    sort_and_rem_duplicates();
  }

  size_type          size() const noexcept { return conts.keys.size(); }
  [[nodiscard]] bool empty() const noexcept { return conts.keys.empty(); }
  size_type          max_size() const noexcept { return std::min(conts.keys.capacity(), conts.values.capacity()); }

  void reserve(size_type n)
  {
    conts.keys.reserve(n);
    conts.values.reserve(n);
  }

  void clear() noexcept
  {
    conts.keys.clear();
    conts.values.clear();
  }

  const key_container_type&    keys() const noexcept { return conts.keys; }
  const mapped_container_type& values() const noexcept { return conts.values; }

  mapped_type& operator[](const key_type& key) { return emplace(key).first->second; }
  mapped_type& operator[](key_type&& key) { return emplace(std::move(key)).first->second; }

  mapped_type& at(const key_type& key)
  {
    auto it = find(key);
    if (it == end()) {
      throw std::out_of_range("Key not found");
    }
    return it->second;
  }
  const mapped_type& at(const key_type& key) const
  {
    auto it = find(key);
    if (it == cend()) {
      throw std::out_of_range("Key not found");
    }
    return it->second;
  }

  iterator find(const key_type& key) noexcept
  {
    auto key_it = std::lower_bound(conts.keys.begin(), conts.keys.end(), key, this->key_comp);
    if (key_it != conts.keys.end() and not this->key_comp(key, *key_it)) {
      return iterator{&conts, key_it};
    }
    return end();
  }
  const_iterator find(const key_type& key) const noexcept
  {
    auto it = std::lower_bound(conts.keys.cbegin(), conts.keys.cend(), key, this->key_comp);
    if (it != conts.keys.cend() and not this->key_comp(key, *it)) {
      return const_iterator{&conts, it};
    }
    return cend();
  }

  iterator lower_bound(const key_type& key) noexcept
  {
    auto it = std::lower_bound(conts.keys.begin(), conts.keys.end(), key, this->key_comp);
    return iterator{&conts, it};
  }
  const_iterator lower_bound(const key_type& key) const noexcept
  {
    auto it = std::lower_bound(conts.keys.cbegin(), conts.keys.cend(), key, this->key_comp);
    return const_iterator{&conts, it};
  }

  template <typename... Args>
  std::pair<iterator, bool> emplace(Args&&... args)
  {
    value_type p(std::forward<Args>(args)...);
    return emplace_impl(std::move(p.first), std::move(p.second));
  }

  std::pair<iterator, bool> insert(const value_type& value) { return emplace_impl(value.first, value.second); }
  std::pair<iterator, bool> insert(value_type&& value)
  {
    return emplace_impl(std::move(value.first), std::move(value.second));
  }
  iterator insert(const_iterator pos, const value_type& value)
  {
    return emplace_hint_impl(pos, value.first, value.second).first;
  }
  iterator insert(const_iterator pos, value_type&& value)
  {
    return emplace_hint_impl(pos, std::move(value.first), std::move(value.second)).first;
  }

  iterator erase(iterator pos_it) { return erase(static_cast<const_iterator>(pos_it)); }
  iterator erase(const_iterator pos_it)
  {
    auto index  = pos_it.index;
    auto key_it = conts.keys.erase(conts.keys.begin() + index);
    conts.values.erase(conts.values.begin() + index);
    return iterator{&conts, key_it};
  }

  size_type erase(const key_type& key)
  {
    auto it = find(key);
    if (it == end()) {
      return 0;
    }
    erase(it);
    return 1;
  }

  iterator               begin() noexcept { return iterator{&conts, conts.keys.cbegin()}; }
  const_iterator         begin() const noexcept { return {&conts, conts.keys.cbegin()}; }
  iterator               end() noexcept { return {&conts, conts.keys.cend()}; }
  const_iterator         end() const noexcept { return {&conts, conts.keys.cend()}; }
  reverse_iterator       rbegin() noexcept { return reverse_iterator(end()); }
  const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
  reverse_iterator       rend() noexcept { return reverse_iterator(begin()); }
  const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
  const_iterator         cbegin() const noexcept { return begin(); }
  const_iterator         cend() const noexcept { return end(); }
  const_reverse_iterator crbegin() const noexcept { return rbegin(); }
  const_reverse_iterator crend() const noexcept { return rend(); }

  void swap(flat_map& other) noexcept
  {
    std::swap(this->comp, other.comp);
    std::swap(conts.keys, other.conts.keys);
    std::swap(conts.values, other.conts.values);
  }

  friend void swap(flat_map& lhs, flat_map& rhs) noexcept { lhs.swap(rhs); }

  bool operator==(const flat_map& other) const noexcept
  {
    return std::equal(begin(), end(), other.begin(), other.end());
  }
  bool operator!=(const flat_map& other) const noexcept { return not(*this == other); }

private:
  auto value_comp() const
  {
    struct value_comparator : public Compare {
      value_comparator(const Compare& c) : Compare(c) {}
      bool operator()(const value_type& lhs, const value_type& rhs) const { return lhs.first < rhs.first; }
    };
    return value_comparator{this->key_comp};
  }

  template <typename Key2, typename... Args>
  std::pair<iterator, bool> emplace_impl(Key2&& key, Args&&... args)
  {
    auto key_it = std::lower_bound(conts.keys.begin(), conts.keys.end(), key, this->key_comp);
    if (key_it != conts.keys.end() and not this->key_comp(key, *key_it)) {
      return {iterator{&conts, key_it}, false};
    }
    key_it        = conts.keys.insert(key_it, std::forward<Key2>(key));
    auto value_it = conts.values.begin() + (key_it - conts.keys.begin());
    conts.values.emplace(value_it, std::forward<Args>(args)...);
    return {iterator{&conts, key_it}, true};
  }
  template <typename Key2, typename... Args>
  std::pair<iterator, bool> emplace_hint_impl(const_iterator pos, Key2&& key, Args&&... args)
  {
    typename key_container_type::iterator key_it;
    int                                   r = -1;
    int                                   s = -1;
    if ((pos == cbegin() or (r = not this->key_comp(key, pos[-1].first))) and
        (pos == cend() or (s = not this->key_comp(pos[0].first, key)))) {
      key_it = conts.keys.begin() + pos.index;
      if (r == 1 and not this->key_comp(key_it[-1], key)) {
        return {iterator{&conts, key_it - 1}, false};
      }
    } else {
      auto first = conts.keys.begin();
      auto last  = conts.keys.end();
      if (r == 1) {
        first += pos.index;
      } else if (r == 0) {
        last = first + pos.index;
      }
      key_it = std::lower_bound(first, last, key, this->key_comp);
    }

    if (key_it != conts.keys.end() and not this->key_comp(key, *key_it)) {
      // Key already exists.
      return {iterator{&conts, key_it}, false};
    }

    // Emplace is successful.
    key_it = conts.keys.insert(key_it, std::forward<Key2>(key));
    conts.values.emplace(conts.values.begin() + (key_it - conts.keys.begin()), std::forward<Args>(args)...);
    return {iterator{&conts, key_it}, true};
  }

  void sort_and_rem_duplicates()
  {
    using sort_iter = detail::sort_iter<key_container_type, mapped_container_type>;
    using sort_ref  = detail::sort_ref<key_type, mapped_type>;

    auto b = sort_iter{conts.keys.begin(), conts.values.begin()};
    auto e = sort_iter{conts.keys.end(), conts.values.end()};
    std::sort(
        b, e, [this](const sort_ref& lhs, const sort_ref& rhs) { return this->key_comp(*lhs.first, *rhs.first); });
    auto last = std::unique(b, e, [this](const sort_ref& lhs, const sort_ref& rhs) {
      return not this->key_comp(*lhs.first, *rhs.first) and not this->key_comp(*rhs.first, *lhs.first);
    });
    conts.keys.erase(last.first, conts.keys.end());
    conts.values.erase(last.second, conts.values.end());
  }

  containers conts;
};

} // namespace srsran
