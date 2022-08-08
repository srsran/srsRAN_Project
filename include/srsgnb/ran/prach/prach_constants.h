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

namespace srsgnb {

namespace prach_constants {

/// Long sequence length as per TS38.211 Section 6.3.3.1, case \f$L_{RA}=839\$.
static constexpr unsigned LONG_SEQUENCE_LENGTH = 839U;

/// Long sequence PRACH preambles maximum number of symbols. Derived from TS38.211 Table 6.3.3.1-1.
static constexpr unsigned LONG_SEQUENCE_MAX_NOF_SYMBOLS = 4U;

/// Short sequence length as per TS38.211 Section 6.3.3.1, case \f$L_{RA}=139\$.
static constexpr unsigned SHORT_SEQUENCE_LENGTH = 139U;

/// Short sequence PRACH preambles maximum number of symbols. Derived from TS38.211 Table 6.3.3.1-2.
static constexpr unsigned SHORT_SEQUENCE_MAX_NOF_SYMBOLS = 12U;

} // namespace prach_constants

} // namespace srsgnb