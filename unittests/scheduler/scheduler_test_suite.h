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
/// \param cell_cfg Cell configuration.
/// \param sibs Scheduled SIBs in a given slot.
void test_pdsch_sib_consistency(const cell_configuration& cell_cfg, span<const sib_information> sibs);

/// \brief Current checks:
/// - PRACH occasions parameters match RACH-ConfigCommon present in cell_cfg.
/// \param cell_cfg Cell configuration.
/// \param prachs Scheduled PRACH opportunities in a given slot.
void test_prach_opportunity_validity(const cell_configuration& cell_cfg, span<const prach_occasion_info> prachs);

/// \brief Detects collisions in the "RB x symbol" DL resource grid.
void test_dl_resource_grid_collisions(const cell_configuration& cell_cfg, const dl_sched_result& result);

/// \brief Detects collisions in the "RB x symbol" UL resource grid.
void test_ul_resource_grid_collisions(const cell_configuration& cell_cfg, const ul_sched_result& result);

/// \brief Run all consistency checks for the scheduler result.
void test_scheduler_result_consistency(const cell_configuration& cell_cfg, const sched_result& result);

} // namespace srsgnb
