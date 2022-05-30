/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_UNITTESTS_FAPI_VALIDATORS_HELPERS_H
#define SRSGNB_UNITTESTS_FAPI_VALIDATORS_HELPERS_H

#include "srsgnb/fapi/messages.h"

namespace unittest {

/// Class that defines the vector test for the validators.
template <typename T>
class test_group
{
public:
  // Auxiliary struct.
  struct data {
    unsigned value;
    bool     result;
  };

  // Aliases for the iterators.
  using iterator       = data*;
  using const_iterator = const data*;

  // Iterators.
  iterator       begin() { return &*values.begin(); }
  iterator       end() { return &*values.end(); }
  const_iterator begin() const { return &*values.cbegin(); }
  const_iterator end() const { return &*values.cend(); }

  test_group(std::function<void(T& pdu, int value)> f, const char* prop, std::vector<data> values) :
    f(std::move(f)), prop(prop), values(std::move(values))
  {
  }

  /// Configure the Next case of the test case. Returns the test result.
  void update_msg(T& pdu, int value) const { f(pdu, value); }

  /// Returns the property name.
  const char* property() const { return prop; }

private:
  std::function<void(T& pdu, int value)> f;
  const char*                            prop = "";
  std::vector<data>                      values;
};

/// Builds and returns a valid DL CSI-RS pdu. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.3.
srsgnb::fapi::dl_csi_rs_pdu build_valid_dl_csi_pdu();

/// Builds and returns a valid DL PDCCH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.1.
srsgnb::fapi::dl_pdcch_pdu build_valid_dl_pdcch_pdu();

/// Builds and returns a valid DL PDSCH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.2.
srsgnb::fapi::dl_pdsch_pdu build_valid_dl_pdsch_pdu();

/// Builds and returns a valid DL SSB pdu. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.4.
srsgnb::fapi::dl_ssb_pdu build_valid_dl_ssb_pdu();

/// Builds and returns a valid DL TTI request message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.2.
srsgnb::fapi::dl_tti_request_message build_valid_dl_tti_request();

/// Builds and returns a valid ERROR.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.3.6.1.
srsgnb::fapi::error_indication_message build_valid_error_indication();

/// Builds and returns a valid OUT_OF_SYNC ERROR.indication message. Every parameter is within the range defined in
/// SCF-222 v4.0 Section 3.3.6.1.
srsgnb::fapi::error_indication_message build_valid_out_of_sync_error_indication();

/// Builds and returns a valid Rx_Data.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.7.
srsgnb::fapi::rx_data_indication_message build_valid_rx_data_indication();

/// Builds and returns a valid SLOT.indication message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.1.
srsgnb::fapi::slot_indication_message build_valid_slot_indication();

/// Builds and returns a valid UL_DCI.request message. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.4.
srsgnb::fapi::ul_dci_request_message build_valid_ul_dci_request();

/// Builds and returns a valid UL PRACH PDU. Every parameter is within the range defined in SCF-222 v4.0
/// Section 3.4.3.1.
srsgnb::fapi::ul_prach_pdu build_valid_ul_prach_pdu();

} // namespace unittest

#endif // SRSGNB_UNITTESTS_FAPI_VALIDATORS_HELPERS_H
