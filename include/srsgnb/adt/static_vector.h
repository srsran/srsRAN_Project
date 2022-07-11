
#ifndef SRSGNB_ADT_STATIC_VECTOR_H
#define SRSGNB_ADT_STATIC_VECTOR_H

#include "detail/type_storage.h"
#include "srsgnb/support/srsran_assert.h"
#include <iterator>
#include <memory>

namespace srsgnb {

/// Random access, variable-sized container with embedded storage.
/// Contrarily to std::vector<T>, its size is limited at compile-time, and it does not rely on allocators.
/// Contrarily to std::array, the elements are constructed and destructed dynamically when the container size() changes.
/// @tparam T type of the elements
/// @tparam MAX_N Maximum number of elements of the static_vector
template <typename T, std::size_t MAX_N>
class static_vector
{
public:
  using iterator        = T*;
  using const_iterator  = const T*;
  using size_type       = std::size_t;
  using value_type      = T;
  using difference_type = std::make_signed_t<size_type>;

  constexpr static_vector() noexcept = default;
  explicit static_vector(size_type count) { append(count); }
  static_vector(size_type count, const T& initial_value) { append(count, initial_value); }
  static_vector(const static_vector& other)
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copyable");
    append(other.begin(), other.end());
  }
  static_vector(static_vector&& other) noexcept(std::is_nothrow_move_constructible<value_type>::value)
  {
    static_assert(std::is_move_constructible<T>::value, "T must be move-constructible");
    std::uninitialized_copy(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()), end());
    size_ = other.size();
    other.clear();
  }
  static_vector(std::initializer_list<T> init) { append(init.begin(), init.end()); }
  static_vector(const_iterator it_begin, const_iterator it_end) { append(it_begin, it_end); }
  ~static_vector() { destroy(begin(), end()); }
  static_vector& operator=(const static_vector& other)
  {
    if (this == &other) {
      return *this;
    }
    assign(other.begin(), other.end());
    return *this;
  }
  static_vector& operator=(static_vector&& other) noexcept(std::is_nothrow_move_assignable<value_type>::value)
  {
    if (this == &other) {
      return *this;
    }
    size_t min_common_size = std::min(other.size(), size());
    if (min_common_size > 0) {
      // move already constructed elements
      auto it = std::move(other.begin(), other.begin() + min_common_size, begin());
      destroy(it, end());
    } else {
      clear();
    }
    // append the rest
    std::uninitialized_copy(
        std::make_move_iterator(other.begin() + min_common_size), std::make_move_iterator(other.end()), end());
    size_ = other.size();
    other.clear();
    return *this;
  }

  void assign(size_type nof_elems, const T& value)
  {
    clear();
    append(nof_elems, value);
  }
  void assign(const_iterator it_start, const_iterator it_end)
  {
    clear();
    append(it_start, it_end);
  }
  void assign(std::initializer_list<T> ilist) { assign(ilist.begin(), ilist.end()); }

  // Element access
  T& operator[](std::size_t i) noexcept
  {
    srsran_assert(i < size_, "Array index is out of bounds");
    return buffer[i].get();
  }
  const T& operator[](std::size_t i) const noexcept
  {
    srsran_assert(i < size_, "Array index is out of bounds");
    return buffer[i].get();
  }
  T& back()
  {
    srsran_assert(size_ > 0, "Trying to get back of empty array");
    return *(begin() + size_ - 1);
  }
  const T& back() const
  {
    srsran_assert(size_ > 0, "Trying to get back of empty array");
    return *(begin() + size_ - 1);
  }
  T&       front() { return (*this)[0]; }
  const T& front() const { return (*this)[0]; }
  T*       data() noexcept { return reinterpret_cast<T*>(buffer.data()); }
  const T* data() const noexcept { return reinterpret_cast<const T*>(buffer.data()); }

  // Iterators
  iterator       begin() noexcept { return data(); }
  iterator       end() noexcept { return begin() + size_; }
  const_iterator begin() const noexcept { return data(); }
  const_iterator end() const noexcept { return begin() + size_; }

  // Size/Capacity
  bool             empty() const noexcept { return size_ == 0; }
  std::size_t      size() const noexcept { return size_; }
  bool             full() const noexcept { return size_ == MAX_N; }
  static constexpr std::size_t capacity() noexcept { return MAX_N; }

  // modifiers
  void clear() noexcept
  {
    destroy(begin(), end());
    size_ = 0;
  }
  iterator erase(iterator pos)
  {
    srsran_assert(pos >= this->begin(), "Iterator to erase is out of bounds");
    srsran_assert(pos < this->end(), "Erasing at past-the-end iterator");
    iterator ret = pos;
    std::move(pos + 1, end(), pos);
    pop_back();
    return ret;
  }
  iterator erase(iterator it_start, iterator it_end)
  {
    srsran_assert(it_start >= begin(), "Range to erase is out of bounds");
    srsran_assert(it_start <= it_end, "Trying to erase invalid range");
    srsran_assert(it_end <= end(), "Trying to erase past the end");

    iterator ret = it_start;
    // Shift all elts down.
    iterator new_end = std::move(it_end, end(), it_start);
    destroy(new_end, end());
    size_ = new_end - begin();
    return ret;
  }
  void push_back(const T& value)
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copy-constructible");
    size_++;
    srsran_assert(size_ <= MAX_N, "static vector maximum size={} was exceeded", MAX_N);
    new (&back()) T(value);
  }
  void push_back(T&& value)
  {
    static_assert(std::is_move_constructible<T>::value, "T must be move-constructible");
    size_++;
    srsran_assert(size_ <= MAX_N, "static vector maximum size={} was exceeded", MAX_N);
    new (&back()) T(std::move(value));
  }
  template <typename... Args>
  void emplace_back(Args&&... args)
  {
    static_assert(std::is_constructible<T, Args&&...>::value, "Passed arguments to emplace_back are invalid");
    size_++;
    srsran_assert(size_ <= MAX_N, "static vector maximum size={} was exceeded", MAX_N);
    new (&back()) T(std::forward<Args>(args)...);
  }
  void pop_back()
  {
    srsran_assert(size_ > 0, "Trying to erase element from empty vector");
    back().~T();
    size_--;
  }
  void resize(size_type count)
  {
    static_assert(std::is_default_constructible<T>::value, "T must be default constructible");
    srsran_assert(count <= MAX_N, "Provided static vector size is too high");
    resize(count, T());
  }
  void resize(size_type count, const T& value)
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copy constructible");
    srsran_assert(count <= MAX_N, "Provided static vector size is too high");
    if (size_ > count) {
      destroy(begin() + count, end());
      size_ = count;
    } else if (size_ < count) {
      size_t to_append = count - size_;
      if (to_append > MAX_N) {
        // Note: Gcc 11 fails compilation without this assertion. Potentially a bug.
        __builtin_unreachable();
      }
      append(to_append, value);
    }
  }

  bool operator==(const static_vector& other) const noexcept
  {
    return std::equal(begin(), end(), other.begin(), other.end());
  }
  bool operator!=(const static_vector& other) const noexcept { return not(*this == other); }

  void swap(static_vector& other) noexcept
  {
    using std::swap;

    auto&        small_vec  = size() < other.size() ? *this : other;
    auto&        big_vec    = size() < other.size() ? other : *this;
    const size_t small_size = small_vec.size();
    const size_t big_size   = big_vec.size();

    for (size_t i = 0; i < small_size; ++i) {
      swap(small_vec[i], big_vec[i]);
    }
    for (size_t i = small_size; i < big_size; ++i) {
      small_vec.buffer[i].emplace(std::move(big_vec[i]));
      big_vec.buffer[i].destroy();
    }
    small_vec.size_ = big_size;
    big_vec.size_   = small_size;
  }

private:
  void destroy(iterator it_start, iterator it_end) noexcept
  {
    for (auto it = it_start; it != it_end; ++it) {
      it->~T();
    }
  }
  void append(const_iterator it_begin, const_iterator it_end)
  {
    size_type N = std::distance(it_begin, it_end);
    srsran_assert(N + size_ <= MAX_N, "static vector maximum size={} was exceeded", MAX_N);
    std::uninitialized_copy(it_begin, it_end, end());
    size_ += N;
  }
  void append(size_type N, const T& element)
  {
    static_assert(std::is_copy_constructible<T>::value, "T must be copy-constructible");
    srsran_assert(N + size_ <= MAX_N, "static vector maximum size={} was exceeded", MAX_N);
    std::uninitialized_fill_n(end(), N, element);
    size_ += N;
  }
  void append(size_type N)
  {
    static_assert(std::is_default_constructible<T>::value, "T must be default-constructible");
    srsran_assert(N + size_ <= MAX_N, "static vector maximum size={} was exceeded", MAX_N);
    for (size_type i = size_; i < size_ + N; ++i) {
      buffer[i].emplace();
    }
    size_ += N;
  }

  std::size_t                                size_ = 0;
  std::array<detail::type_storage<T>, MAX_N> buffer;
};

} // namespace srsgnb

#endif // SRSGNB_ADT_STATIC_VECTOR_H
