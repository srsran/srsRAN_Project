/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/async/detail/function_signature.h"

using namespace srsgnb;
using namespace detail;

// Free Function tests
void foo() {}
int  foo2(char c, float v)
{
  return 0;
}
static_assert(type_list_size(function_args_t<decltype(foo)>{}) == 0, "Invalid number of function arguments");
static_assert(std::is_same<function_return_t<decltype(foo)>, void>::value, "Invalid number of function arguments");
static_assert(std::is_same<function_args_t<decltype(foo)>, type_list<> >::value, "Invalid argument derivation");
static_assert(type_list_size(function_args_t<decltype(foo2)>{}) == 2, "Invalid number of function arguments");
static_assert(std::is_same<function_args_t<decltype(foo2)>, type_list<char, float> >::value,
              "Invalid argument derivation");
static_assert(std::is_same<function_return_t<decltype(foo2)>, int>::value, "Invalid number of function arguments");

// Class Method tests
struct C {
  void bar() {}
  int  bar2(char& c, const float& v) { return 0; }
};

static_assert(type_list_size(function_args_t<decltype(&C::bar)>{}) == 0, "Invalid number of function arguments");
static_assert(std::is_same<function_return_t<decltype(&C::bar)>, void>::value, "Invalid number of function arguments");
static_assert(std::is_same<function_args_t<decltype(&C::bar)>, type_list<> >::value, "Invalid argument derivation");
static_assert(type_list_size(function_args_t<decltype(&C::bar2)>{}) == 2, "Invalid number of function arguments");
static_assert(std::is_same<function_return_t<decltype(&C::bar2)>, int>::value, "Invalid number of function arguments");
static_assert(std::is_same<function_args_t<decltype(&C::bar2)>, type_list<char&, const float&> >::value,
              "Invalid argument derivation");

int main()
{
  return 0;
}