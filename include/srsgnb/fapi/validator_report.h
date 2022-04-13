#ifndef SRSGNB_FAPI_VALIDATOR_REPORT_H
#define SRSGNB_FAPI_VALIDATOR_REPORT_H

#include "srsgnb/adt/optional.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/fapi/messages.h"

namespace srsgnb {
namespace fapi {

/// Defines a validator report that contains useful information to debug when the validation of a fapi message fails.
struct validator_report {
  /// Maximum number of reports supported.
  static constexpr unsigned MAX_NUM_REPORTS = 4u;

  /// Error report information structure. Contains the information that helps to identify the source of the error.
  struct error_report {
    int32_t                     value;
    std::pair<int32_t, int32_t> expected_value_range;
    const char*                 property_name;
    optional<dl_pdu_type>       pdu_type;
    slot_point                  slot;

    error_report(int32_t value, std::pair<int32_t, int32_t> range, const char* name, dl_pdu_type pdu_type) :
      value(value), expected_value_range(range), property_name(name), pdu_type(pdu_type)
    {}

    error_report(int32_t value, const char* name, dl_pdu_type pdu_type) :
      value(value), property_name(name), pdu_type(pdu_type)
    {}

    error_report(int32_t value, std::pair<int32_t, int32_t> range, const char* name) :
      value(value), expected_value_range(range), property_name(name)
    {}

    error_report(int32_t value, const char* name) : value(value), property_name(name) {}
  };

  /// Emplace back a report.
  void emplace_back(int32_t value, std::pair<int32_t, int32_t> range, const char* name, dl_pdu_type pdu_type)
  {
    reports.emplace_back(value, range, name, pdu_type);
  }

  /// Emplace back a report.
  void emplace_back(int32_t value, const char* name, dl_pdu_type pdu_type)
  {
    reports.emplace_back(value, name, pdu_type);
  }

  /// Emplace back a report.
  void emplace_back(int32_t value, std::pair<int32_t, int32_t> range, const char* name)
  {
    reports.emplace_back(value, range, name);
  }

  /// Emplace back a report.
  void emplace_back(int32_t value, const char* name) { reports.emplace_back(value, name); }

  static_vector<error_report, MAX_NUM_REPORTS> reports;
};

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_FAPI_VALIDATOR_REPORT_H