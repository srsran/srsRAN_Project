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
#include "pucch_allocator.h"
#include "srsgnb/scheduler/scheduler_dci.h"

namespace srsgnb {

/// Class that is in charge of providing the scheduler an available PUCCH resource to be used, either for HARQ or SR.
class pucch_resource_manager
{
public:
  // Reset all resources to "unused".
  void slot_indication(slot_point slot_tx);

  /// Returns the index of the PUCCH resource to be used for HARQ-ACK.
  /// \remark: This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  /// \return The \c pucch-ResourceId of the resource to be used by the scheduler for HARQ-ACK, if available;
  ///         else, returns \c nullptr.
  const pucch_resource* get_next_harq_res_available(slot_point slot_harq, const pucch_config& pucch_cfg);

  /// Returns the index of the PUCCH resource to be used for SR.
  /// \remark This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  /// \remark There is only one resource used for SR.
  /// \return The \c pucch-ResourceId of the resource to be used by the scheduler for SR, if available; else, returns
  ///         \c nullptr.
  const pucch_resource* get_next_sr_res_available(slot_point slot_sr, const pucch_config& pucch_cfg);

private:
  // Keeps track of whether the PUCCH resource is used alread (SR) or of how many times the resource has been used
  // (HARQ).
  struct slot_resource_counter {
    unsigned next_pucch_harq_res_idx{0};
    bool     sr_resource_available{true};
  };

  // Size of the ring buffer of slot_resource_counter.
  static const size_t SLOT_RES_COUNTER_RING_SIZE = 20;

  slot_resource_counter& get_slot_resource_counter(slot_point sl);

  // Ring buffer of slot_resource_counter for PUCCH resources.
  std::array<slot_resource_counter, SLOT_RES_COUNTER_RING_SIZE> resource_slots;

  // Keeps track of the last slot_point used by the resource manager.
  slot_point last_sl_ind;
};

/// Implementation of the PUCCH allocator interface.
class pucch_allocator_impl final : public pucch_allocator
{
public:
  explicit pucch_allocator_impl(const cell_configuration& cell_cfg_);

  ~pucch_allocator_impl() override;

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

  /// Allocate the PUCCH resource for a SR's opportunity for a given UE.
  /// \param[out,in] slot_alloc struct with scheduling results.
  /// \param[in] crnti C-RNTI  of the UE.
  /// \param[in] ue_cell_cfg user configuration.
  void pucch_allocate_sr_opportunity(cell_slot_resource_allocator& slot_alloc,
                                     rnti_t                        crnti,
                                     const ue_cell_configuration&  ue_cell_cfg) override;

  /// Updates the internal slot_point and tracking of PUCCH resource usage over time.
  void slot_indication(slot_point sl_tx);

private:
  // Structs with the info about the PUCCH resources.
  struct pucch_res_alloc_cfg {
    // True if the struct has a valid config.
    bool       has_config{false};
    grant_info first_hop_res;
    // Contains grant only if intra-slot freq-hopping is active.
    grant_info second_hop_res;
    // Cyclic-shift.
    unsigned cs;
    // PUCCH format.
    pucch_format format;
  };

  // Allocates the PUCCH (common) resource for HARQ-(N)-ACK.
  pucch_res_alloc_cfg alloc_pucch_common_res_harq(unsigned&                      pucch_res_indicator,
                                                  cell_slot_resource_allocator&  pucch_alloc,
                                                  const dci_context_information& dci_info);

  // Fills the PUCCH HARQ grant for common resources.
  void fill_pucch_harq_grant(pucch_info& pucch_info, rnti_t rnti, const pucch_res_alloc_cfg& pucch_res);

  // Allocates the PUCCH SR resource on grid.
  void allocate_pucch_sr_on_grid(cell_slot_resource_allocator& pucch_slot_alloc, const pucch_resource& pucch_sr_res);

  // Fills the PUCCH SR grant.
  void fill_pucch_sr_grant(pucch_info&           pucch_sr_grant,
                           rnti_t                crnti,
                           const pucch_resource& pucch_sr_res,
                           unsigned              harq_ack_bits);

  const unsigned            PUCCH_FORMAT_1_NOF_PRBS{1};
  const cell_configuration& cell_cfg;
  slot_point                last_sl_ind;
  pucch_resource_manager    resource_manager;

  srslog::basic_logger& logger;
};

} // namespace srsgnb
