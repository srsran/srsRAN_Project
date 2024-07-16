/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../ue_scheduling/ue.h"

namespace srsran {

/// Container that store all UEs belonging to a slice.
using slice_ue_repository = slotted_id_table<du_ue_index_t, const ue*, MAX_NOF_DU_UES>;

} // namespace srsran
