/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/span.h"
#include "srsran/ran/uci/uci_part2_size_description.h"
#include "srsran/ran/uci/uci_payload_type.h"

namespace srsran {

/// \brief Calculates the UCI part 2 from UCI part 1.
/// \param[in] part1 UCI part 1 decoded data.
/// \param[in] descr UCI part 1 parameters correspondence to UCI part 2 size.
/// \return The size of UCI part 2 payload.
unsigned uci_part2_get_size(const uci_payload_type& part1, const uci_part2_size_description& descr);

} // namespace srsran
