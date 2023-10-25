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

// RLC mode
constexpr uint8_t RLC_TM_MODE = 1;
constexpr uint8_t RLC_UM_MODE = 2;
constexpr uint8_t RLC_AM_MODE = 4;

// Direction
constexpr uint8_t RLC_DIRECTION_UPLINK   = 0;
constexpr uint8_t RLC_DIRECTION_DOWNLINK = 1;

// Bearer type
constexpr uint8_t RLC_BEARER_TYPE_CCCH        = 1;
constexpr uint8_t RLC_BEARER_TYPE_BCCH_BCH    = 2;
constexpr uint8_t RLC_BEARER_TYPE_PCCH        = 3;
constexpr uint8_t RLC_BEARER_TYPE_SRB         = 4;
constexpr uint8_t RLC_BEARER_TYPE_DRB         = 5;
constexpr uint8_t RLC_BEARER_TYPE_BCCH_DL_SCH = 6;

// RLC sequence number length
constexpr uint8_t RLC_TM_SN_LENGTH_0_BITS  = 0;
constexpr uint8_t RLC_UM_SN_LENGTH_6_BITS  = 6;
constexpr uint8_t RLC_UM_SN_LENGTH_12_BITS = 12;
constexpr uint8_t RLC_AM_SN_LENGTH_12_BITS = 12;
constexpr uint8_t RLC_AM_SN_LENGTH_18_BITS = 18;

// Context information for every RLC NR PDU that will be logged
struct rlc_nr_context_info {
  uint8_t  rlc_mode;
  uint8_t  direction;
  uint8_t  sequence_number_length;
  uint8_t  bearer_type;
  uint8_t  bearer_id;
  uint16_t ueid;

  ~rlc_nr_context_info() = default;

  /// \brief Create a PCAP PDU context for DL RLC AM PDUs
  /// \param ue_index UE index
  /// \param rb_id Radio bearer type and ID
  /// \param cfg RLC TX AM config
  rlc_nr_context_info(du_ue_index_t ue_index, rb_id_t rb_id, const rlc_tx_am_config& cfg)
  {
    rlc_mode  = RLC_AM_MODE;
    direction = RLC_DIRECTION_DOWNLINK;
    set_sequence_number_length(cfg.sn_field_length);
    set_bearer_info(rb_id);
    ueid = ue_index;
  }

  /// \brief Create a PCAP PDU context for UL RLC AM PDUs
  /// \param ue_index UE index
  /// \param rb_id Radio bearer type and ID
  /// \param cfg RLC RX AM config
  rlc_nr_context_info(du_ue_index_t ue_index, rb_id_t rb_id, const rlc_rx_am_config& cfg)
  {
    rlc_mode  = RLC_AM_MODE;
    direction = RLC_DIRECTION_UPLINK;
    set_sequence_number_length(cfg.sn_field_length);
    set_bearer_info(rb_id);
    ueid = ue_index;
  }

  /// \brief Create a PCAP PDU context for DL RLC UM PDUs
  /// \param ue_index UE index
  /// \param rb_id Radio bearer type and ID
  /// \param cfg RLC TX UM config
  rlc_nr_context_info(du_ue_index_t ue_index, rb_id_t rb_id, const rlc_tx_um_config& cfg)
  {
    rlc_mode  = RLC_UM_MODE;
    direction = RLC_DIRECTION_DOWNLINK;
    set_sequence_number_length(cfg.sn_field_length);
    set_bearer_info(rb_id);
    ueid = ue_index;
  }

  /// \brief Create a PCAP PDU context for UL RLC UM PDUs
  /// \param ue_index UE index
  /// \param rb_id Radio bearer type and ID
  /// \param cfg RLC RX UM config
  rlc_nr_context_info(du_ue_index_t ue_index, rb_id_t rb_id, const rlc_rx_um_config& cfg)
  {
    rlc_mode  = RLC_UM_MODE;
    direction = RLC_DIRECTION_UPLINK;
    set_sequence_number_length(cfg.sn_field_length);
    set_bearer_info(rb_id);
    ueid = ue_index;
  }

  /// \brief Create a PCAP PDU context for UL/DL RLC TM PDUs
  /// \param ue_index UE index
  /// \param rb_id Radio bearer type and ID
  /// \param is_uplink RLC TM direction (DL: false, UL: true)
  rlc_nr_context_info(du_ue_index_t ue_index, rb_id_t rb_id, bool is_uplink)
  {
    rlc_mode               = RLC_TM_MODE;
    direction              = is_uplink ? RLC_DIRECTION_UPLINK : RLC_DIRECTION_DOWNLINK;
    sequence_number_length = RLC_TM_SN_LENGTH_0_BITS;
    set_bearer_info(rb_id);
    ueid = ue_index;
  }

private:
  /// \brief Sets the SN length according to RLC AM-specific SN length type
  /// \param sn_field_length RLC AM-specific SN length type
  void set_sequence_number_length(rlc_am_sn_size sn_field_length)
  {
    switch (sn_field_length) {
      case rlc_am_sn_size::size12bits:
        sequence_number_length = RLC_AM_SN_LENGTH_12_BITS;
      case rlc_am_sn_size::size18bits:
        sequence_number_length = RLC_AM_SN_LENGTH_18_BITS;
    }
  }

  /// \brief Sets the SN length according to RLC UM-specific SN length type
  /// \param sn_field_length RLC UM-specific SN length type
  void set_sequence_number_length(rlc_um_sn_size sn_field_length)
  {
    switch (sn_field_length) {
      case rlc_um_sn_size::size6bits:
        sequence_number_length = RLC_UM_SN_LENGTH_6_BITS;
      case rlc_um_sn_size::size12bits:
        sequence_number_length = RLC_UM_SN_LENGTH_12_BITS;
    }
  }

  /// \brief Sets the radio bearer ID and type
  /// \param rb_id Radio bearer ID and type
  void set_bearer_info(rb_id_t rb_id)
  {
    if (rb_id.is_drb()) {
      bearer_type = RLC_BEARER_TYPE_DRB;
      bearer_id   = drb_id_to_uint(rb_id.get_drb_id());
    } else if (rb_id.is_srb()) {
      bearer_type = RLC_BEARER_TYPE_SRB;
      bearer_id   = srb_id_to_uint(rb_id.get_srb_id());
    }
  }
};

/// @brief Interface class for writing a RLC PCAP to a file.
class rlc_pcap
{
public:
  virtual ~rlc_pcap() = default;

  virtual void open(const std::string& filename_)                                    = 0;
  virtual void close()                                                               = 0;
  virtual bool is_write_enabled()                                                    = 0;
  virtual void push_pdu(const rlc_nr_context_info& context, const_span<uint8_t> pdu) = 0;
  virtual void push_pdu(const rlc_nr_context_info& context, const byte_buffer& pdu)  = 0;
};

} // namespace srsran
