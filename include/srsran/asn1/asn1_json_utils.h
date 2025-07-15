/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
