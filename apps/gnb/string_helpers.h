/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <string>

namespace srsgnb {

/**
 * It splits a given string into multiple elements given a delimiter. The elements are casted to the specified type.
 * @tparam Insertable It is the list data-type. It needs to implement insert(iterator, element)
 * @param input It is the input string
 * @param delimiter Character used for indicating the end of the strings
 * @param list contains the parsed values
 */
template <class Insertable>
static inline void string_parse_list(const std::string& input, char delimiter, Insertable& list)
{
  std::stringstream ss(input);

  // Removes all possible elements of the list
  list.clear();

  while (ss.good()) {
    std::string substr;
    std::getline(ss, substr, delimiter);

    if (not substr.empty()) {
      list.insert(list.end(), substr);
    }
  }
}

} // namespace srsgnb
