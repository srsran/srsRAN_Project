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

#include "five_qi_qos_mapping.h"
#include "srsran/ran/qos/five_qi.h"
#include "srsran/ran/qos/packet_error_rate.h"
#include "srsran/ran/qos/qos_prio_level.h"
#include <optional>
#include <variant>

namespace srsran {

struct dyn_5qi_descriptor {
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

struct non_dyn_5qi_descriptor {
  /// Standardized 5QI. Must contain one of the 5QIs in TS23.501, Table 5.7.4-1. The default is a non-GBR 5QI.
  five_qi_t five_qi = uint_to_five_qi(9);
  /// Priority level, in case the default priority level specified in TS23.501, Table 5.7.4-1 is not used.
  std::optional<qos_prio_level_t> qos_prio_level;
  /// Averaging Window, in case the default value specified in TS23.501, Table 5.7.4-1 is not used.
  std::optional<uint16_t> averaging_win;
  /// \brief Maximum Data Burst Volume, in case the default value specified in TS23.501, Table 5.7.4-1 is not used.
  /// This value should only be used in delay-critical GBR DRBs.
  std::optional<uint16_t> max_data_burst_volume;
};

/// QoS Parameters of either a dynamic or non-dynamic 5QI.
struct qos_characteristics {
  qos_characteristics() : choice(non_dyn_5qi_descriptor{}) {}
  qos_characteristics(const non_dyn_5qi_descriptor& val) : choice(val) {}
  qos_characteristics(const dyn_5qi_descriptor& val) : choice(val) {}

  bool is_dyn_5qi() const { return std::holds_alternative<dyn_5qi_descriptor>(choice); }

  /// Retrieves the 5QI associated with the QoS characteristics.
  five_qi_t get_5qi() const
  {
    if (is_dyn_5qi()) {
      return get_dyn_5qi().five_qi.has_value() ? get_dyn_5qi().five_qi.value() : five_qi_t::invalid;
    }
    return get_nondyn_5qi().five_qi;
  }

  dyn_5qi_descriptor&           get_dyn_5qi() { return std::get<dyn_5qi_descriptor>(choice); }
  const dyn_5qi_descriptor&     get_dyn_5qi() const { return std::get<dyn_5qi_descriptor>(choice); }
  non_dyn_5qi_descriptor&       get_nondyn_5qi() { return std::get<non_dyn_5qi_descriptor>(choice); }
  const non_dyn_5qi_descriptor& get_nondyn_5qi() const { return std::get<non_dyn_5qi_descriptor>(choice); }

private:
  std::variant<non_dyn_5qi_descriptor, dyn_5qi_descriptor> choice;
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
  /// QoS Descriptor of a dynamic or non-dynamic 5QI.
  qos_characteristics          qos_desc;
  alloc_and_retention_priority alloc_retention_prio;
  /// This parameter applies to GBR flows only. See TS 38.473, clause 9.3.1.45.
  std::optional<gbr_qos_flow_information> gbr_qos_info;
  bool                                    add_qos_flow_info = false;
  /// This parameter applies to non-GBR flows only. See TS 23.501.
  bool reflective_qos_attribute_subject_to = false;
};

} // namespace srsran