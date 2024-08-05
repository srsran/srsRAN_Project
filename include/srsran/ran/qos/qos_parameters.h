/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ran/qos/five_qi.h"
#include "srsran/ran/qos/qos_prio_level.h"
#include <cmath>
#include <cstdint>
#include <optional>

namespace srsran {

/// \brief Representation of the Packet Error Rate (PER) as defined in TS 38.473.
struct packet_error_rate_t {
  uint8_t scalar   = 0;
  uint8_t exponent = 0;

  double to_double() const { return scalar * std::pow(10, exponent); }
};

struct dyn_5qi_descriptor_t {
  qos_prio_level_t         qos_prio_level;
  uint16_t                 packet_delay_budget;
  packet_error_rate_t      per;
  std::optional<five_qi_t> five_qi;
  std::optional<bool>      is_delay_critical;
  /// This parameter should be present if the GBR QoS Flow information is set.
  std::optional<uint16_t> averaging_win;
  /// This parameter should be present if the GBR QoS Flow information is set.
  std::optional<uint16_t> max_data_burst_volume;
};

struct non_dyn_5qi_descriptor_t {
  five_qi_t                       five_qi;
  std::optional<qos_prio_level_t> qos_prio_level;
  std::optional<uint16_t>         averaging_win;
  std::optional<uint16_t>         max_data_burst_volume;
};

struct qos_characteristics_t {
  std::optional<dyn_5qi_descriptor_t>     dyn_5qi;
  std::optional<non_dyn_5qi_descriptor_t> non_dyn_5qi;

  five_qi_t get_five_qi() const
  {
    if (non_dyn_5qi.has_value()) {
      return non_dyn_5qi.value().five_qi;
    }
    if (dyn_5qi.has_value()) {
      if (dyn_5qi.value().five_qi.has_value()) {
        return dyn_5qi.value().five_qi.value();
      }
    } else {
      report_fatal_error("Invalid QoS characteristics. Either dynamic or non-dynamic 5QI must be set");
    }
    return five_qi_t::invalid;
  }
};

/// \brief QoS parameters for a GBR QoS flow or GBR bearer for downlink and uplink. See TS 38.473, clause 9.3.1.46.
struct gbr_qos_flow_information {
  /// Maximum Bit Rate in DL.
  uint64_t max_br_dl;
  /// Maximum Bit Rate in UL.
  uint64_t max_br_ul;
  /// Guaranteed Flow Bit Rate (provided there is data to deliver) in DL.
  uint64_t gbr_dl;
  /// Guaranteed Flow Bit Rate (provided there is data to deliver) in UL.
  uint64_t gbr_ul;
  /// Indicates the maximum rate for lost packets that can be tolerated in the DL. Expressed in ratio of lost packets
  /// per number of packets sent, expressed in tenth of percent.Values {0,...,1000}.
  std::optional<uint16_t> max_packet_loss_rate_dl;
  /// Indicates the maximum rate for lost packets that can be tolerated in the UL. Expressed in ratio of lost packets
  /// per number of packets sent, expressed in tenth of percent.Values {0,...,1000}.
  std::optional<uint16_t> max_packet_loss_rate_ul;
};

struct alloc_and_retention_priority {
  /// Priority level, where highest is 1, lowest is 14, no-priority is 15. The value 0 is reserved.
  uint8_t prio_level_arp         = 15;
  bool    may_trigger_preemption = false;
  bool    is_preemptable         = false;
};

struct qos_flow_level_qos_parameters {
  qos_characteristics_t        qos_characteristics;
  alloc_and_retention_priority alloc_retention_prio;
  /// This parameter applies to GBR flows only. See TS 38.473, clause 9.3.1.45.
  std::optional<gbr_qos_flow_information> gbr_qos_info;
  bool                                    add_qos_flow_info = false;
  /// This parameter applies to non-GBR flows only. See TS 23.501.
  bool reflective_qos_attribute_subject_to = false;
};

} // namespace srsran