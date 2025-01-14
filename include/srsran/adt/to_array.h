
#pragma once

#include <array>

namespace srsran {

namespace detail {

template <class T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N> to_array_impl(T (&a)[N], std::index_sequence<I...>)
{
  return {{a[I]...}};
}

template <class T, std::size_t N, std::size_t... I>
constexpr std::array<std::remove_cv_t<T>, N> to_array_impl(T (&&a)[N], std::index_sequence<I...>)
{
  return {{std::move(a[I])...}};
}

template <class T, std::size_t N, class Factory, std::size_t... Is>
constexpr std::array<T, N> make_array_impl(const Factory& factory, std::index_sequence<Is...> /* unused */)
{
  return {{factory(Is)...}};
}

} // namespace detail

/// Helper method to generate an std::array from a C array without needing to derive the size N.
template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&a)[N])
{
  return detail::to_array_impl(a, std::make_index_sequence<N>{});
}
template <class T, std::size_t N>
constexpr std::array<std::remove_cv_t<T>, N> to_array(T (&&a)[N])
{
  return detail::to_array_impl(std::move(a), std::make_index_sequence<N>{});
}

/// Helper method to generate an std::array using an array-element factory.
template <class T, std::size_t N, class Factory>
constexpr std::array<T, N> make_array(const Factory& factory)
{
  return detail::make_array_impl<T, N>(factory, std::make_index_sequence<N>());
}

} // namespace srsran
