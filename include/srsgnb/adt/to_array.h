
#ifndef SRSGNB_ADT_TO_ARRAY_H
#define SRSGNB_ADT_TO_ARRAY_H

#include <array>

namespace srsgnb {

namespace detail {

template <class T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N>
          to_array_impl(T (&a)[N], std::index_sequence<I...>)
{
  return { {a[I]...} };
}

template <class T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N> to_array_impl(T(&&a)[N], std::index_sequence<I...>)
{
  return {{std::move(a[I])...}};
}

} // namespace detail

/// Helper method to generate an std::array from a C array without needing to derive the size N.
template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N])
{
  return detail::to_array_impl(a, std::make_index_sequence<N>{});
}
template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T(&&a)[N])
{
  return detail::to_array_impl(std::move(a), std::make_index_sequence<N>{});
}

} // namespace srsgnb

#endif // SRSGNB_ADT_TO_ARRAY_H
