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

#include <cstddef>

namespace srsran {

/// \brief Maximum size in bytes of a PDSCH PDU for a given UE.
///
/// It is not given by the TS. It assumes 156 resource elements for a maximum of 275 PRB, four layers and 256-QAM.
static constexpr size_t MAX_DL_PDU_LENGTH = (156 * 275 * 4 * 8) / 8;

} // namespace srsran
