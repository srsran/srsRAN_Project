
#ifndef SRSGNB_TYPE_LIST_H
#define SRSGNB_TYPE_LIST_H

#include <type_traits>

namespace srsgnb {

namespace detail {

template <std::size_t I, std::size_t N, typename... Types>
struct get_type_from_index_helper;

template <std::size_t I, std::size_t N, typename T, typename... Types>
struct get_type_from_index_helper<I, N, T, Types...> {
  using type = typename get_type_from_index_helper<I + 1, N, Types...>::type;
};

template <std::size_t I, typename T, typename... Types>
struct get_type_from_index_helper<I, I, T, Types...> {
  using type = T;
};

/// List of types
template <typename... Args>
struct type_list {};

/// Get size of type_list
template <typename... Args>
constexpr std::size_t type_list_size(type_list<Args...> t)
{
  return sizeof...(Args);
}

/// Metafunction to extract type from variadic template arguments based on provided Index
template <std::size_t Index, class... Types>
class get_type_from_index
{
  static_assert(Index < sizeof...(Types), "index out of bounds");

public:
  using type = typename get_type_from_index_helper<0, Index, Types...>::type;
};

/// Specialization when argument is a type_list
template <std::size_t Index, class... Types>
class get_type_from_index<Index, type_list<Types...> >
{
public:
  using type = typename get_type_from_index<Index, Types...>::type;
};

template <std::size_t Index, typename... Types>
using get_type_from_index_t = typename get_type_from_index<Index, Types...>::type;

} // namespace detail

} // namespace srsgnb

#endif // SRSGNB_TYPE_LIST_H
