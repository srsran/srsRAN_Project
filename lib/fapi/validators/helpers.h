/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_FAPI_VALIDATORS_HELPERS_H
#define SRSGNB_LIB_FAPI_VALIDATORS_HELPERS_H

#include "srsgnb/fapi/message_validators.h"

namespace srsgnb {
namespace fapi {

/// Helper validation function. If the values are within the [min,max] range, returns true, otherwise returns false. If
/// the validation fails, given report is filled.
inline bool validate_field(int32_t           min,
                           int32_t           max,
                           int32_t           value,
                           const char*       property,
                           message_type_id   msg_type,
                           unsigned          pdu_type,
                           validator_report& report)
{
  if (min <= value && value <= max) {
    return true;
  }

  report.append(value, std::make_pair(min, max), property, msg_type, pdu_type);
  return false;
}

/// Helper validation function. If the values are within [min,max] range, returns true, otherwise returns false. If the
/// validation fails, given report is filled.
inline bool validate_field(int32_t           min,
                           int32_t           max,
                           int32_t           value,
                           const char*       property,
                           message_type_id   msg_type,
                           validator_report& report)
{
  if (min <= value && value <= max) {
    return true;
  }

  report.append(value, std::make_pair(min, max), property, msg_type);
  return false;
}

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_VALIDATORS_HELPERS_H
