/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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

/// \brief This interface represents the notification entry point of the transmitting side of a F1-C bearer of the DU
/// through which the lower layer (e.g. RLC) notifies the F1-C bearer of transmit/delivery of PDCP PDUs.
class f1c_tx_delivery_handler
{
public:
  virtual ~f1c_tx_delivery_handler() = default;

  /// \brief Informs the F1-C bearer about the highest PDCP PDU sequence number that was transmitted by
  /// the lower layers (i.e. by the RLC).
  ///
  /// \param highest_sn Highest transmitted PDCP PDU sequence number.
  virtual void handle_transmit_notification(uint32_t highest_pdcp_sn) = 0;

  /// \brief Informs the F1-C bearer about the highest PDCP PDU sequence number that was successfully
  /// delivered in sequence towards the UE.
  ///
  /// \param highest_sn Highest in a sequence delivered PDCP PDU sequence number.
  virtual void handle_delivery_notification(uint32_t highest_pdcp_sn) = 0;
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

class f1c_bearer : public f1c_tx_sdu_handler, public f1c_tx_delivery_handler, public f1c_rx_pdu_handler
{};

} // namespace srs_du
} // namespace srsran
