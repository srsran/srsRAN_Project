/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

/// \file
/// \brief Generation of packed RRC messages for testing purposes. Use this file when you don't want to include
/// the RRC ASN.1 headers.

#include "srsran/adt/byte_buffer.h"
#include "srsran/ran/subcarrier_spacing.h"

namespace srsran {

/// Generates a dummy RRC handoverPrepInformation as per TS 38.331.
byte_buffer create_ho_prep_info();

/// \brief Generates a dummy Measurement Timing Configuration.
byte_buffer create_meas_timing_cfg(uint32_t carrier_freq, subcarrier_spacing scs);

} // namespace srsran