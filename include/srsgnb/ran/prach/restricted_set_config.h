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

/// \brief PRACH Restricted set configuration.
///
/// PRACH \f$N_{CS}\f$ restricted set configuration. Used for interpreting TS38.211 Tables 6.3.3.1-5 and 6.3.3.1-6.
enum class restricted_set_config { UNRESTRICTED, TYPE_A, TYPE_B };

} // namespace srsgnb