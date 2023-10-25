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

#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/span.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/rlc/rlc_config.h"
#include <cstdint>

namespace srsran {

struct pcap_rlc_pdu_context;

/// \brief Interface class for writing a RLC PCAP to a file.
class pcap_rlc
{
public:
  virtual ~pcap_rlc() = default;

  virtual void open(const std::string& filename_)                                     = 0;
  virtual void close()                                                                = 0;
  virtual bool is_write_enabled()                                                     = 0;
  virtual void push_pdu(const pcap_rlc_pdu_context& context, const_span<uint8_t> pdu) = 0;
  virtual void push_pdu(const pcap_rlc_pdu_context& context, const byte_buffer& pdu)  = 0;
};

/// \brief Context information for every RLC NR PDU that will be logged.
///
/// This struct follows the definition in Wireshark's "packet-rlc-nr.h".
struct pcap_rlc_pdu_context {
  uint8_t  rlc_mode;
  uint8_t  direction;
  uint8_t  sequence_number_length;
  uint8_t  bearer_type;
  uint8_t  bearer_id;
  uint16_t ueid;

  ~pcap_rlc_pdu_context() = default;

  /// \brief Create a PCAP PDU context for DL RLC AM PDUs
  /// \param ue_index UE index
  /// \param rb_id Radio bearer type and ID
  /// \param cfg RLC TX AM config
  pcap_rlc_pdu_context(du_ue_index_t ue_index, rb_id_t rb_id, const rlc_tx_am_config& cfg);

  /// \brief Create a PCAP PDU context for UL RLC AM PDUs
  /// \param ue_index UE index
  /// \param rb_id Radio bearer type and ID
  /// \param cfg RLC RX AM config
  pcap_rlc_pdu_context(du_ue_index_t ue_index, rb_id_t rb_id, const rlc_rx_am_config& cfg);

  /// \brief Create a PCAP PDU context for DL RLC UM PDUs
  /// \param ue_index UE index
  /// \param rb_id Radio bearer type and ID
  /// \param cfg RLC TX UM config
  pcap_rlc_pdu_context(du_ue_index_t ue_index, rb_id_t rb_id, const rlc_tx_um_config& cfg);

  /// \brief Create a PCAP PDU context for UL RLC UM PDUs
  /// \param ue_index UE index
  /// \param rb_id Radio bearer type and ID
  /// \param cfg RLC RX UM config
  pcap_rlc_pdu_context(du_ue_index_t ue_index, rb_id_t rb_id, const rlc_rx_um_config& cfg);

  /// \brief Create a PCAP PDU context for UL/DL RLC TM PDUs
  /// \param ue_index UE index
  /// \param rb_id Radio bearer type and ID
  /// \param is_uplink RLC TM direction (DL: false, UL: true)
  pcap_rlc_pdu_context(du_ue_index_t ue_index, rb_id_t rb_id, bool is_uplink);

private:
  /// \brief Sets the SN length according to RLC AM-specific SN length type
  /// \param sn_field_length RLC AM-specific SN length type
  void set_sequence_number_length(rlc_am_sn_size sn_field_length);

  /// \brief Sets the SN length according to RLC UM-specific SN length type
  /// \param sn_field_length RLC UM-specific SN length type
  void set_sequence_number_length(rlc_um_sn_size sn_field_length);

  /// \brief Sets the radio bearer ID and type
  /// \param rb_id Radio bearer ID and type
  void set_bearer_info(rb_id_t rb_id);
};

} // namespace srsran
