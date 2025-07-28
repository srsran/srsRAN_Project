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
};

} // namespace fapi
} // namespace srsran
