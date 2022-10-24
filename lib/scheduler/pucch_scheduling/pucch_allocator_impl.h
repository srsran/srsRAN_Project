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
  void slot_indication(slot_point slot_tx)
  {
    // Update Slot.
    last_sl_ind = slot_tx;

    get_slot_resource_counter(last_sl_ind - 1).sr_resource_available   = true;
    get_slot_resource_counter(last_sl_ind - 1).next_pucch_harq_res_idx = 0;
  }

  /// Returns the index of the PUCCH resource to be used for HARQ-ACK.
  /// \remark: This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  /// \return The \c pucch-ResourceId of the resource to be used by the scheduler.
  const pucch_resource* get_next_harq_res_available(slot_point slot_harq, const pucch_config& pucch_cfg)
  {
    srsgnb_sanity_check(slot_harq < last_sl_ind + SLOT_RES_COUNTER_RING_SIZE,
                        "PDCCH being allocated to far into the future");

    const auto& pucch_res_list = pucch_cfg.pucch_res_list;
    return get_slot_resource_counter(slot_harq).next_pucch_harq_res_idx < pucch_res_list.size()
               ? &pucch_res_list[get_slot_resource_counter(slot_harq).next_pucch_harq_res_idx]
               : nullptr;
  };

  /// Returns the index of the PUCCH resource to be used for SR.
  /// \remark: This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  /// \remark: There is only one resource used for SR.
  /// \return The \c pucch-ResourceId of the resource to be used by the scheduler.
  const pucch_resource* get_next_sr_res_available(slot_point slot_sr, const pucch_config& pucch_cfg)
  {
    srsgnb_sanity_check(slot_sr < last_sl_ind + SLOT_RES_COUNTER_RING_SIZE,
                        "PDCCH being allocated to far into the future");
    srsgnb_sanity_check(pucch_cfg.sr_res_list.size() == 1, "UE SR resource list must have size 1.");

    if (get_slot_resource_counter(slot_sr).sr_resource_available) {
      const auto& pucch_res_list = pucch_cfg.pucch_res_list;

      // Check if the list of PUCCH resources (corresponding to \c resourceToAddModList, as part of \c PUCCH-Config, as
      // per TS 38.331) contains the resource indexed to be used for SR.
      const auto sr_pucch_resource_cfg =
          std::find_if(pucch_res_list.begin(),
                       pucch_res_list.end(),
                       [sr_res_idx = pucch_cfg.sr_res_list[0].pucch_res_id](const pucch_resource& pucch_sr_res_cfg) {
                         return static_cast<unsigned>(sr_res_idx) == pucch_sr_res_cfg.res_id;
                       });

      // If there is no such PUCCH resource, skip to the next SR resource.
      if (sr_pucch_resource_cfg == pucch_res_list.end()) {
        // TODO: Add information about the LC which this SR is for.
        return nullptr;
      }

      get_slot_resource_counter(slot_sr).sr_resource_available = false;
      return &(*sr_pucch_resource_cfg);
    }
    return nullptr;
  };

private:
  // Keeps track if the PUCCH resource is used alread (SR) or how many times the resource has been used (HARQ).
  struct slot_resource_counter {
    unsigned next_pucch_harq_res_idx{0};
    bool     sr_resource_available{true};
  };
  static const size_t SLOT_RES_COUNTER_RING_SIZE = 20;

  slot_resource_counter& get_slot_resource_counter(slot_point sl)
  {
    srsgnb_sanity_check(sl < last_sl_ind + SLOT_RES_COUNTER_RING_SIZE, "PDCCH being allocated to far into the future");
    return resource_slots[sl.to_uint() % SLOT_RES_COUNTER_RING_SIZE];
  }

  /// Size of the ring buffer of pdcch_slot_allocator. This size sets a limit on how far in advance a PDCCH can be
  /// allocated.

  std::array<slot_resource_counter, SLOT_RES_COUNTER_RING_SIZE> resource_slots;

  slot_point last_sl_ind;
};

/// Implementation of the PUCCH scheduler interface.
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

  void pucch_allocate_sr_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                     rnti_t                        crnti,
                                     const ue_cell_configuration&  ue_cell_cfg) override;

  void slot_indication(slot_point sl_tx)
  {
    //
    srsgnb_sanity_check(not last_sl_ind.valid() or sl_tx == last_sl_ind + 1, "Detected skipped slot");

    // Update Slot.
    last_sl_ind = sl_tx;

    resource_manager.slot_indication(sl_tx);
  }

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

  void allocate_pucch_on_grid(cell_slot_resource_allocator& pucch_slot_alloc, const pucch_resource& pucch_sr_res);

  void fill_pucch_sr_grant(pucch_info&           pucch_sr_grant,
                           rnti_t                crnti,
                           const pucch_resource& pucch_sr_res,
                           const pucch_config&   pucch_cfg,
                           unsigned              harq_ack_bits);

  // Cell configuration.
  const cell_configuration& cell_cfg;

  slot_point last_sl_ind;

  pucch_resource_manager resource_manager;

  const unsigned PUCCH_FORMAT_1_NOF_PRBS{1};

  srslog::basic_logger& logger;
};

} // namespace srsgnb
