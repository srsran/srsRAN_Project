/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RAN_PRACH_PRACH_PREAMBLES_H
#define SRSGNB_RAN_PRACH_PRACH_PREAMBLES_H

namespace srsgnb {

/// \brief PRACH preamble formats.
///
/// PRACH preamble formats are described in TS38.211 Table 6.3.3.1-1 and 6.3.3.1-2.
enum class preamble_format { FORMAT0 = 0, FORMAT1, FORMAT2, FORMAT3, OTHER };

} // namespace srsgnb

#endif // SRSGNB_RAN_PRACH_PRACH_PREAMBLES_H
