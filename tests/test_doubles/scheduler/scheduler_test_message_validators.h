/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Validators of scheduler result messages.
///
/// The validators in this file only perform checks that do not depend on any configuration or contextual information
/// of the scheduler or DU-high.

#pragma once

#include "srsran/adt/span.h"

namespace srsran {

struct dl_msg_alloc;
struct ul_sched_info;

namespace test_helper {

/// \brief Determine if the PDSCH grant for a given UE has valid content.
bool is_valid_dl_msg_alloc(const dl_msg_alloc& grant, std::optional<coreset_configuration> coreset0);

/// \brief Determine if the PUSCH grant for a given UE has valid content.
bool is_valid_ul_sched_info(const ul_sched_info& grant);

/// \brief Determine if the UE PDSCH grants for a given slot are valid.
bool is_valid_dl_msg_alloc_list(span<const dl_msg_alloc> grants, std::optional<coreset_configuration> coreset0);

} // namespace test_helper
} // namespace srsran
