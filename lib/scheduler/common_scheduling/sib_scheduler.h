/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_SCHEDULER_COMMON_SCHEDULING_SIB_SCHEDULER_H
#define SRSGNB_LIB_SCHEDULER_COMMON_SCHEDULING_SIB_SCHEDULER_H

#include "../pdcch_scheduling/pdcch_scheduler.h"
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
  sib1_scheduler(const cell_configuration&                       cfg_,
                 pdcch_scheduler&                                pdcch_sch,
                 const sched_cell_configuration_request_message& msg);

  /// \brief Performs beams' SIB1s (if any) scheduling for the current slot.
  ///
  /// \param[out,in] res_grid Resource grid with current allocations and scheduling results.
  /// \param[in] sl_point Slot for which the SIB1 scheduler is called.
  void schedule_sib1(cell_slot_resource_allocator& res_grid, slot_point sl_point);

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
  void fill_sib1_grant(cell_slot_resource_allocator& res_grid,
                       crb_interval                  sib1_crbs_grant,
                       unsigned                      time_resource,
                       const dmrs_information&       dmrs_info,
                       unsigned                      tbs);

  /// SIB1 Logger.
  srslog::basic_logger& logger = srslog::fetch_basic_logger("MAC");

  /// Parameters for SIB1 scheduling.
  const cell_configuration& cell_cfg;
  pdcch_scheduler&          pdcch_sched;

  /// Parameters for SIB1 scheduling.
  uint8_t           coreset0;
  uint8_t           searchspace0;
  uint8_t           sib1_mcs;
  uint8_t           sib1_rv;
  aggregation_level sib1_dci_aggr_lev;
  /// The SIB1 payload is in bytes.
  unsigned sib1_payload_size;
  /// This is a derived parameters, that depends on the SSB periodicity, SIB1 periodicity and SIB1 re-tx periodicity.
  unsigned sib1_period;

  /// This is a dummy BWP configuration dimensioned based on CORESET#0 RB limits. It's used for CRB-to-PRB conversion.
  bwp_configuration coreset0_bwp_cfg;

  /// Vector of slots n0 (1 per beam) that will be used for SIB1 scheduling [TS 38.213, Section 13].
  static_vector<slot_point, MAX_NUM_BEAMS> sib1_n0_slots;
};

} // end of namespace srsgnb

#endif // SRSGNB_LIB_SCHEDULER_COMMON_SCHEDULING_SIB_SCHEDULER_H
