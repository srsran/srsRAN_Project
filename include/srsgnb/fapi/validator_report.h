/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_FAPI_VALIDATOR_REPORT_H
#define SRSGNB_FAPI_VALIDATOR_REPORT_H

#include "srsgnb/adt/optional.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/fapi/messages.h"

namespace srsgnb {
namespace fapi {

/// A validator report holds the context of the fields and values that caused a FAPI message validation to fail.
struct validator_report {
  /// Maximum number of supported reports.
  static constexpr unsigned MAX_NUM_REPORTS = 4U;

  /// Error report information structure. Contains the information that helps to identify the source of the failure.
  struct error_report {
    int32_t                     value;
    std::pair<int32_t, int32_t> expected_value_range;
    const char*                 property_name;
    message_type_id             message_type;
    optional<unsigned>          pdu_type;

    error_report(int32_t                     value,
                 std::pair<int32_t, int32_t> range,
                 const char*                 name,
                 message_type_id             message_type,
                 unsigned                    pdu_type) :
      value(value),
      expected_value_range(std::move(range)),
      property_name(name),
      message_type(message_type),
      pdu_type(pdu_type)
    {
    }

    error_report(int32_t value, const char* name, message_type_id message_type, unsigned pdu_type) :
      value(value), property_name(name), message_type(message_type), pdu_type(pdu_type)
    {
    }

    error_report(int32_t value, std::pair<int32_t, int32_t> range, const char* name, message_type_id message_type) :
      value(value), expected_value_range(std::move(range)), property_name(name), message_type(message_type)
    {
    }

    error_report(int32_t value, const char* name, message_type_id message_type) :
      value(value), property_name(name), message_type(message_type)
    {
    }
  };

  validator_report(uint16_t sfn, uint16_t slot) : sfn(sfn), slot(slot) {}

  /// Appends an error report.
  void append(int32_t                     value,
              std::pair<int32_t, int32_t> range,
              const char*                 name,
              message_type_id             message_type,
              unsigned                    pdu_type)
  {
    ++nof_errors;
    if (reports.size() < MAX_NUM_REPORTS) {
      reports.emplace_back(value, range, name, message_type, pdu_type);
    }
  }

  /// Appends an error report.
  void append(int32_t value, const char* name, message_type_id message_type, unsigned pdu_type)
  {
    ++nof_errors;
    if (reports.size() < MAX_NUM_REPORTS) {
      reports.emplace_back(value, name, message_type, pdu_type);
    }
  }

  /// Appends an error report.
  void append(int32_t value, std::pair<int32_t, int32_t> range, const char* name, message_type_id message_type)
  {
    ++nof_errors;
    if (reports.size() < MAX_NUM_REPORTS) {
      reports.emplace_back(value, range, name, message_type);
    }
  }

  /// Appends an error report.
  void append(int32_t value, const char* name, message_type_id message_type)
  {
    ++nof_errors;
    if (reports.size() < MAX_NUM_REPORTS) {
      reports.emplace_back(value, name, message_type);
    }
  }

  uint16_t                                     sfn;
  uint16_t                                     slot;
  static_vector<error_report, MAX_NUM_REPORTS> reports;
  unsigned                                     nof_errors = 0;
};

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_FAPI_VALIDATOR_REPORT_H
