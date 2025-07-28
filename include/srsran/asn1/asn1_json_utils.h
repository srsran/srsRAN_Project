/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once

#include "asn1_utils.h"
#include "nlohmann/json.hpp"

namespace asn1 {

inline void to_json(nlohmann::json& out_j, const real_s& obj)
{
  out_j = obj.value;
}

inline void from_json(const nlohmann::json& in_j, real_s& obj)
{
  in_j.get_to(obj.value);
}

inline void to_json(nlohmann::ordered_json& out_j, const real_s& obj)
{
  out_j = obj.value;
}

inline void from_json(const nlohmann::ordered_json& in_j, real_s& obj)
{
  in_j.get_to(obj.value);
}

} // namespace asn1
