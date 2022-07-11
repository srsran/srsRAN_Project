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

#include "lib/scheduler/cell/cell_configuration.h"
#include "srsgnb/scheduler/scheduler_slot_handler.h"

namespace srsgnb {

/// \brief Current checks:
/// - RNTIs match.
/// - The PDSCH symbols match chosen DCI time domain resource assignment.
void test_pdcch_pdsch_common_consistency(const cell_configuration&   cell_cfg,
                                         const pdcch_dl_information& pdcch,
                                         const pdsch_information&    pdsch);

/// \brief Current checks:
/// - If CORESET#0 is not defined, SIB1 cannot be scheduled.
/// - SIB must use alloc_type_1.
/// - SIB PRBs must fall within CORESET#0 RB boundaries.
/// - SIB PRBs do not collide with other PDSCH allocations.
/// \param cell_cfg Cell configuration.
/// \param sibs Scheduled SIBs in a given slot.
/// \param[in/out] used_rbs optional parameter of currently used CRBs in the cell. It is used to detect collisions.
void test_pdsch_sib_consistency(const cell_configuration&   cell_cfg,
                                span<const sib_information> sibs,
                                prb_bitmap*                 used_rbs = nullptr);

/// \brief Run all consistency checks for the scheduler result.
void test_scheduler_result_consistency(const cell_configuration& cell_cfg, const sched_result& result);

} // namespace srsgnb
