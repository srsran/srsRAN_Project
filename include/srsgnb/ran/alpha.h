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

/// The IE Alpha defines possible values of a the pathloss compensation coefficient for uplink power control. Value
/// alpha0 corresponds to the value 0, Value alpha04 corresponds to the value 0.4, and so on. Value alpha1 corresponds
/// to value 1.
/// \remark See TS 38.331, Alpha. Also, see clause 7.1 of TS 38.213.
enum class alpha : unsigned { alpha0, alpha04, alpha05, alpha06, alpha07, alpha08, alpha09, alpha1, not_set };

} // namespace srsgnb
