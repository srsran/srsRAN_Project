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

/// Containers used to return the output of the PUCCH resource manager.
struct pucch_harq_resource_alloc_record {
  /// Pointer to PUCCH resource configuration to be used.
  const pucch_resource* pucch_res;
  /// PUCCH resource indicator corresponding to the resource that will be used by the UE.
  unsigned pucch_res_indicator;
};

/// Class that is in charge of providing the scheduler an available PUCCH resource to be used, either for HARQ or SR.
class pucch_resource_manager
{
public:
  pucch_resource_manager();

  // Reset all resources to "unused".
  void slot_indication(slot_point slot_tx);

  /// Returns the PUCCH resource to be used for HARQ-ACK (format 1).
  /// \remark This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  /// \return If any PUCCH resource available, it returns (i) the pointer to the configuration and (ii) the PUCCH
  /// resource indicator corresponding to the PUCCH resource that will be used by the UE. If there are no PUCCH
  /// resources available, the pointer passed will be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  pucch_harq_resource_alloc_record
  get_next_harq_res_available(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// Returns the PUCCH format 2 resource to be used (SR / HARQ-ACK / CSI).
  /// \remark This index refers to the \c pucch-ResourceId of the \c PUCCH-Resource, as per TS 38.331.
  /// \return If any PUCCH resource available, it returns (i) the pointer to the configuration and (ii) the PUCCH
  /// resource indicator corresponding to the PUCCH resource that will be used by the UE. If there are no PUCCH
  /// resources available, the pointer passed will be \c nullptr, whereas the PUCCH resource indicator is to be ignored.
  pucch_harq_resource_alloc_record
  get_next_format2_res_available(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// Returns the pointer to the configuration of the PUCCH resource to be used for SR.
  /// \remark There is only one resource used for SR.
  /// \return the pointer to the configuration of the PUCCH resource to be used for SR, if available; else, it returns
  /// \c nullptr.
  const pucch_resource* get_next_sr_res_available(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 1) resource from being allocated to a given UE.
  /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_harq_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 2) resource from being allocated to a given UE.
  /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_format2_resource(slot_point slot_harq, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Release PUCCH (format 1) resource used for SR from being allocated to a given UE.
  /// \param[in] slot_harq slot for which the PUCCH resource was scheduled.
  /// \param[in] crnti UE from which the resource needs to be released.
  /// \param[in] pucch_cfg UE's PUCCH config.
  /// \return True if the resource for the UE was found in the allocation records for the given slot.
  bool release_sr_resource(slot_point slot_sr, rnti_t crnti, const pucch_config& pucch_cfg);

  /// \brief Returns the PUCCH resource indicator (format 1) of the resource used for a given RNTI at a given slot.
  /// \return PUCCH resource indicator of the resource used allocated to the UE; if UE is not found, returns -1.
  int get_f1_pucch_res_indic(slot_point slot_tx, rnti_t crnti);

  /// \brief Returns the PUCCH resource indicator (format 2) of the resource used for a given RNTI at a given slot.
  /// \return PUCCH resource indicator of the resource used allocated to the UE; if UE is not found, returns -1.
  int get_f2_pucch_res_indic(slot_point slot_tx, rnti_t crnti);

private:
  /// Size of the ring buffer of pucch_resource_manager. This size sets a limit on how far in advance a PUCCH can be
  /// allocated.
  static const size_t RES_MANAGER_RING_BUFFER_SIZE =
      get_allocator_ring_size_gt_min(SCHEDULER_MAX_K0 + SCHEDULER_MAX_K1);

  // [Implementation-defined] Number of PUCCH resources (of single format) that can be handled by the resource manager.
  // NOTE: this number allows us to have a 1-to-1 match between PUCCH resource indicator and index of the PUCCH resource
  // in its corresponding PUCCH resource set.
  static const size_t MAX_HARQ_PUCCH_RESOURCES{8};

  using pucch_res_record_array = std::array<rnti_t, MAX_HARQ_PUCCH_RESOURCES>;

  // Record for the RNTI and PUCCH resource indicator used for a given resource at a given slot.
  struct rnti_pucch_res_id_slot_record {
    // Keep track of the UE allocated to the SR resource.
    rnti_t ue_using_sr_resource{INVALID_RNTI};
    // Keeps track of the RNTI of the UE using a given PUCCH resource format 1 (indexed by the PUCCH res. indicator).
    pucch_res_record_array ues_using_format1_res;
    // Keeps track of the RNTI of the UE using a given PUCCH resource format 2 (indexed by the PUCCH res. indicator).
    pucch_res_record_array ues_using_format2_res;
  };

  // Returns the resource manager allocation record for a given slot.
  rnti_pucch_res_id_slot_record& get_slot_resource_counter(slot_point sl);

  // Ring buffer of rnti_pucch_res_id_slot_record for PUCCH resources.
  std::array<rnti_pucch_res_id_slot_record, RES_MANAGER_RING_BUFFER_SIZE> resource_slots;

  // Keeps track of the last slot_point used by the resource manager.
  slot_point last_sl_ind;
};

/// Implementation of the PUCCH allocator interface.
class pucch_allocator_impl final : public pucch_allocator
{
public:
  explicit pucch_allocator_impl(const cell_configuration& cell_cfg_);

  ~pucch_allocator_impl() override;

  pucch_harq_ack_grant alloc_common_pucch_harq_ack_ue(cell_resource_allocator&    res_alloc,
                                                      rnti_t                      tcrnti,
                                                      unsigned                    pdsch_time_domain_resource,
                                                      unsigned                    k1,
                                                      const pdcch_dl_information& dci_info) override;

  void pucch_allocate_sr_opportunity(cell_slot_resource_allocator& slot_alloc,
                                     rnti_t                        crnti,
                                     const ue_cell_configuration&  ue_cell_cfg) override;

  pucch_harq_ack_grant alloc_ded_pucch_harq_ack_ue(cell_resource_allocator&     res_alloc,
                                                   rnti_t                       crnti,
                                                   const ue_cell_configuration& ue_cell_cfg,
                                                   unsigned                     pdsch_time_domain_resource,
                                                   unsigned                     k1) override;

  pucch_uci_bits remove_ue_uci_from_pucch(cell_slot_resource_allocator& slot_alloc, rnti_t crnti) override;

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
  void allocate_pucch_ded_res_on_grid(cell_slot_resource_allocator& pucch_slot_alloc,
                                      const pucch_resource&         pucch_res,
                                      unsigned                      nof_prbs);

  // Helper that allocates a NEW PUCCH HARQ grant.
  pucch_harq_ack_grant allocate_new_pucch_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                     rnti_t                        crnti,
                                                     const ue_cell_configuration&  ue_cell_cfg,
                                                     pucch_info*                   existing_sr_grant);

  // Helper that updates an existing PUCCH HARQ grant.
  pucch_harq_ack_grant update_existing_pucch_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                        pucch_info&                   existing_harq_grant,
                                                        pucch_info*                   existing_sr_grant,
                                                        rnti_t                        rnti,
                                                        slot_point                    sl_tx,
                                                        const ue_cell_configuration&  ue_cell_cfg);

  pucch_harq_ack_grant convert_to_format2(cell_slot_resource_allocator& pucch_slot_alloc,
                                          pucch_info&                   existing_harq_grant,
                                          pucch_info*                   existing_sr_grant,
                                          rnti_t                        rnti,
                                          slot_point                    sl_tx,
                                          const ue_cell_configuration&  ue_cell_cfg);

  void remove_pucch_format1_from_grants(cell_slot_resource_allocator& slot_alloc,
                                        rnti_t                        crnti,
                                        const pucch_config&           pucch_cfg);

  // Fills the PUCCH SR grant.
  void fill_pucch_ded_res_grant(pucch_info&           pucch_sr_grant,
                                rnti_t                crnti,
                                const pucch_resource& pucch_ded_res_cfg,
                                unsigned              harq_ack_bits,
                                sr_nof_bits           sr_bits);

  const unsigned            PUCCH_FORMAT_1_NOF_PRBS{1};
  const cell_configuration& cell_cfg;
  slot_point                last_sl_ind;
  pucch_resource_manager    resource_manager;

  srslog::basic_logger& logger;
};

} // namespace srsgnb
