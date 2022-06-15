/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_SIB_SCHEDULER_H

#include "../pdcch_scheduler/pdcch_scheduler.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/sched_consts.h"

namespace srsgnb {

struct cell_slot_resource_allocator;
enum class ssb_pattern_case;

/// This class schedules the SIB1 and fills the grant to be passed to passed to lower layers.
class sib1_scheduler
{
public:
  sib1_scheduler(const cell_configuration& cfg_,
                 pdcch_scheduler&          pdcch_sch,
                 uint8_t                   pdcch_config_sib1_,
                 uint8_t                   sib1_mcs_,
                 uint8_t                   sib1_rv_,
                 aggregation_level         sib1_dci_aggr_lev_,
                 unsigned                  sib1_rxtx_periodicity_,
                 subcarrier_spacing        scs_common);

  /// \brief Performs beams' SIB1s (if any) scheduling for the current slot.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] sl_point Slot for which the SIB1 scheduler is called.
  void schedule_sib1(cell_slot_resource_allocator& res_grid, const slot_point sl_point);

private:
  /// \brief Computes the SIB1 n0 slot, at which each beam's SIB1 is allocated [TS 38.213, Section 13].
  ///
  /// These slots are computed and saved in the body of the constructor.
  /// \param[in] scs_common SCS corresponding to subCarrierSpacingCommon, which must coincide with SCS if initial
  /// DL BWP.
  void precompute_sib1_n0(subcarrier_spacing scs_common);

  /// \brief Searches in PDSCH and PDCCH for space to allocate SIB1 and SIB1's DCI, respectively.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] beam_idx SSB or beam index which the SIB1 corresponds to.
  bool allocate_sib1(cell_slot_resource_allocator& res_grid, unsigned beam_idx);

  /// \brief Fills  the SIB1 n0 slots, at which each beam's SIB1 is allocated.
  ///
  /// These slots are computed and saved in the body of the constructor.
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] beam_idx SSB or beam index which the SIB1 corresponds to.
  /// \param[in] sib1_crbs_grant CRBs interval in the PDSCH allocated for SIB1.
  void fill_sib1_grant(cell_slot_resource_allocator& res_grid, unsigned beam_idx, crb_interval sib1_crbs_grant);

  /// SIB1 Logger.
  srslog::basic_logger& logger = srslog::fetch_basic_logger("MAC");

  /// Parameters for SIB1 scheduling.
  const cell_configuration& cfg;
  pdcch_scheduler&          pdcch_sched;

  /// [Implementation-defined] SIB1 parameters.
  /// This included in MIB message and defines the CORESET 0 and SearchSpaceSet 0.
  uint8_t           pdcch_config_sib1;
  uint8_t           sib1_mcs;
  uint8_t           sib1_rv;
  aggregation_level sib1_dci_aggr_lev;
  /// This is a derived parameters, that depends on the SSB periodicity, SIB1 periodicity and SIB1 re-tx periodicity.
  unsigned sib1_periodicity;

  /// This is a dummy BWP configuration dimensioned based on CORESET#0 RB limits. It's used for CRB-to-PRB conversion.
  bwp_configuration coreset0_bwp_cfg;

  /// Vector of slots n0 (1 per beam) that will be used for SIB1 scheduling [TS 38.213, Section 13].
  static_vector<slot_point, MAX_NUM_BEAMS> sib1_n0_slots;
};

} // end of namespace srsgnb

#define SRSGNB_SIB_SCHEDULER_H

#endif // SRSGNB_SIB_SCHEDULER_H
