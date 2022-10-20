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

#include "../cell/resource_grid.h"
#include "../ue_scheduling/ue_configuration.h"
#include "pucch_scheduler.h"
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

/// Class that is in charge of providing the scheduler an available PUCCH resource to be used, either for HARQ or SR.
class pucch_resource_manager
{
public:
  explicit pucch_resource_manager(const uplink_config& ul_config);

  // Reset all resources to "unused".
  void reset()
  {
    sr_resource_available = true;
    next_harq_res         = 0;
  };

  /// Returns the index of the PUCCH resource to be used for HARQ-ACK.
  /// \remark: This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  /// \return The \c pucch-ResourceId of the resource to be used by the scheduler.
  int get_next_harq_res_available()
  {
    return next_harq_res < pucch_harq_resources.size() ? pucch_harq_resources[next_harq_res++] : -1;
  };

  /// Returns the index of the PUCCH resource to be used for SR.
  /// \remark: This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  /// \remark: There is only one resource used for SR.
  /// \return The \c pucch-ResourceId of the resource to be used by the scheduler.
  int get_next_sr_res_available();

private:
  std::vector<int> pucch_harq_resources;

  int sr_resource_idx;
  // true = unused; false = used.
  bool   sr_resource_available;
  size_t next_harq_res;
};

/// Implementation of the PUCCH scheduler interface.
class pucch_scheduler_impl final : public pucch_scheduler
{
public:
  explicit pucch_scheduler_impl(const cell_configuration& cell_cfg_, const uplink_config& ul_config, ue_list& ues_);

  ~pucch_scheduler_impl() override;

  /// Allocate the PUCCH resource for HARQ-ACK for a given UE.
  /// \param[out,in] slot_alloc struct with scheduling results.
  /// \param[in] tcrnti temporary RNTI  of the UE.
  /// \param[in] pdsch_time_domain_resource k0 value, or delay (in slots) of PDSCH slot vs the corresponding PDCCH slot.
  /// \param[in] k1 delay in slots of the UE's PUCCH HARQ-ACK report with respect to the PDSCH.
  /// \param[in] dci_info information with DL DCI, needed for HARQ-(N)-ACK scheduling info.
  /// \return The grant for the UE's PUCCH HARQ-(N)-ACK report.
  pucch_harq_ack_grant alloc_common_pucch_harq_ack_ue(cell_resource_allocator&    slot_alloc,
                                                      rnti_t                      tcrnti,
                                                      unsigned                    pdsch_time_domain_resource,
                                                      unsigned                    k1,
                                                      const pdcch_dl_information& dci_info) override;

  /// Allocate the PUCCH resource all UEs's SR opportunities.
  /// \param[out,in] res_alloc struct with scheduling results.
  /// \param[in] sl_tx slot for which the SR should be allocated.
  void pucch_run_sr_allocator(cell_resource_allocator& res_alloc, slot_point sl_tx) override;

private:
  // Structs with the info about the PUCCH resources.
  struct pucch_res_alloc_cfg {
    /// True if the struct has a valid config.
    bool       has_config{false};
    grant_info first_hop_res;
    // Contains grant only if intra-slot freq-hopping is active.
    optional<grant_info> second_hop_res;
    // Cyclic-shift.
    unsigned cs;
    // PUCCH format.
    pucch_format format;
  };

  // Allocates the PUCCH (common) resource for HARQ-(N)-ACK.
  pucch_res_alloc_cfg alloc_pucch_common_res_harq(unsigned&                         pucch_res_indicator,
                                                  cell_slot_resource_allocator&     pucch_alloc,
                                                  const dci_dl_context_information& dci_info);

  // Fills the output of the PUCCH scheduler.
  void fill_pucch_res_output(pucch_info& pucch_info, rnti_t rnti, pucch_res_alloc_cfg pucch_res);

  void sched_ue_sr_opportunity(cell_resource_allocator& cell_alloc, ue& user, slot_point sl_tx);

  void pucch_allocate_sr(cell_slot_resource_allocator& pucch_slot_alloc,
                         rnti_t                        crnti,
                         const pucch_info*             existing_pucch_harq_grant,
                         const ue_cell_configuration&  ue_cell_cfg,
                         const pucch_resource&         pucch_sr_res);

  void allocate_pucch_on_grid(cell_slot_resource_allocator& pucch_slot_alloc, const pucch_resource& pucch_sr_res);

  void fill_pucch_sr_grant(pucch_info&           pucch_sr_grant,
                           rnti_t                crnti,
                           const pucch_resource& pucch_sr_res,
                           const pucch_config&   pucch_cfg,
                           unsigned              harq_ack_bits);

  // Cell configuration.
  const cell_configuration& cell_cfg;
  pucch_resource_manager    resource_manager;

  const unsigned PUCCH_FORMAT_1_NOF_PRBS{1};

  ue_list& ues;

  srslog::basic_logger& logger;
};

} // namespace srsgnb
