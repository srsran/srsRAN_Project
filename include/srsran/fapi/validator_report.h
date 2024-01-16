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

#pragma once

#include "srsran/adt/optional.h"
#include "srsran/adt/static_vector.h"
#include "srsran/fapi/messages.h"

namespace srsran {
namespace fapi {

/// A validator report holds the context of the fields and values that caused a FAPI message validation to fail.
struct validator_report {
  /// Maximum number of supported reports.
  static constexpr unsigned MAX_NUM_REPORTS = 4U;

  /// Error report information structure. Contains the information that helps to identify the source of the failure.
  struct error_report {
    int32_t                               value;
    const char*                           property_name;
    message_type_id                       message_type;
    optional<std::pair<int32_t, int32_t>> expected_value_range;
    optional<unsigned>                    pdu_type;

    error_report(int32_t                     value_,
                 std::pair<int32_t, int32_t> range,
                 const char*                 name,
                 message_type_id             message_type_,
                 unsigned                    pdu_type_) :
      value(value_), property_name(name), message_type(message_type_), expected_value_range(range), pdu_type(pdu_type_)
    {
    }

    error_report(int32_t value_, const char* name, message_type_id message_type_, unsigned pdu_type_) :
      value(value_), property_name(name), message_type(message_type_), pdu_type(pdu_type_)
    {
    }

    error_report(int32_t value_, std::pair<int32_t, int32_t> range, const char* name, message_type_id message_type_) :
      value(value_), property_name(name), message_type(message_type_), expected_value_range(range)
    {
    }

    error_report(int32_t value_, const char* name, message_type_id message_type_) :
      value(value_), property_name(name), message_type(message_type_)
    {
    }
  };

  validator_report(uint16_t sfn_, uint16_t slot_) : sfn(sfn_), slot(slot_) {}

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
} // namespace srsran
