#ifndef SRSGNB_LIB_FAPI_VALIDATORS_TOOLS_H
#define SRSGNB_LIB_FAPI_VALIDATORS_TOOLS_H

#include "srsgnb/fapi/validators.h"

namespace srsgnb {
namespace fapi {

/// Helper validation function. If verifies that values is within [min,max] range and returns true. If value is outside
/// the range, returns false. If the validation fails, given report is filled.
bool inline validate_field(int32_t min, int32_t max, int32_t value, const char* property, validator_report& report)
{
  if (min <= value && value <= max) {
    return true;
  }

  report.emplace_back(value, std::make_pair(min, max), property);
  return false;
}

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_VALIDATORS_TOOLS_H