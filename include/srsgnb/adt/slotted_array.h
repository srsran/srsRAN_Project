
#pragma once

#include "span.h"
#include "tiny_optional.h"
#include "srsgnb/support/srsgnb_assert.h"
#include <array>

/// \file Definitions of slotted_array<T, N>, slotted_vector<T> used to manage containers with optional elements. All
/// the defined classes provide methods to access, remove, add elements by index with O(1) complexity, iterators that
/// skip absent elements, and track number of present elements.

namespace srsgnb {

namespace detail {

/// Iterator implementation for an array of optional types that automatically skips positions without a value.
template <typename ArrayOfOpts>
class slotted_array_iter_impl
{
  using iterator_type = slotted_array_iter_impl<ArrayOfOpts>;

public:
  using iterator_category = std::forward_iterator_tag;
  using value_type        = typename ArrayOfOpts::value_type::value_type;
  using difference_type   = std::ptrdiff_t;
  using reference         = std::conditional_t<std::is_const<ArrayOfOpts>::value, const value_type, value_type>&;
  using pointer           = std::conditional_t<std::is_const<ArrayOfOpts>::value, const value_type, value_type>*;

  slotted_array_iter_impl() = default;
  slotted_array_iter_impl(ArrayOfOpts& vec_, size_t idx_) : vec(&vec_), idx(idx_)
  {
    while (idx < vec->size() and not(*vec)[idx].has_value()) {
      ++idx;
    }
  }
  template <typename U, std::enable_if_t<not std::is_same<ArrayOfOpts, U>::value, int> = 0>
  slotted_array_iter_impl(const slotted_array_iter_impl<U>& other) : vec(other.vec), idx(other.idx)
  {
  }

  template <typename U, std::enable_if_t<not std::is_same<ArrayOfOpts, U>::value, int> = 0>
  slotted_array_iter_impl& operator=(const slotted_array_iter_impl<U>& other)
  {
    vec = other.vec;
    idx = other.idx;
    return *this;
  }

  reference operator*() { return (*vec)[idx].value(); }
  reference operator*() const { return (*vec)[idx].value(); }
  pointer   operator->() { return &(*vec)[idx].value(); }
  pointer   operator->() const { return &(*vec)[idx].value(); }

  iterator_type& operator++()
  {
    while (++idx < vec->size() and not(*vec)[idx].has_value()) {
    }
    return *this;
  }

  iterator_type& operator--()
  {
    while (--idx < vec->size() and not(*vec)[idx].has_value()) {
    }
    return *this;
  }

  template <typename U>
  bool operator==(const slotted_array_iter_impl<U>& other) const
  {
    return idx == other.idx and vec == other.vec;
  }
  template <typename U>
  bool operator!=(const slotted_array_iter_impl<U>& other) const
  {
    return not(*this == other);
  }

private:
  template <typename Vec>
  friend class base_slotted_array_view;
  template <typename U>
  friend class slotted_array_iter_impl;

  ArrayOfOpts* vec = nullptr;
  size_t       idx = std::numeric_limits<size_t>::max();
};

/// Iterator implementation for a vector of optional types that automatically skips positions without a value.
template <typename VectorData>
class slotted_vector_iter_impl
{
  using iterator_type = slotted_vector_iter_impl<VectorData>;
  using elem_type     = typename VectorData::value_type;

public:
  using iterator_category = std::forward_iterator_tag;
  using value_type        = std::remove_const_t<elem_type>;
  using difference_type   = std::ptrdiff_t;
  using pointer           = elem_type*;
  using reference         = elem_type&;

  slotted_vector_iter_impl() = default;
  slotted_vector_iter_impl(VectorData& vec_, const std::vector<size_t>& index_mapper_, size_t idx_) noexcept :
    vec(&vec_), index_mapper(&index_mapper_), idx(idx_)
  {
    while (idx < index_mapper->size() and (*index_mapper)[idx] == std::numeric_limits<size_t>::max()) {
      ++idx;
    }
  }
  template <typename OtherVectorData, std::enable_if_t<not std::is_same<OtherVectorData, VectorData>::value, int> = 0>
  slotted_vector_iter_impl(const slotted_vector_iter_impl<OtherVectorData>& other) noexcept :
    vec(other.vec), index_mapper(other.index_mapper), idx(other.idx)
  {
  }

  template <typename OtherVectorData, std::enable_if_t<not std::is_same<OtherVectorData, VectorData>::value, int> = 0>
  slotted_vector_iter_impl& operator=(const slotted_vector_iter_impl<OtherVectorData>& other) noexcept
  {
    vec          = other.vec;
    index_mapper = other.index_mapper;
    idx          = other.idx;
    return *this;
  }

  reference operator*() { return (*vec)[(*index_mapper)[idx]]; }
  pointer   operator->() { return &(*vec)[(*index_mapper)[idx]]; }

  iterator_type& operator++()
  {
    while (++idx < index_mapper->size() and (*index_mapper)[idx] == std::numeric_limits<size_t>::max()) {
    }
    return *this;
  }

  iterator_type& operator--()
  {
    while (--idx < index_mapper->size() and (*index_mapper)[idx] == std::numeric_limits<size_t>::max()) {
    }
    return *this;
  }

  template <typename U>
  bool operator==(const slotted_vector_iter_impl<U>& other) const
  {
    return idx == other.idx and vec == other.vec and index_mapper == other.index_mapper;
  }
  template <typename U>
  bool operator!=(const slotted_vector_iter_impl<U>& other) const
  {
    return not(*this == other);
  }

private:
  template <typename U>
  friend class slotted_vector_iter_impl;

  VectorData*                vec          = nullptr;
  const std::vector<size_t>* index_mapper = nullptr;
  size_t                     idx          = std::numeric_limits<size_t>::max();
};

struct cast_to_size_operator {
  template <typename T>
  constexpr size_t operator()(const T& t) const
  {
    return static_cast<size_t>(t);
  }
};

} // namespace detail

/// Array of optional elements. The iteration is in order of indexes and correctly skips absent elements.
/// Pointer/References/Iterators remain valid throughout the object lifetime.
/// NOTE: The sorted iteration and pointer validation guarantees may add some overhead if the array is very fragmented.
/// @tparam T type of objects
/// @tparam N static size of max nof items
template <typename T, size_t N>
class slotted_array
{
  using array_type = std::array<tiny_optional<T>, N>;

public:
  using value_type     = T;
  using iterator       = detail::slotted_array_iter_impl<array_type>;
  using const_iterator = detail::slotted_array_iter_impl<const array_type>;

  bool contains(size_t idx) const noexcept { return idx < vec.size() and vec[idx].has_value(); }

  T& operator[](size_t idx) noexcept
  {
    srsgnb_assert(contains(idx), "Bad access for element with index={}", idx);
    return *vec[idx];
  }
  const T& operator[](size_t idx) const noexcept
  {
    srsgnb_assert(contains(idx), "Bad access for element with index={}", idx);
    return *vec[idx];
  }

  constexpr bool   empty() const noexcept { return nof_elems == 0; }
  constexpr size_t size() const noexcept { return nof_elems; }

  iterator       begin() { return iterator{vec, 0}; }
  iterator       end() { return iterator{vec, vec.size()}; }
  const_iterator begin() const { return const_iterator{vec, 0}; }
  const_iterator end() const { return const_iterator{vec, vec.size()}; }

  /// Insert given object in given index
  /// \param idx Position of the constructed element in the array
  /// \param u object to insert
  template <typename U>
  void insert(size_t idx, U&& u)
  {
    static_assert(std::is_convertible<U, value_type>::value, "Ctor T(U&&) does not exist.");
    srsgnb_assert(idx < this->vec.size(), "Out-of-bounds access to array: {}>={}", idx, this->vec.size());
    this->nof_elems += this->contains(idx) ? 0 : 1;
    this->vec[idx] = std::forward<U>(u);
  }

  /// Overwrite array element with given index with a newly constructed object
  /// \param idx Position of the constructed element in the array
  /// \param args Arguments to pass to element ctor
  template <typename... Args>
  void emplace(size_t idx, Args&&... args)
  {
    static_assert(std::is_constructible<value_type, Args&&...>::value, "Ctor T(Args...) does not exist.");
    srsgnb_assert(idx < this->vec.size(), "Out-of-bounds access to array: {}>={}", idx, this->vec.size());
    this->nof_elems += this->contains(idx) ? 0 : 1;
    this->vec[idx].emplace(std::forward<Args>(args)...);
    srsgnb_assert(this->vec[idx].has_value(), "Inserted object must be represent an optional with value");
  }

  /// Erase object pointed by the given index
  /// \param idx Position of the erased element in the array
  bool erase(size_t idx) noexcept
  {
    srsgnb_assert(idx < this->vec.size(), "Out-of-bounds access to array: {}>={}", idx, this->vec.size());
    if (this->contains(idx)) {
      this->nof_elems--;
      this->vec[idx].reset();
      return true;
    }
    return false;
  }

  /// Erase object pointed by the given iterator. Iterator must point to valid element
  /// \param it container iterator
  void erase(iterator it) noexcept { erase(this->extract_iterator_index(it)); }

  /// Clear all elements of the container
  void clear() noexcept
  {
    this->nof_elems = 0;
    for (auto& e : this->vec) {
      e.reset();
    }
  }

  /// Find first position that is empty
  size_t find_first_empty(size_t start_guess = 0) const
  {
    srsgnb_assert(start_guess < vec.size(), "invalid first guess array index");
    if (nof_elems == vec.size()) {
      return vec.size();
    }
    for (size_t i = start_guess; i < vec.size(); ++i) {
      if (not vec[i].has_value()) {
        return i;
      }
    }
    return vec.size();
  }

private:
  template <typename It>
  static size_t extract_iterator_index(It it)
  {
    return it.idx;
  }

  size_t     nof_elems = 0; ///< Number of present fields
  array_type vec;           ///< Container to store optional elements
};

/// \brief Container representing a vector of optional elements. The index of an inserted element, remains valid
/// throughout the element lifetime.
///
/// Contrarily to slotted_array, this container may allocate and cause pointer/reference/iterator invalidation. On the
/// other hand, it has the advantage of providing a more compact representation of its content.
template <typename T>
class slotted_vector
{
  constexpr static size_t absent_value = std::numeric_limits<size_t>::max();

public:
  using value_type     = T;
  using iterator       = detail::slotted_vector_iter_impl<std::vector<T>>;
  using const_iterator = detail::slotted_vector_iter_impl<const std::vector<T>>;

  slotted_vector() = default;
  slotted_vector(slotted_vector&& other) noexcept :
    objects(std::move(other.objects)), index_mapper(std::move(other.index_mapper))
  {
  }

  slotted_vector<T>& operator=(const slotted_vector<T>&) = default;
  slotted_vector<T>& operator=(slotted_vector<T>&& other) noexcept
  {
    objects      = std::move(other.objects);
    index_mapper = std::move(other.index_mapper);
    return *this;
  }

  constexpr bool empty() const noexcept { return objects.empty(); }

  constexpr size_t size() const noexcept { return objects.size(); }

  bool contains(size_t idx) const { return idx < index_mapper.size() and index_mapper[idx] != absent_value; }

  T& operator[](size_t idx) noexcept
  {
    srsgnb_assert(contains(idx), "Bad access for element with index={}", idx);
    return objects[index_mapper[idx]];
  }

  const T& operator[](size_t idx) const noexcept
  {
    srsgnb_assert(contains(idx), "Bad access for element with index={}", idx);
    return objects[index_mapper[idx]];
  }

  /// May allocate and cause pointer invalidation
  template <typename... Args>
  void emplace(size_t idx, Args&&... args)
  {
    static_assert(std::is_constructible<value_type, Args&&...>::value, "Ctor T(Args...) does not exist.");
    if (contains(idx)) {
      objects[index_mapper[idx]] = T(std::forward<Args>(args)...);
      return;
    }
    objects.emplace_back(std::forward<Args>(args)...);
    if (idx >= index_mapper.size()) {
      index_mapper.resize(idx + 1, (size_t)absent_value);
    }
    index_mapper[idx] = objects.size() - 1;
  }

  /// \brief Insert given object in given index. May allocate and cause pointer invalidation
  /// \param idx Position of the constructed element in the array
  /// \param u object to insert
  template <typename U>
  void insert(size_t idx, U&& u)
  {
    static_assert(std::is_convertible<U, value_type>::value, "Ctor T(U&&) does not exist.");
    emplace(idx, std::forward<U>(u));
  }

  /// Erase object pointed by the given index.
  bool erase(size_t idx)
  {
    if (not contains(idx)) {
      return false;
    }
    if (index_mapper[idx] < objects.size() - 1) {
      std::swap(objects[index_mapper[idx]], objects.back());
      auto it = std::find(index_mapper.begin(), index_mapper.end(), objects.size() - 1U);
      srsgnb_assert(it != index_mapper.end(), "Container in invalid state");
      *it = index_mapper[idx];
    }
    objects.pop_back();
    index_mapper[idx] = absent_value;
    if (idx == index_mapper.size() - 1) {
      while (not index_mapper.empty() and index_mapper.back() == absent_value) {
        index_mapper.pop_back();
      }
    }
    return true;
  }

  /// Erase object pointed by the given iterator. Iterator must point to valid element.
  /// \param it container iterator
  void erase(iterator it) noexcept { erase(this->extract_iterator_index(it)); }

  /// Clear all elements of the container
  void clear() noexcept
  {
    objects.clear();
    index_mapper.clear();
  }

  /// Find first position that is empty in slotted_vector.
  size_t find_first_empty(size_t start_guess = 0) const
  {
    if (objects.size() == index_mapper.size()) {
      return index_mapper.size();
    }
    for (size_t i = start_guess; i < index_mapper.size(); ++i) {
      if (index_mapper[i] == absent_value) {
        return i;
      }
    }
    return objects.size();
  }

  iterator       begin() { return iterator{objects, index_mapper, 0}; }
  iterator       end() { return iterator{objects, index_mapper, index_mapper.size()}; }
  const_iterator begin() const { return const_iterator{objects, index_mapper, 0}; }
  const_iterator end() const { return const_iterator{objects, index_mapper, index_mapper.size()}; }

private:
  std::vector<T>      objects;
  std::vector<size_t> index_mapper;
};

/// \brief Represents a slotted array that is indexed via an ID type that is convertible to an integer (e.g. enum).
template <typename IdType, typename T, size_t N, typename IdToIntConversion = detail::cast_to_size_operator>
class slotted_id_table : private IdToIntConversion
{
  static_assert(std::is_convertible<decltype(std::declval<IdToIntConversion>()(std::declval<IdType>())), size_t>::value,
                "IdType must be convertible to size_t");

public:
  using key_type       = IdType;
  using value_type     = typename slotted_array<T, N>::value_type;
  using iterator       = typename slotted_array<T, N>::iterator;
  using const_iterator = typename slotted_array<T, N>::const_iterator;

  using IdToIntConversion::IdToIntConversion;

  bool contains(key_type id) const noexcept { return sl_ar.contains(to_index(id)); }

  constexpr T&       operator[](key_type id) { return sl_ar[to_index(id)]; }
  constexpr const T& operator[](key_type id) const { return sl_ar[to_index(id)]; }

  constexpr bool   empty() const noexcept { return sl_ar.empty(); }
  constexpr size_t size() const { return sl_ar.size(); }

  iterator       begin() { return sl_ar.begin(); }
  iterator       end() { return sl_ar.end(); }
  const_iterator begin() const { return sl_ar.begin(); }
  const_iterator end() const { return sl_ar.end(); }

  /// Insert given object with a given Id.
  /// \param id ID the constructed element in the table.
  /// \param u object to insert
  template <typename U>
  void insert(key_type id, U&& u)
  {
    sl_ar.insert(to_index(id), std::forward<U>(u));
  }

  /// Overwrite array element with given index with a newly constructed object
  /// \param id ID of the constructed element in the table
  /// \param args Arguments to pass to element ctor
  template <typename... Args>
  void emplace(key_type id, Args&&... args)
  {
    sl_ar.emplace(to_index(id), std::forward<Args>(args)...);
  }

  /// Erase object pointed by the given index
  /// \param id ID of the erased element in the table
  bool erase(key_type id) noexcept { return sl_ar.erase(to_index(id)); }

  /// Erase object pointed by the given iterator. Iterator must point to valid element
  /// \param it container iterator
  void erase(iterator it) noexcept { sl_ar.erase(it); }

  /// Clear all elements of the container
  void clear() noexcept { sl_ar.clear(); }

  /// Find first position that is empty
  key_type find_first_empty(key_type start_guess = 0) const
  {
    return static_cast<key_type>(find_first_empty(to_index(start_guess)));
  }

private:
  constexpr size_t to_index(key_type k) const { return IdToIntConversion::operator()(k); }

  slotted_array<T, N> sl_ar;
};

} // namespace srsgnb
