/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Validators of scheduler result messages
///
/// The validators in this file only perform checks that do not depend on any configuration or contextual information
/// of the scheduler or DU-high.

#pragma once

namespace srsran {
namespace test_helper {

/// \brief Determine if the PDSCH grant for a given UE has valid content.
bool is_valid_dl_msg_alloc(const dl_msg_alloc& grant);

/// \brief Determine if the PUSCH grant for a given UE has valid content.
bool is_valid_ul_sched_info(const ul_sched_info& grant);

} // namespace test_helper
} // namespace srsran
