/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/ran/qos/five_qi_qos_mapping.h"
#include <unordered_map>

using namespace srsran;

/// \brief Standardized 5QI to QoS characteristics mapping as per TS 23.501, table 5.7.4-1.
static const std::unordered_map<five_qi_t, qos_characteristics> five_qi_to_qos_mapping = {
    // clang-format off

    // GBR.
    {uint_to_five_qi(1),
     qos_characteristics{srsran::qos_flow_resource_type::gbr, 20, 100, packet_error_rate_t::make(1e-2), 2000, std::nullopt}},
    {uint_to_five_qi(2),
     qos_characteristics{srsran::qos_flow_resource_type::gbr, 40, 150, packet_error_rate_t::make(1e-3), 2000, std::nullopt}},
    {uint_to_five_qi(3),
     qos_characteristics{srsran::qos_flow_resource_type::gbr, 30, 50,  packet_error_rate_t::make(1e-3), 2000, std::nullopt}},
    {uint_to_five_qi(4),
     qos_characteristics{srsran::qos_flow_resource_type::gbr, 50, 300, packet_error_rate_t::make(1e-6), 2000, std::nullopt}},
    {uint_to_five_qi(65),
     qos_characteristics{srsran::qos_flow_resource_type::gbr, 7,  75,  packet_error_rate_t::make(1e-2), 2000, std::nullopt}},
    {uint_to_five_qi(66),
     qos_characteristics{srsran::qos_flow_resource_type::gbr, 20, 100, packet_error_rate_t::make(1e-2), 2000, std::nullopt}},
    {uint_to_five_qi(67),
     qos_characteristics{srsran::qos_flow_resource_type::gbr, 15, 100, packet_error_rate_t::make(1e-3), 2000, std::nullopt}},

    // Non-GBR.
    {uint_to_five_qi(5),
     qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 10, 100, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(6),
     qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 60, 300, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(7),
     qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 70, 100, packet_error_rate_t::make(1e-3), std::nullopt, std::nullopt}},
    {uint_to_five_qi(8),
     qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 80, 300, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(9),
     qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 90, 300, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(69),
     qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 5,  60,  packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(70),
     qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 55, 200, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(79),
     qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 65, 50,  packet_error_rate_t::make(1e-2), std::nullopt, std::nullopt}},
    {uint_to_five_qi(80),
     qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 68, 10,  packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},

    // Delay Critical GBR.
    {uint_to_five_qi(82),
     qos_characteristics{srsran::qos_flow_resource_type::delay_critical_gbr, 19, 10,  packet_error_rate_t::make(1e-4), 2000, 255}},
    {uint_to_five_qi(83),
     qos_characteristics{srsran::qos_flow_resource_type::delay_critical_gbr, 22, 10,  packet_error_rate_t::make(1e-4), 2000, 1354}},
    {uint_to_five_qi(84),
     qos_characteristics{srsran::qos_flow_resource_type::delay_critical_gbr, 24, 30,  packet_error_rate_t::make(1e-5), 2000, 1354}},
    {uint_to_five_qi(85),
     qos_characteristics{srsran::qos_flow_resource_type::delay_critical_gbr, 21, 5,   packet_error_rate_t::make(1e-5), 2000, 255}},
    // clang-format on
};

const qos_characteristics* srsran::get_5qi_to_qos_characteristics_mapping(five_qi_t five_qi)
{
  const auto qos_char = five_qi_to_qos_mapping.find(five_qi);
  return qos_char != five_qi_to_qos_mapping.end() ? &qos_char->second : nullptr;
}
