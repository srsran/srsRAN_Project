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

#pragma once

/// \file
/// \brief Generation of packed RRC messages for testing purposes. Use this file when you don't want to include
/// the RRC ASN.1 headers.

#include "srsran/adt/byte_buffer.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {
namespace test_helpers {

/// Generates a dummy RRC handoverPrepInformation as per TS 38.331.
byte_buffer create_ho_prep_info();

/// \brief Generates a dummy Measurement Timing Configuration.
byte_buffer create_meas_timing_cfg(uint32_t carrier_freq, subcarrier_spacing scs);

/// \brief Generates a packed dummy SIB1 message.
byte_buffer create_packed_sib1();

/// \brief Generates a dummy SIB1 hex string.
std::string create_sib1_hex_string();

/// \brief Generates a dummy CellGroupConfig.
byte_buffer create_cell_group_config();

} // namespace test_helpers
} // namespace srsran
