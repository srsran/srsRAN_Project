/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/fapi/message_validators.h"

namespace srsran {
namespace fapi {

/// \brief Helper validation function.
///
/// If the value is within the [min, max] range, returns true, otherwise false. If the validation fails, the given
/// report is filled.
template <typename Integer>
inline bool validate_field(Integer           min,
                           Integer           max,
                           Integer           value,
                           const char*       property,
                           message_type_id   msg_type,
                           unsigned          pdu_type,
                           validator_report& report)
{
  static_assert(std::is_integral<Integer>::value, "Integral required");

  if (min <= value && value <= max) {
    return true;
  }

  report.append(value, std::make_pair(min, max), property, msg_type, pdu_type);
  return false;
}

/// \brief Helper validation function.
///
/// If the value is within the [min, max] range, returns true, otherwise false. If the validation fails, the given
/// report is filled.
template <typename Integer>
inline bool validate_field(Integer           min,
                           Integer           max,
                           Integer           value,
                           const char*       property,
                           message_type_id   msg_type,
                           validator_report& report)
{
  static_assert(std::is_integral<Integer>::value, "Integral required");

  if (min <= value && value <= max) {
    return true;
  }

  report.append(value, std::make_pair(min, max), property, msg_type);
  return false;
}

/// \brief Helper validation function.
///
/// If expected_value matches value, returns true, otherwise false. If the validation fails, the given report is filled.
template <typename Integer>
inline bool validate_field_eq(Integer           expected_value,
                              Integer           value,
                              const char*       property,
                              message_type_id   msg_type,
                              unsigned          pdu_type,
                              validator_report& report)
{
  static_assert(std::is_integral<Integer>::value, "Integral required");

  if (expected_value == value) {
    return true;
  }

  report.append(value, property, msg_type, pdu_type);
  return false;
}

} // namespace fapi
} // namespace srsran