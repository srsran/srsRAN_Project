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

/// Identification of an HARQ process.
enum harq_id_t { MAX_HARQ_ID = 15, MAX_NOF_HARQS = 16, INVALID_HARQ_ID = 16 };

constexpr inline harq_id_t to_harq_id(unsigned h_id)
{
  return static_cast<harq_id_t>(h_id);
}

} // namespace srsgnb