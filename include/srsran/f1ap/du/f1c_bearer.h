/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "srsran/adt/byte_buffer_slice_chain.h"

namespace srsran {
namespace srs_du {

/// \brief This interface represents the data entry point of the transmitting side of a F1-C bearer of the DU.
/// The lower layer will use this class to pass F1AP SDUs (e.g. PDCP PDUs/RLC SDUs) into the F1-C bearer towards CU-CP.
class f1c_tx_sdu_handler
{
public:
  virtual ~f1c_tx_sdu_handler() = default;

  /// Handle SDUs that are pushed to the F1AP from lower layers (e.g. RLC).
  virtual void handle_sdu(byte_buffer_slice_chain sdu) = 0;
};

/// \brief This interface represents the F1AP entry point of the receiving side of a F1-C bearer of the DU.
/// The F1-C gateway will use it to pass F1AP PDUs (from the CU-CP) into the F1-C bearer of the DU.
class f1c_rx_pdu_handler
{
public:
  virtual ~f1c_rx_pdu_handler() = default;

  /// Handle SDUs that are pushed to the F1AP from upper layers.
  virtual void handle_pdu(byte_buffer pdu) = 0;
};

class f1c_bearer : public f1c_tx_sdu_handler, public f1c_rx_pdu_handler
{};

} // namespace srs_du
} // namespace srsran