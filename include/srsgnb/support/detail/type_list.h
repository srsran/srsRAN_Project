
#ifndef SRSGNB_TYPE_LIST_H
#define SRSGNB_TYPE_LIST_H

#include <type_traits>

namespace srsgnb {

/// List of types
template <typename... Args>
struct type_list {};

/// Invalid index to type_list
constexpr std::size_t invalid_type_index = -1;

/// Metafunction to extract type from variadic template arguments based on provided Index
template <std::size_t Index, class... Types>
class get_type_from_index
{
  static_assert(Index < sizeof...(Types), "index out of bounds");

  template <std::size_t I, std::size_t N, class... Rest>
  struct extract_impl;

  template <std::size_t I, std::size_t N, class T, class... Rest>
  struct extract_impl<I, N, T, Rest...> {
    using type = typename extract_impl<I + 1, N, Rest...>::type;
  };

  template <std::size_t N, class T, class... Rest>
  struct extract_impl<N, N, T, Rest...> {
    using type = T;
  };

public:
  using type = typename extract_impl<0, Index, Types...>::type;
};

/// Specialization when argument is a type_list
template <std::size_t Index, class... Types>
class get_type_from_index<Index, type_list<Types...> >
{
  using type = get_type_from_index<Index, Types...>;
};

template<std::size_t Index, typename... Types>
using get_type_from_index_t = typename get_type_from_index<Index, Types...>::type;

} // namespace srsgnb

#endif // SRSGNB_TYPE_LIST_H
