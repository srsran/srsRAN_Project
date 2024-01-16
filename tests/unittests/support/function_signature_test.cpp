/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/support/async/detail/function_signature.h"

using namespace srsran;
using namespace detail;

// Free Function tests
void foo() {}
int  foo2(char c, float v)
{
  return 0;
}
static_assert(type_list_size(function_args_t<decltype(foo)>{}) == 0, "Invalid number of function arguments");
static_assert(std::is_same<function_return_t<decltype(foo)>, void>::value, "Invalid number of function arguments");
static_assert(std::is_same<function_args_t<decltype(foo)>, type_list<>>::value, "Invalid argument derivation");
static_assert(type_list_size(function_args_t<decltype(foo2)>{}) == 2, "Invalid number of function arguments");
static_assert(std::is_same<function_args_t<decltype(foo2)>, type_list<char, float>>::value,
              "Invalid argument derivation");
static_assert(std::is_same<function_return_t<decltype(foo2)>, int>::value, "Invalid number of function arguments");

// Class Method tests
struct C {
  void bar() {}
  int  bar2(char& c, const float& v) { return 0; }
};

static_assert(type_list_size(function_args_t<decltype(&C::bar)>{}) == 0, "Invalid number of function arguments");
static_assert(std::is_same<function_return_t<decltype(&C::bar)>, void>::value, "Invalid number of function arguments");
static_assert(std::is_same<function_args_t<decltype(&C::bar)>, type_list<>>::value, "Invalid argument derivation");
static_assert(type_list_size(function_args_t<decltype(&C::bar2)>{}) == 2, "Invalid number of function arguments");
static_assert(std::is_same<function_return_t<decltype(&C::bar2)>, int>::value, "Invalid number of function arguments");
static_assert(std::is_same<function_args_t<decltype(&C::bar2)>, type_list<char&, const float&>>::value,
              "Invalid argument derivation");

int main()
{
  return 0;
}