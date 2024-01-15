/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

/// \file
/// \brief In this file, we provide a list of common checks to verify the validity of the scheduler output for a given
/// cell configuration. To maximize their applicability, these chosen checks should solely depend on the (assumed
/// immutable) cell configuration, and shouldn't depend on the scheduler or UE states. We assume that unit tests
/// checks should cover the latter cases.

#pragma once

#include "lib/scheduler/cell/resource_grid.h"
#include "srsran/scheduler/scheduler_slot_handler.h"

namespace srsran {

class cell_configuration;
struct cell_resource_allocator;

/// \brief Current checks:
/// - PDCCHs and PDSCHs are only allocated in DL slots.
/// - PUCCHs, PUSCHs and PRACHs are only allocated in UL slots.
/// - In partial DL/UL slots, verifies that allocations are within the respective DL/UL symbols.
void assert_tdd_pattern_consistency(const cell_configuration& cell_cfg, slot_point sl_tx, const sched_result& result);

/// \brief Current checks:
/// - RNTIs match.
/// - The PDSCH symbols match chosen DCI time domain resource assignment.
/// - The PDSCH PRBs match the chosen DCI freq domain resource assignment.
void assert_pdcch_pdsch_common_consistency(const cell_configuration&   cell_cfg,
                                           const pdcch_dl_information& pdcch,
                                           const pdsch_information&    pdsch);

/// \brief Verifies that DL PDCCHs have an associated PDSCH and that the common parameters between both match. Current
/// checks:
/// - A DL PDCCH always has an associated PDSCH with the same RNTI, taking into account k0 >= 0.
/// - The symbol and PRB parameters of the DCI content match the PDSCH parameters.
void assert_pdcch_pdsch_common_consistency(const cell_configuration&      cell_cfg,
                                           const cell_resource_allocator& cell_res_grid);

/// \brief Current checks:
/// - If CORESET#0 is not defined, SIB1 cannot be scheduled.
/// - SIB must use alloc_type_1.
/// - SIB PRBs must fall within CORESET#0 RB boundaries.
/// \param cell_cfg Cell configuration.
/// \param sibs Scheduled SIBs in a given slot.
void test_pdsch_sib_consistency(const cell_configuration& cell_cfg, span<const sib_information> sibs);

/// \brief Verify RAR content is valid. Current checks:
/// - RAR PRBS must fall within CORESET#0 RB boundaries.
/// - RAR must be alloc_type_1.
/// - No repeated RA-RNTIs across RAR grants and no repeated C-RNTIs across Msg3 grants.
/// - Consistent content in DCI of RARs (e.g. has to be f1_0, PRBs fall within CORESET#0 RB limits).
void test_pdsch_rar_consistency(const cell_configuration& cell_cfg, span<const rar_information> rars);

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
void test_scheduler_result_consistency(const cell_configuration& cell_cfg,
                                       slot_point                sl_tx,
                                       const sched_result&       result);

/// \brief Run all consistency checks for multiple slot scheduler results.
void test_scheduler_result_consistency(const cell_configuration&      cell_cfg,
                                       const cell_resource_allocator& cell_res_grid);

/// \brief Verifies that the cell resource grid PRBs and symbols was filled with the allocated PUCCHs.
// TODO: Replace this function with a more thorough check.
bool assert_ul_resource_grid_filled(const cell_configuration&      cell_cfg,
                                    const cell_resource_allocator& cell_res_grid,
                                    unsigned                       tx_delay,
                                    bool                           expect_grants = false);

bool test_res_grid_has_re_set(const cell_resource_allocator& cell_res_grid, grant_info grant, unsigned tx_delay);

} // namespace srsran
