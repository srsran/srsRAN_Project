/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#pragma once
#include "../cell/resource_grid.h"
#include "../config/ue_configuration.h"
#include "pucch_allocator.h"
#include "pucch_resource_manager.h"
#include "srsran/ran/pucch/pucch_uci_bits.h"
#include "srsran/scheduler/result/sched_result.h"
#include "srsran/srslog/logger.h"

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

  /// Called on cell deactivation.
  void stop();

  std::optional<unsigned> alloc_common_harq_ack(cell_resource_allocator&    res_alloc,
                                                rnti_t                      tcrnti,
                                                unsigned                    k0,
                                                unsigned                    k1,
                                                const pdcch_dl_information& dci_info) override;

  std::optional<unsigned> alloc_common_and_ded_harq_ack(cell_resource_allocator&     res_alloc,
                                                        rnti_t                       rnti,
                                                        const ue_cell_configuration& ue_cell_cfg,
                                                        unsigned                     k0,
                                                        unsigned                     k1,
                                                        const pdcch_dl_information&  dci_info) override;

  std::optional<unsigned> alloc_ded_harq_ack(cell_resource_allocator&     res_alloc,
                                             rnti_t                       crnti,
                                             const ue_cell_configuration& ue_cell_cfg,
                                             unsigned                     k0,
                                             unsigned                     k1) override;

  void alloc_sr_opportunity(cell_slot_resource_allocator& slot_alloc,
                            rnti_t                        crnti,
                            const ue_cell_configuration&  ue_cell_cfg) override;

  void alloc_csi_opportunity(cell_slot_resource_allocator& pucch_slot_alloc,
                             rnti_t                        crnti,
                             const ue_cell_configuration&  ue_cell_cfg,
                             unsigned                      csi_part1_nof_bits) override;

  pucch_uci_bits remove_ue_uci_from_pucch(cell_slot_resource_allocator& slot_alloc,
                                          rnti_t                        crnti,
                                          const ue_cell_configuration&  ue_cell_cfg) override;

  [[nodiscard]] bool has_common_pucch_grant(rnti_t rnti, slot_point sl_tx) const override;

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

  struct pucch_common_params {
    unsigned pucch_res_indicator;
    unsigned r_pucch;
  };

  struct harq_res_id {
    pucch_res_set_idx pucch_set_idx = pucch_res_set_idx::set_0;
    uint8_t           pucch_res_ind = 0;
  };

  /// \brief Defines the type of PUCCH resource.
  /// - harq_ack indicates the HAR-ACK resource (it can carry HARQ-ACK and/or SR and/or CSI bits).
  /// - sr indicates the resource dedicated for SR (it can carry SR and HARQ-ACK bits).
  /// - csi indicates the resource dedicated for CSI (it can carry CSI and SR bits).
  enum class pucch_grant_type { harq_ack, sr, csi };

  /// Converts a pucch_grant_type to string.
  static const char* to_string(pucch_grant_type type)
  {
    switch (type) {
      case pucch_grant_type::harq_ack:
        return "HARQ-ACK";
      case pucch_grant_type::sr:
        return "SR";
      case pucch_grant_type::csi:
        return "CSI";
      default:
        return "unknown";
    }
  }

  /// \brief Defines a PUCCH grant (and its relevant information) currently allocated to a given UE.
  /// It is used internally to keep track of the UEs' allocations of the PUCCH grants with dedicated resources.
  struct pucch_grant {
    pucch_grant_type type;
    pucch_format     format = pucch_format::NOF_FORMATS;
    // Only relevant for HARQ-ACK resources.
    harq_res_id       harq_id;
    pucch_uci_bits    bits;
    ofdm_symbol_range symbols = {NOF_OFDM_SYM_PER_SLOT_NORMAL_CP + 1, NOF_OFDM_SYM_PER_SLOT_NORMAL_CP + 1};
    // NOTE: The pointer to the PUCCH resource configuration can only be used within the same slot; this is to prevent
    // the possibility that the re-configurations can null the pointer before an allocation and the next.
    const pucch_resource* pucch_res_cfg = nullptr;

    // Set format and symbols given the configuration.
    void set_res_config(const pucch_resource& res_cfg);
  };

  /// \brief List of possible PUCCH grants that allocated to a UE, at a given slot.
  class pucch_grant_list
  {
  public:
    std::optional<pucch_grant> harq_resource;
    std::optional<pucch_grant> sr_resource;
    std::optional<pucch_grant> csi_resource;

    [[nodiscard]] pucch_uci_bits get_uci_bits() const;
    [[nodiscard]] bool           is_empty() const;
    [[nodiscard]] unsigned       get_nof_grants() const;
  };

  /// Keeps track of the PUCCH grants (common + dedicated) for a given UE.
  struct ue_grants {
    rnti_t rnti;
    // Information about the common PUCCH grant.
    bool has_common_pucch = false;
    // List of PUCCH grants with dedicated resources.
    pucch_grant_list pucch_grants;
  };

  /// Keeps track of the PUCCH allocation context for a given slot.
  struct slot_context {
    static_vector<ue_grants, MAX_PUCCH_PDUS_PER_SLOT> ue_grants_list;

    /// Clears the slot context.
    void clear() { ue_grants_list.clear(); }

    /// Finds the UE grants for a given RNTI.
    [[nodiscard]] ue_grants* find_ue_grants(rnti_t rnti)
    {
      auto* it = std::find_if(ue_grants_list.begin(), ue_grants_list.end(), [rnti](const ue_grants& grants) {
        return grants.rnti == rnti;
      });
      return it != ue_grants_list.end() ? it : nullptr;
    }
  };

  /// \brief Context information for a PUCCH allocation attempt.
  struct alloc_context;

  ///////////////  Main private functions   //////////////

  // Allocates the PUCCH (common) resource for HARQ-(N)-ACK.
  std::optional<pucch_res_alloc_cfg> alloc_pucch_common_res_harq(const cell_slot_resource_allocator& pucch_alloc,
                                                                 const dci_context_information&      dci_info);

  void compute_pucch_common_params_and_alloc(cell_slot_resource_allocator& pucch_alloc,
                                             rnti_t                        rnti,
                                             pucch_common_params           pucch_params);

  std::optional<pucch_common_params> find_common_and_ded_harq_res_available(cell_slot_resource_allocator& pucch_alloc,
                                                                            ue_grants&                   current_grants,
                                                                            const ue_cell_configuration& ue_cell_cfg,
                                                                            const dci_context_information& dci_info,
                                                                            const alloc_context&           alloc_ctx);

  // Helper that allocates a NEW PUCCH HARQ grant (Format 0 or 1).
  std::optional<unsigned> allocate_harq_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                                              const ue_cell_configuration&  ue_cell_cfg,
                                              const alloc_context&          alloc_ctx);

  // Helper that allocates a new PUCCH HARQ grant (Format 2/3/4) for CSI.
  void allocate_csi_grant(cell_slot_resource_allocator& pucch_slot_alloc,
                          const ue_cell_configuration&  ue_cell_cfg,
                          unsigned                      csi_part1_bits,
                          const alloc_context&          alloc_ctx);

  // Update the grants data for the case in which multiplexing is not needed.
  std::optional<unsigned> allocate_without_multiplexing(cell_slot_resource_allocator& pucch_slot_alloc,
                                                        const pucch_uci_bits&         new_bits,
                                                        ue_grants&                    current_grants,
                                                        const ue_cell_configuration&  ue_cell_cfg,
                                                        const alloc_context&          alloc_ctx);

  // Implements the main steps of the multiplexing procedure as defined in TS 38.213, Section 9.2.5.
  // Note: If \c common_grants is set, it means that the function is called while allocating common and dedicated
  // resources together. In this case, the allocation will fail if the multiplexed resources collide with it or with any
  // other UL grant.
  std::optional<unsigned>
  multiplex_and_allocate_pucch(cell_slot_resource_allocator&                    pucch_slot_alloc,
                               const pucch_uci_bits&                            new_bits,
                               ue_grants&                                       current_grants,
                               const ue_cell_configuration&                     ue_cell_cfg,
                               std::optional<uint8_t>                           preserve_res_indicator,
                               const alloc_context&                             alloc_ctx,
                               std::optional<std::pair<grant_info, grant_info>> common_grants = std::nullopt);

  // Computes which resources are expected to be sent, depending on the UCI bits to be sent, before any multiplexing.
  std::optional<pucch_grant_list> get_pucch_res_pre_multiplexing(pucch_resource_manager::ue_reservation_guard& guard,
                                                                 const ue_grants&      ue_current_grants,
                                                                 const pucch_uci_bits& new_bits);

  // Execute the multiplexing algorithm as defined in TS 38.213, Section 9.2.5.
  pucch_grant_list multiplex_resources(pucch_resource_manager::ue_reservation_guard& guard,
                                       const pucch_grant_list&                       candidate_grants,
                                       std::optional<uint8_t>                        preserve_res_indicator);

  // Applies the multiplexing rules depending on the PUCCH resource format, as per TS 38.213, Section 9.2.5.1/2.
  std::optional<pucch_grant> merge_pucch_resources(pucch_resource_manager::ue_reservation_guard& guard,
                                                   span<const pucch_grant>                       resources_to_merge,
                                                   std::optional<uint8_t> preserve_res_indicator);

  // Allocate the PUCCH PDUs in the scheduler output, depending on the new PUCCH grants to be transmitted, and depending
  // on the PUCCH PDUs currently allocated.
  std::optional<unsigned> allocate_grants(pucch_resource_manager::ue_reservation_guard& guard,
                                          cell_slot_resource_allocator&                 pucch_slot_alloc,
                                          ue_grants&                                    existing_pucchs,
                                          const pucch_grant_list&                       grants_to_tx,
                                          const alloc_context&                          alloc_ctx);

  ///////////////  Private helpers   ///////////////

  /// Returns whether a given UE can be allocated PUCCH in a given slot.
  bool can_allocate_pucch(const cell_slot_resource_allocator& pucch_slot_alloc,
                          const ue_grants*                    existing_ue_grants,
                          const alloc_context&                alloc_ctx) const;

  /// Returns whether a given fallback UE can be allocated PUCCH in a given slot.
  bool can_allocate_fallback_pucch(const cell_slot_resource_allocator& pucch_slot_alloc,
                                   const ue_grants*                    existing_ue_grants,
                                   const alloc_context&                alloc_ctx) const;

  /// Returns whether there is space for new PUCCH grants in the given scheduler result.
  bool is_there_space_for_new_pucch_grants(const sched_result& slot_result, unsigned nof_grants_to_allocate) const;

  /// \brief Fills the PUCCH PDU for common HARQ-ACK resources.
  /// \param[out] pucch_pdu PUCCH PDU to be filled.
  /// \param[in] pucch_res PUCCH resource configuration.
  /// \param[in] rnti RNTI of the UE.
  void fill_common_pdu(pucch_info& pucch_pdu, const pucch_res_alloc_cfg& pucch_res, rnti_t rnti) const;

  /// \brief Fills the PUCCH PDU for dedicated resources.
  // \param[out] pucch_pdu PUCCH PDU to be filled.
  // \param[in] pucch_res PUCCH resource configuration.
  // \param[in] uci_bits UCI bits to be sent in the PUCCH.
  // \param[in] ue_cell_cfg UE cell configuration.
  // \param[in] rnti RNTI of the UE.
  // \param[in] adjust_prbs If true, adjusts the number of PRBs based on the number of UCI bits to be carried.
  //            Only applicable for PUCCH resources of Formats 2 or 3.
  void fill_ded_pdu(pucch_info&                  pucch_pdu,
                    const pucch_resource&        pucch_res,
                    const pucch_uci_bits&        uci_bits,
                    const ue_cell_configuration& ue_cell_cfg,
                    rnti_t                       rnti,
                    bool                         adjust_prbs) const;

  void remove_unused_pucch_res(pucch_resource_manager::ue_reservation_guard& guard,
                               const ue_grants&                              existing_pucchs,
                               const pucch_grant_list&                       grants_to_tx);

  // \brief Ring of PUCCH allocations indexed by slot.
  circular_array<slot_context, cell_resource_allocator::RING_ALLOCATOR_SIZE> slots_ctx;

  const cell_configuration& cell_cfg;
  const unsigned            max_pucch_grants_per_slot;
  const unsigned            max_ul_grants_per_slot;
  slot_point                last_sl_ind;
  pucch_resource_manager    resource_manager;

  srslog::basic_logger& logger;
};

} // namespace srsran
