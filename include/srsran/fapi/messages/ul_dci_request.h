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

#include "srsran/fapi/messages/base_message.h"
#include "srsran/fapi/messages/dl_pdcch_pdu.h"

namespace srsran {
namespace fapi {

enum class ul_dci_pdu_type : uint16_t { PDCCH };

/// Uplink DCI PDU information.
struct ul_dci_pdu {
  ul_dci_pdu_type pdu_type;
  units::bytes    pdu_size;
  dl_pdcch_pdu    pdu;
};

/// Uplink DCI request message.
struct ul_dci_request_message : public base_message {
  /// [Implementation-defined] DCI index in the array number PDUs of each type.
  static constexpr unsigned DCI_INDEX = 1;
  /// [Implementation-defined] Maximum number of supported UL PDU types in this release.
  static constexpr unsigned MAX_NUM_DL_TYPES = 2;
  /// [Implementation-defined] Maximum number of supported UCI PDUs in this message.
  static constexpr unsigned MAX_NUM_UL_DCI_PDUS = 128;

  uint16_t                                       sfn;
  uint16_t                                       slot;
  std::array<uint16_t, MAX_NUM_DL_TYPES>         num_pdus_of_each_type;
  static_vector<ul_dci_pdu, MAX_NUM_UL_DCI_PDUS> pdus;
  // Vendor specific parameters.
  bool is_last_message_in_slot;
};

} // namespace fapi
} // namespace srsran
