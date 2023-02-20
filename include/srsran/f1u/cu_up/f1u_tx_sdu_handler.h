/*
 *
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/pdcp/pdcp_tx_pdu.h"

namespace srsran {
namespace srs_cu_up {

/// \brief This interface represents the data entry point of the transmitting side of a F1-U bearer of the CU-UP.
/// The upper layer (e.g. PDCP) will push NR-U SDUs (e.g. PDCP PDUs/RLC SDUs) into the F1-U bearer towards the DU.
/// The upper layer (e.g. PDCP) will also inform the lower layer of SDUs (e.g. PDCP PDUs) to be discarded.
class f1u_tx_sdu_handler
{
public:
  virtual ~f1u_tx_sdu_handler() = default;

  virtual void handle_sdu(pdcp_tx_pdu sdu)   = 0;
  virtual void discard_sdu(uint32_t pdcp_sn) = 0;
};

} // namespace srs_cu_up
} // namespace srsran
