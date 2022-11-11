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

#include "variant/variant.hpp"

namespace srsgnb {

template <typename... Types>
using variant = mpark::variant<Types...>;

} // namespace srsgnb
