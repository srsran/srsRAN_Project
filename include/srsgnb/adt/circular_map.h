
#ifndef SRSGNB_ADT_CIRCULAR_MAP_H
#define SRSGNB_ADT_CIRCULAR_MAP_H

#include "detail/type_storage.h"
#include "expected.h"
#include "srsgnb/support/srsran_assert.h"
#include <array>

namespace srsgnb {

/// Contiguous array of optional objects indexed circularly via a integer key. Random access, insertion and removal
/// should have complexity O(1).
/// Each container element contains 3 fields: (i) presence flag, (ii) key, (iii) stored object
/// In case the presence flag is set to true, the stored object exists and its ctor has been called.
/// The following steps are performed to access an element:
/// 1. The provided key K is converted to an array index via the operation I = K % N.
/// 2. The presence flag for the index I is checked. If present,
/// 3. Check if the provided key K corresponds to the key stored in the element. If the keys match,
/// 4. Retrieve the stored object.
/// The container does not resolve collisions, i.e. it doesn't allow two objects of different keys but same index
/// to be inserted in the container. It is up to the user to resolve such situations.
/// There is no pointer and iterator invalidation.
/// \tparam K type of key used to retrieve an element index. K must be an unsigned integer
/// \tparam T type of each element
/// \tparam N maximum number of elements
template <typename K, typename T, size_t N>
class circular_map
{
  static_assert(std::is_integral<K>::value and std::is_unsigned<K>::value, "Container key must be an unsigned integer");

  using obj_t = std::pair<K, T>;

public:
  using key_type        = K;
  using mapped_type     = T;
  using value_type      = std::pair<K, T>;
  using difference_type = std::ptrdiff_t;

  /// Iterator type. It automatically skips absent elements of the container
  class iterator
  {
  public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = std::pair<K, T>;
    using difference_type   = std::ptrdiff_t;
    using pointer           = value_type*;
    using reference         = value_type&;

    iterator() = default;
    iterator(circular_map<K, T, N>* map, size_t idx_) : ptr(map), idx(idx_)
    {
      if (idx < ptr->capacity() and not ptr->present[idx]) {
        ++(*this);
      }
    }

    iterator& operator++()
    {
      while (++idx < ptr->capacity() and not ptr->present[idx]) {
      }
      return *this;
    }

    obj_t& operator*()
    {
      srsran_assert(idx < ptr->capacity(), "Iterator out-of-bounds ({} >= {})", idx, ptr->capacity());
      return ptr->get_obj_(idx);
    }
    obj_t* operator->()
    {
      srsran_assert(idx < ptr->capacity(), "Iterator out-of-bounds ({} >= {})", idx, ptr->capacity());
      return &ptr->get_obj_(idx);
    }
    const obj_t* operator*() const
    {
      srsran_assert(idx < ptr->capacity(), "Iterator out-of-bounds ({} >= {})", idx, ptr->capacity());
      return &ptr->get_obj_(idx);
    }
    const obj_t* operator->() const
    {
      srsran_assert(idx < ptr->capacity(), "Iterator out-of-bounds ({} >= {})", idx, ptr->capacity());
      return &ptr->get_obj_(idx);
    }

    bool operator==(const iterator& other) const { return ptr == other.ptr and idx == other.idx; }
    bool operator!=(const iterator& other) const { return not(*this == other); }

  private:
    friend class circular_map<K, T, N>;
    circular_map<K, T, N>* ptr = nullptr;
    size_t                 idx = 0;
  };
  class const_iterator
  {
  public:
    const_iterator() = default;
    const_iterator(const circular_map<K, T, N>* map, size_t idx_) : ptr(map), idx(idx_)
    {
      if (idx < ptr->capacity() and not ptr->present[idx]) {
        ++(*this);
      }
    }

    const_iterator& operator++()
    {
      while (++idx < ptr->capacity() and not ptr->present[idx]) {
      }
      return *this;
    }

    const obj_t& operator*() const { return ptr->buffer[idx].get(); }
    const obj_t* operator->() const { return &ptr->buffer[idx].get(); }

    bool operator==(const const_iterator& other) const { return ptr == other.ptr and idx == other.idx; }
    bool operator!=(const const_iterator& other) const { return not(*this == other); }

  private:
    friend class circular_map<K, T, N>;
    const circular_map<K, T, N>* ptr = nullptr;
    size_t                       idx = 0;
  };

  constexpr circular_map() { std::fill(present.begin(), present.end(), false); }
  circular_map(const circular_map<K, T, N>& other) : present(other.present), count(other.count)
  {
    for (size_t idx = 0; idx < other.capacity(); ++idx) {
      if (present[idx]) {
        buffer[idx].emplace(other.get_obj_(idx));
      }
    }
  }
  circular_map(circular_map<K, T, N>&& other) noexcept : present(other.present), count(other.count)
  {
    for (size_t idx = 0; idx < other.capacity(); ++idx) {
      if (present[idx]) {
        buffer[idx].emplace(std::move(other.get_obj_(idx)));
      }
    }
    other.clear();
  }
  ~circular_map() { clear(); }
  circular_map& operator=(const circular_map<K, T, N>& other)
  {
    if (this == &other) {
      return *this;
    }
    for (size_t idx = 0; idx < other.capacity(); ++idx) {
      copy_if_present_helper(buffer[idx], other.buffer[idx], present[idx], other.present[idx]);
    }
    count   = other.count;
    present = other.present;
  }
  circular_map& operator=(circular_map<K, T, N>&& other) noexcept
  {
    for (size_t idx = 0; idx < other.capacity(); ++idx) {
      move_if_present_helper(buffer[idx], other.buffer[idx], present[idx], other.present[idx]);
    }
    count   = other.count;
    present = other.present;
    other.clear();
    return *this;
  }

  /// If an object with the given key exists
  bool contains(K key) const noexcept
  {
    size_t idx = key % N;
    return present[idx] and get_obj_(idx).first == key;
  }

  /// Constructs object with given key if no collision is detected.
  /// \param key key of created object
  /// \return true if no collision was detected and object was inserted. False, otherwise.
  template <typename... Args>
  bool emplace(K key, Args&&... args)
  {
    static_assert(std::is_constructible<T, Args...>::value, "Invalid argument types");
    size_t idx = key % N;
    if (present[idx]) {
      return false;
    }
    buffer[idx].emplace(
        std::piecewise_construct, std::forward_as_tuple(key), std::forward_as_tuple(std::forward<Args>(args)...));
    present[idx] = true;
    count++;
    return true;
  }

  bool insert(K key, const T& obj)
  {
    size_t idx = key % N;
    if (present[idx]) {
      return false;
    }
    buffer[idx].emplace(key, obj);
    present[idx] = true;
    count++;
    return true;
  }
  srsgnb::expected<iterator, T> insert(K key, T&& obj)
  {
    size_t idx = key % N;
    if (present[idx]) {
      return srsgnb::expected<iterator, T>(std::move(obj));
    }
    buffer[idx].emplace(key, std::move(obj));
    present[idx] = true;
    count++;
    return iterator(this, idx);
  }

  /// Insert object with provided key. If an object already exists with the same index, it gets overwritten.
  /// \param key key of the object to insert
  /// \param obj Object to insert
  template <typename U>
  void overwrite(K key, U&& obj)
  {
    size_t idx = key % N;
    if (present[idx]) {
      erase(buffer[idx].get().first);
    }
    insert(key, std::forward<U>(obj));
  }

  bool erase(K key) noexcept
  {
    if (not contains(key)) {
      return false;
    }
    size_t idx = key % N;
    get_obj_(idx).~obj_t();
    present[idx] = false;
    --count;
    return true;
  }

  iterator erase(iterator it) noexcept
  {
    srsran_assert(it.idx < N and it.ptr == this, "Iterator out-of-bounds ({} >= {})", it.idx, N);
    iterator next = it;
    ++next;
    present[it.idx] = false;
    get_obj_(it.idx).~obj_t();
    --count;
    return next;
  }

  void clear() noexcept
  {
    for (size_t i = 0; i < N; ++i) {
      if (present[i]) {
        present[i] = false;
        get_obj_(i).~obj_t();
      }
    }
    count = 0;
  }

  /// Random Access by key
  T& operator[](K key) noexcept
  {
    srsran_assert(contains(key), "Accessing non-existent ID={}", (size_t)key);
    return get_obj_(key % N).second;
  }
  const T& operator[](K key) const noexcept
  {
    srsran_assert(contains(key), "Accessing non-existent ID={}", (size_t)key);
    return get_obj_(key % N).second;
  }

  size_t        size() const noexcept { return count; }
  bool          empty() const noexcept { return count == 0; }
  bool          full() const noexcept { return count == N; }
  bool          has_space(K key) noexcept { return not present[key % N]; }
  static size_t capacity() { return N; }

  iterator       begin() { return iterator(this, 0); }
  iterator       end() { return iterator(this, N); }
  const_iterator begin() const { return const_iterator(this, 0); }
  const_iterator end() const { return const_iterator(this, N); }

  /// Find object iterator by key
  iterator find(K key)
  {
    if (contains(key)) {
      return iterator(this, key % N);
    }
    return end();
  }
  const_iterator find(K key) const
  {
    if (contains(key)) {
      return const_iterator(this, key % N);
    }
    return end();
  }

private:
  obj_t&       get_obj_(size_t idx) { return buffer[idx].get(); }
  const obj_t& get_obj_(size_t idx) const { return buffer[idx].get(); }

  std::array<detail::type_storage<obj_t>, N> buffer;
  std::array<bool, N>                        present;
  size_t                                     count = 0;
};

} // namespace srsgnb

#endif // SRSGNB_ADT_CIRCULAR_MAP_H
