/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

/// \brief Standardized 5QI to QoS characteristics mapping as per TS 23.501, table 5.7.4-1.
static const std::unordered_map<five_qi_t, standardized_qos_characteristics> five_qi_to_qos_mapping = {
    // clang-format off

    // GBR.
    {uint_to_five_qi(1),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::gbr, 20, 100, packet_error_rate_t::make(1e-2), 2000, std::nullopt}},
    {uint_to_five_qi(2),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::gbr, 40, 150, packet_error_rate_t::make(1e-3), 2000, std::nullopt}},
    {uint_to_five_qi(3),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::gbr, 30, 50,  packet_error_rate_t::make(1e-3), 2000, std::nullopt}},
    {uint_to_five_qi(4),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::gbr, 50, 300, packet_error_rate_t::make(1e-6), 2000, std::nullopt}},
    {uint_to_five_qi(65),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::gbr, 7,  75,  packet_error_rate_t::make(1e-2), 2000, std::nullopt}},
    {uint_to_five_qi(66),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::gbr, 20, 100, packet_error_rate_t::make(1e-2), 2000, std::nullopt}},
    {uint_to_five_qi(67),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::gbr, 15, 100, packet_error_rate_t::make(1e-3), 2000, std::nullopt}},

    // Non-GBR.
    {uint_to_five_qi(5),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 10, 100, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(6),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 60, 300, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(7),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 70, 100, packet_error_rate_t::make(1e-3), std::nullopt, std::nullopt}},
    {uint_to_five_qi(8),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 80, 300, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(9),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 90, 300, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(69),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 5,  60,  packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(70),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 55, 200, packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},
    {uint_to_five_qi(79),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 65, 50,  packet_error_rate_t::make(1e-2), std::nullopt, std::nullopt}},
    {uint_to_five_qi(80),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::non_gbr, 68, 10,  packet_error_rate_t::make(1e-6), std::nullopt, std::nullopt}},

    // Delay Critical GBR.
    {uint_to_five_qi(82),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::delay_critical_gbr, 19, 10,  packet_error_rate_t::make(1e-4), 2000, 255}},
    {uint_to_five_qi(83),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::delay_critical_gbr, 22, 10,  packet_error_rate_t::make(1e-4), 2000, 1354}},
    {uint_to_five_qi(84),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::delay_critical_gbr, 24, 30,  packet_error_rate_t::make(1e-5), 2000, 1354}},
    {uint_to_five_qi(85),
     standardized_qos_characteristics{srsran::qos_flow_resource_type::delay_critical_gbr, 21, 5,   packet_error_rate_t::make(1e-5), 2000, 255}},
    // clang-format on
};

const standardized_qos_characteristics* srsran::get_5qi_to_qos_characteristics_mapping(five_qi_t five_qi)
{
  const auto qos_char = five_qi_to_qos_mapping.find(five_qi);
  return qos_char != five_qi_to_qos_mapping.end() ? &qos_char->second : nullptr;
}
