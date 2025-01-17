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

#include "srsran/ran/qos/five_qi_qos_mapping.h"
#include <unordered_map>

using namespace srsran;

using qos_chars = standardized_qos_characteristics;
using flow_type = qos_flow_resource_type;

/// \brief Standardized 5QI to QoS characteristics mapping as per TS 23.501, table 5.7.4-1.
static const std::unordered_map<five_qi_t, qos_chars> five_qi_to_qos_mapping = {
    // clang-format off

    // GBR.
    {uint_to_five_qi(1),
     qos_chars{flow_type::gbr, qos_prio_level_t{20}, 100, packet_error_rate_t::make(1e-2), 2000, std::nullopt}},
    {uint_to_five_qi(2),
     qos_chars{flow_type::gbr, qos_prio_level_t{40}, 150, packet_error_rate_t::make(1e-3), 2000, std::nullopt}},
    {uint_to_five_qi(3),
     qos_chars{flow_type::gbr, qos_prio_level_t{30}, 50,  packet_error_rate_t::make(1e-3), 2000, std::nullopt}},
    {uint_to_five_qi(4),
     qos_chars{flow_type::gbr, qos_prio_level_t{50}, 300, packet_error_rate_t::make(1e-6), 2000, std::nullopt}},
    {uint_to_five_qi(65),
     qos_chars{flow_type::gbr,qos_prio_level_t{ 7},  75,  packet_error_rate_t::make(1e-2), 2000, std::nullopt}},
    {uint_to_five_qi(66),
     qos_chars{flow_type::gbr, qos_prio_level_t{20}, 100, packet_error_rate_t::make(1e-2), 2000, std::nullopt}},
    {uint_to_five_qi(67),
     qos_chars{flow_type::gbr, qos_prio_level_t{15}, 100, packet_error_rate_t::make(1e-3), 2000, std::nullopt}},

    // Non-GBR.
    {uint_to_five_qi(5),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{10}, 100, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(6),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{60}, 300, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(7),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{70}, 100, packet_error_rate_t::make(1e-3), std::nullopt, std::nullopt}},
    {uint_to_five_qi(8),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{80}, 300, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(9),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{90}, 300, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(69),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{5},  60,  packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(70),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{55}, 200, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(79),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{65}, 50,  packet_error_rate_t::make(1e-2), std::nullopt, std::nullopt}},
    {uint_to_five_qi(80),
     qos_chars{flow_type::non_gbr, qos_prio_level_t{68}, 10,  packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},

    // Delay Critical GBR.
    {uint_to_five_qi(82),
     qos_chars{flow_type::delay_critical_gbr, qos_prio_level_t{19}, 10,  packet_error_rate_t::make(1e-4), 2000, 255}},
    {uint_to_five_qi(83),
     qos_chars{flow_type::delay_critical_gbr, qos_prio_level_t{22}, 10,  packet_error_rate_t::make(1e-4), 2000, 1354}},
    {uint_to_five_qi(84),
     qos_chars{flow_type::delay_critical_gbr, qos_prio_level_t{24}, 30,  packet_error_rate_t::make(1e-5), 2000, 1354}},
    {uint_to_five_qi(85),
     qos_chars{flow_type::delay_critical_gbr, qos_prio_level_t{21}, 5,   packet_error_rate_t::make(1e-5), 2000, 255}},
    // clang-format on
};

const qos_chars* srsran::get_5qi_to_qos_characteristics_mapping(five_qi_t five_qi)
{
  const auto qos_char = five_qi_to_qos_mapping.find(five_qi);
  return qos_char != five_qi_to_qos_mapping.end() ? &qos_char->second : nullptr;
}
