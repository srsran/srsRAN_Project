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

#include "srsran/ran/ssb/pbch_mib_message.h"
#include <cstdint>

namespace srsran {

/// \brief Packs the PBCH payload including the Master Information Block (MIB).
///
/// The PBCH payload generation is described in TS38.212 Section 7.1.1. The MIB contents are described by the
/// Information Element \e MIB in TS38.331 Section 6.2.2.
///
/// \param[in] msg   PBCH Message containing an MIB for packing.
/// \param[in] L_max Maximum number of SS/PBCH block candidates in a 5ms burst, described in TS38.213 Section 4.1.
/// \return Packed PBCH payload.
uint32_t pbch_mib_pack(const pbch_mib_message& msg, unsigned L_max);

} // namespace srsran
