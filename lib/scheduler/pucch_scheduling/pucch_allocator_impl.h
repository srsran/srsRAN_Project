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

#include "../cell/resource_grid.h"
#include "../config/ue_configuration.h"
#include "pucch_allocator.h"
#include "pucch_resource_manager.h"
#include "srsran/scheduler/scheduler_dci.h"
#include <variant>

namespace srsran {

/// Implementation of the PUCCH allocator interface.
class pucch_allocator_impl final : public pucch_allocator
{
public:
  explicit pucch_allocator_impl(const cell_configuration& cell_cfg_,
                                unsigned                  max_pucchs_per_slot,
                                unsigned                  max_ul_grants_per_slot_);

  ~pucch_allocator_impl() override;

  /// Updates the internal slot_point and tracking of PUCCH resource usage; and resets the PUCCH common allocation grid.
  void slot_indication(slot_point sl_tx) override;

  std::optional<unsigned> alloc_common_pucch_harq_ack_ue(cell_resource_allocator&    res_alloc,
                                                         rnti_t                      tcrnti,
                                                         unsigned                    k0,
                                                         unsigned                    k1,
                                                         const pdcch_dl_information& dci_info) override;

  std::optional<unsigned> alloc_common_and_ded_harq_res(cell_resource_allocator&     res_alloc,
                                                        rnti_t                       rnti,
                                                        const ue_cell_configuration& ue_cell_cfg,
                                                        unsigned                     k0,
                                                        unsigned                     k1,
                                                        const pdcch_dl_information&  dci_info) override;

  std::optional<unsigned> alloc_ded_pucch_harq_ack_ue(cell_resource_allocator&     res_alloc,
                                                      rnti_t                       crnti,
                                                      const ue_cell_configuration& ue_cell_cfg,
                                                      unsigned                     k0,
                                                      unsigned                     k1) override;

  void pucch_allocate_sr_opportunity(cell_slot_resource_allocator& slot_alloc,
                                     rnti_t                        crnti,
                                     const ue_cell_configuration&  ue_cell_cfg) override;

  void pucch_allocate_csi_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                                      rnti_t                        crnti,
                                      const ue_cell_configuration&  ue_cell_cfg,
                                      unsigned                      csi_part1_nof_bits) override;

  pucch_uci_bits remove_ue_uci_from_pucch(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        crnti,
                                          const ue_cell_configuration&  ue_cell_cfg) override;

  bool has_common_pucch_f1_grant(rnti_t rnti, slot_point sl_tx) const override;

private:
  /// ////////////  Helper struct and classes   //////////////

  // Structs with the info about the PUCCH resources.
  struct pucch_res_alloc_cfg {
    // True if the struct has a valid config.
    unsigned   pucch_res_indicator;
    grant_info first_hop_res;
    // Contains grant only if intra-slot freq-hopping is active.
    grant_info second_hop_res;
    // Cyclic-shift.
    unsigned cs;
    // PUCCH format.
    pucch_format format;
  };

  // Contains the existing PUCCH grants currently allocated to a given UE.
  struct existing_pucch_grants {
    pucch_info* format1_sr_grant{nullptr};
    pucch_info* format1_harq_grant{nullptr};
    pucch_info* format1_harq_common_grant{nullptr};
    pucch_info* format2_grant{nullptr};
  };

  struct pucch_com_ded_res {
    pucch_res_alloc_cfg   pucch_common_info;
    const pucch_resource& pucch_ded_cfg;
  };

  struct uci_bits {
    unsigned    harq_ack_bits  = 0U;
    sr_nof_bits sr_bits        = sr_nof_bits::no_sr;
    unsigned    csi_part1_bits = 0U;

    unsigned get_total_bits() const { return harq_ack_bits + sr_nof_bits_to_uint(sr_bits) + csi_part1_bits; }
  };

  // At the moment, we only supports PUCCH resource set index 0 and 1.
  enum class pucch_res_set_idx : uint8_t { set_0 = 0, set_1 };

  struct harq_res_id {
    pucch_res_set_idx pucch_set_idx = pucch_res_set_idx::set_0;
    uint8_t           pucch_res_ind = 0;
  };

  enum class pucch_grant_type { harq_ack, sr, csi };

  class pucch_grant
  {
  public:
    pucch_grant_type type;
    // Only relevant for HARQ-ACK resources.
    harq_res_id           harq_id;
    pucch_format          format;
    uci_bits              bits;
    const pucch_resource* pucch_res_cfg = nullptr;

    ofdm_symbol_range get_symbols() const;
  };

  class pucch_grant_list
  {
  public:
    std::optional<pucch_grant> harq_resource;
    std::optional<pucch_grant> sr_resource;
    std::optional<pucch_grant> csi_resource;
    unsigned                   nof_grants = 0;

    uci_bits get_uci_bits() const;
  };

  using resource_set_q_t = std::vector<pucch_allocator_impl::pucch_grant>;

  struct ue_pucch_bits {
    rnti_t           rnti;
    pucch_grant_list pucch_grants;
  };

  using slot_pucch_grants = static_vector<ue_pucch_bits, MAX_PUCCH_PDUS_PER_SLOT>;

  class res_manager_garbage_collector
  {
  public:
    res_manager_garbage_collector(pucch_resource_manager& res_manager_) : res_manager(res_manager_){};

    bool                    harq_set_0 = false;
    bool                    harq_set_1 = false;
    bool                    csi        = false;
    bool                    sr         = false;
    pucch_resource_manager& res_manager;

    void reset();
    void release_resource(slot_point slot_tx, rnti_t crnti, const ue_cell_configuration& ue_cell_cfg);
  };

  /// ////////////  Main private functions   //////////////

  // Allocates the PUCCH (common) resource for HARQ-(N)-ACK.
  std::optional<pucch_res_alloc_cfg> alloc_pucch_common_res_harq(cell_slot_resource_allocator&  pucch_alloc,
                                                                 const dci_context_information& dci_info);

  std::optional<unsigned> exec_common_and_ded_res_alloc(cell_slot_resource_allocator& pucch_alloc,
                                                        pucch_info*                   existing_grant,
                                                        rnti_t                        rnti,
                                                        const ue_cell_configuration&  ue_cell_cfg,
                                                        pucch_res_alloc_cfg           common_res_cfg,
                                                        const pucch_resource&         ded_res_cfg);

  // Helper that allocates a NEW PUCCH HARQ grant (Format 1).
  std::optional<unsigned> allocate_new_format1_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                                          rnti_t                        crnti,
                                                          const ue_cell_configuration&  ue_cell_cfg,
                                                          pucch_info*                   existing_sr_grant);

  // Helper that allocates a NEW PUCCH HARQ grant (Format 1).
  std::optional<unsigned> allocate_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                              rnti_t                        crnti,
                                              const ue_cell_configuration&  ue_cell_cfg);

  // Helper that add an HARQ-ACK bit to existing PUCCH HARQ grant (Format 1).
  std::optional<unsigned> add_harq_ack_bit_to_format1_grant(pucch_info&         existing_harq_grant,
                                                            pucch_info*         existing_sr_grant,
                                                            rnti_t              rnti,
                                                            slot_point          sl_tx,
                                                            const pucch_config& pucch_cfg);

  // Helper that allocates a new PUCCH HARQ grant (Format 2) for CSI.
  void allocate_csi_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                          rnti_t                        crnti,
                          const ue_cell_configuration&  ue_cell_cfg,
                          unsigned                      csi_part1_bits);

  // Helper that replaces PUCCH grant Format 1 with Format 2 grant for HARQ-ACK reporting.
  std::optional<unsigned> convert_to_format2_harq(cell_slot_resource_allocator& pucch_slot_alloc,
                                                  pucch_info&                   existing_harq_grant,
                                                  pucch_info*                   existing_sr_grant,
                                                  rnti_t                        rnti,
                                                  const ue_cell_configuration&  ue_cell_cfg,
                                                  unsigned                      harq_ack_bits_increment);

  // Helper that changes the current PUCCH Format 2 grant (specifically used for CSI reporting) into a PUCCH Format 2
  // resource for the HARQ-ACK + CSI.
  std::optional<unsigned> change_format2_resource(cell_slot_resource_allocator& pucch_slot_alloc,
                                                  pucch_info&                   existing_grant,
                                                  rnti_t                        rnti,
                                                  const ue_cell_configuration&  ue_cell_cfg,
                                                  unsigned                      harq_ack_bits_increment,
                                                  std::optional<pucch_harq_resource_alloc_record> harq_f2_res);

  // Helper that adds HARQ-ACK bits to a PUCCH Format 2 grant for HARQ-ACK.
  std::optional<unsigned> add_harq_bits_to_harq_f2_grant(pucch_info&                  existing_f2_grant,
                                                         slot_point                   sl_tx,
                                                         rnti_t                       crnti,
                                                         const ue_cell_configuration& ue_cell_cfg,
                                                         unsigned                     harq_ack_bits_increment);

  std::optional<pucch_com_ded_res> find_common_and_ded_harq_res_available(cell_slot_resource_allocator&  pucch_alloc,
                                                                          pucch_info*                    existing_grant,
                                                                          rnti_t                         rnti,
                                                                          const ue_cell_configuration&   ue_cell_cfg,
                                                                          const dci_context_information& dci_info);

  // Helper that removes the existing PUCCH Format 1 grants (both HARQ-ACK and SR).
  void remove_pucch_format1_from_grants(cell_slot_resource_allocator& slot_alloc,
                                        rnti_t                        crnti,
                                        const pucch_config&           pucch_cfg);

  // Helper that removes the existing PUCCH Format 2 grant for CSI.
  void remove_format2_csi_from_grants(cell_slot_resource_allocator& slot_alloc,
                                      rnti_t                        crnti,
                                      const ue_cell_configuration&  ue_cell_cfg);

  // Fills the PUCCH HARQ grant for common resources.
  void fill_pucch_harq_common_grant(pucch_info& pucch_info, rnti_t rnti, const pucch_res_alloc_cfg& pucch_res);

  // Fills the PUCCH Format 1 grant.
  void fill_pucch_ded_format1_grant(pucch_info&           pucch_grant,
                                    rnti_t                crnti,
                                    const pucch_resource& pucch_ded_res_cfg,
                                    unsigned              harq_ack_bits,
                                    sr_nof_bits           sr_bits);

  // Fills the PUCCH Format 2 grant.
  void fill_pucch_format2_grant(pucch_info&                  pucch_grant,
                                rnti_t                       crnti,
                                const pucch_resource&        pucch_ded_res_cfg,
                                const ue_cell_configuration& ue_cell_cfg,
                                unsigned                     nof_prbs,
                                unsigned                     harq_ack_bits,
                                sr_nof_bits                  sr_bits,
                                unsigned                     csi_part1_bits);

  std::optional<unsigned> alloc_ded_pucch_harq_ack_ue_1(cell_resource_allocator&     res_alloc,
                                                        rnti_t                       crnti,
                                                        const ue_cell_configuration& ue_cell_cfg,
                                                        unsigned                     k0,
                                                        unsigned                     k1);

  std::optional<unsigned> multiplex_and_allocate_pucch(cell_slot_resource_allocator& pucch_slot_alloc,
                                                       uci_bits                      new_bits,
                                                       ue_pucch_bits&                current_grants,
                                                       const ue_cell_configuration&  ue_cell_cfg);

  std::optional<pucch_grant_list> get_pucch_res_pre_multiplexing(slot_point                   sl_tx,
                                                                 uci_bits                     new_bits,
                                                                 ue_pucch_bits                ue_current_grants,
                                                                 const ue_cell_configuration& ue_cell_cfg);

  pucch_grant_list multiplex_resources(slot_point                   sl_tx,
                                       rnti_t                       crnti,
                                       pucch_grant_list             candidate_grants,
                                       const ue_cell_configuration& ue_cell_cfg);

  std::optional<pucch_grant> merge_pucch_resources(span<const pucch_grant> resources_to_merge,
                                                   slot_point              slot_harq,
                                                   rnti_t                  crnti,
                                                   const pucch_config&     pucch_cfg);

  std::optional<unsigned> allocate_grants(cell_slot_resource_allocator& pucch_slot_alloc,
                                          ue_pucch_bits&                existing_pucchs,
                                          rnti_t                        crnti,
                                          pucch_grant_list              grants_to_tx,
                                          const ue_cell_configuration&  ue_cell_cfg);

  /// ////////////  Private helpers   //////////////

  void remove_unsed_pucch_res(slot_point                   sl_tx,
                              pucch_grant_list             grants_to_tx,
                              ue_pucch_bits&               existing_pucchs,
                              const ue_cell_configuration& ue_cell_cfg);

  // Helper that retrieves the existing grants allocated to a given UE for a given slot.
  existing_pucch_grants
  get_existing_pucch_grants(static_vector<pucch_info, MAX_PUCCH_PDUS_PER_SLOT>& pucchs, rnti_t rnti, slot_point sl_ack);

  unsigned get_max_pucch_grants(unsigned currently_allocated_puschs);

  // \brief Ring of PUCCH allocations indexed by slot.
  circular_array<slot_pucch_grants, cell_resource_allocator::RING_ALLOCATOR_SIZE> pucch_grants_alloc_grid;

  using slot_alloc_list = static_vector<rnti_t, MAX_PUCCH_PDUS_PER_SLOT>;

  // \brief Ring of PUCCH allocations indexed by slot.
  circular_array<slot_alloc_list, cell_resource_allocator::RING_ALLOCATOR_SIZE> pucch_common_alloc_grid;

  const unsigned                PUCCH_FORMAT_1_NOF_PRBS{1};
  const cell_configuration&     cell_cfg;
  const unsigned                max_pucch_grants_per_slot;
  const unsigned                max_ul_grants_per_slot;
  slot_point                    last_sl_ind;
  pucch_resource_manager        resource_manager;
  res_manager_garbage_collector garbage_collector;

  srslog::basic_logger& logger;
};

} // namespace srsran
