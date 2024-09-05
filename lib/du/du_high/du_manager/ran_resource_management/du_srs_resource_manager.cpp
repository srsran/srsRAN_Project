/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_srs_resource_manager.h"
#include "du_ue_resource_config.h"
#include "srsran/ran/srs/srs_bandwidth_configuration.h"

using namespace srsran;
using namespace srs_du;

static std::optional<unsigned> compute_srs_bw_param(unsigned nof_ul_rbs)
{
  // Iterate over Table 6.4.1.4.3-1, TS 38.211, and find the first \f$C_{SRS}\f$ value that provides the
  // greatest \f$m_{SRS,0}\f$ such that it is less than or equal to number of UL RBs.

  // As per Table 6.4.1.4.3-1, TS 38.211, the maximum value of \f$C_{SRS}\f$ is 63.
  constexpr unsigned max_non_valid_c_srs = 64;
  constexpr uint8_t  b_srs_0             = 0;
  for (uint8_t c_srs = 0; c_srs != max_non_valid_c_srs; c_srs++) {
    auto srs_params = srs_configuration_get(c_srs, b_srs_0);
    srsran_assert(srs_params.has_value(), "The SRS is not valid");

    if (c_srs == 0 and srs_params.value().m_srs > nof_ul_rbs) {
      srsran_assertion_failure("C_SRS is not compatible with the number of UL RBs");
      return std::nullopt;
    }
    if (srs_params.value().m_srs == nof_ul_rbs) {
      return c_srs;
    }
    if (srs_params.value().m_srs > nof_ul_rbs) {
      return c_srs - 1;
    }
  }
  return max_non_valid_c_srs - 1;
}

// Helper that returns the frequency shift value for the SRS; the value is computed in such a way that the SRS resources
// are placed at the center of the band.
static unsigned compute_freq_shift(unsigned c_srs, unsigned nof_ul_rbs)
{
  // As per Section 6.4.1.4.3, the parameter \f$C_{SRS}\f$ = 0 provides the bandwidth of the SRS resources.
  constexpr uint8_t                        b_srs_0    = 0;
  std::optional<srsran::srs_configuration> srs_params = srs_configuration_get(c_srs, b_srs_0);
  srsran_sanity_check(srs_params.has_value() and nof_ul_rbs >= srs_params.value().m_srs,
                      "The SRS configuration is not valid");

  return (nof_ul_rbs - srs_params.value().m_srs) / 2;
}

static bool is_ul_slot(unsigned offset, const tdd_ul_dl_config_common& tdd_cfg)
{
  const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
  return srsran::get_active_tdd_ul_symbols(tdd_cfg, slot_index, cyclic_prefix::NORMAL).length() != 0;
}

static bool is_partually_ul_slot(unsigned offset, const tdd_ul_dl_config_common& tdd_cfg)
{
  const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
  return srsran::get_active_tdd_ul_symbols(tdd_cfg, slot_index, cyclic_prefix::NORMAL).length() != 0 and
         srsran::get_active_tdd_dl_symbols(tdd_cfg, slot_index, cyclic_prefix::NORMAL).length() !=
             NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
}

static bool srs_config_validator(const du_cell_config& cell_cfg)
{
  if (not cell_cfg.ue_ded_serv_cell_cfg.ul_config.has_value()) {
    return false;
  }

  if (cell_cfg.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.has_value()) {
    return false;
  }

  const auto& srs_cfg = cell_cfg.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.value();
  if (srs_cfg.srs_res_set_list.size() != 1) {
    return false;
  }

  if (not std::holds_alternative<srs_config::srs_resource_set::periodic_resource_type>(
          srs_cfg.srs_res_set_list.front().res_type)) {
    return false;
  }

  if (srs_cfg.srs_res_set_list.front().srs_res_id_list.size() != 1 or
      srs_cfg.srs_res_set_list.front().srs_res_id_list.front() != static_cast<srs_config::srs_res_id>(0U)) {
    return false;
  }

  if (srs_cfg.srs_res_list.size() != 1 or
      srs_cfg.srs_res_list.front().id.ue_res_id != static_cast<srs_config::srs_res_id>(0U)) {
    return false;
  }

  if (srs_cfg.srs_res_list.front().res_type != srs_resource_type::periodic) {
    return false;
  }

  if (not srs_cfg.srs_res_list.front().periodicity_and_offset.has_value()) {
    return false;
  }

  return true;
}

du_srs_policy_max_ul_th::du_srs_policy_max_ul_th(span<const du_cell_config> cell_cfg_list_) :
  cells(cell_cfg_list_.begin(), cell_cfg_list_.end())
{
  for (auto& cell : cells) {
    if (not cell.cell_cfg.srs_cfg.srs_period.has_value()) {
      continue;
    }

    srsran_assert(srs_config_validator(cell.cell_cfg), "The SRS configuration is not valid");
    std::optional<unsigned> c_srs =
        compute_srs_bw_param(cell.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length()).value();
    srsran_assert(c_srs.has_value(), "SRS parameters didn't provide a valid C_SRS value");
    cell.srs_common_params.c_srs = c_srs.value();
    std::optional<unsigned> freq_shift =
        compute_freq_shift(c_srs.value(), cell.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length());
    srsran_assert(freq_shift.has_value(), "SRS parameters didn't provide a valid freq_shift value");
    cell.srs_common_params.freq_shift = freq_shift.value();

    // TODO: evaluate whether we need to consider the case of multiple cells.
    cell.cell_srs_res_list = generate_cell_srs_list(cell.cell_cfg);

    const unsigned srs_period_slots = static_cast<unsigned>(cell.cell_cfg.srs_cfg.srs_period.value());
    cell.slot_resource_cnt.reserve(srs_period_slots);

    for (unsigned offset = 0; offset != srs_period_slots; ++offset) {
      unsigned offset_res_cnt = 0U;
      for (auto& res : cell.cell_srs_res_list) {
        // Handle TDD and FDD configurations separately, as we treat partially-UL slots differently from fully-UL slots.
        if (cell_cfg_list_[0].tdd_ul_dl_cfg_common.has_value()) {
          // Verify whether the offset maps to a partially- or fully-UL slot.
          if (not is_ul_slot(offset, cell_cfg_list_[0].tdd_ul_dl_cfg_common.value())) {
            continue;
          }
          // For partially-UL slots, we need to check if the SRS can be placed in the UL symbols of the slot.
          if (is_partually_ul_slot(offset, cell_cfg_list_[0].tdd_ul_dl_cfg_common.value())) {
            // TODO: Fix check for pattern 2.
            if (res.symbols.start() < NOF_OFDM_SYM_PER_SLOT_NORMAL_CP -
                                          cell_cfg_list_[0].tdd_ul_dl_cfg_common.value().pattern1.nof_ul_symbols) {
              continue;
            }
          }
          // This is the fully-UL slot case: check if the SRS can be placed in the UL symbols of the slot.
          else if (res.symbols.start() <
                   NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell.cell_cfg.srs_cfg.max_nof_symbols.to_uint()) {
            continue;
          }
        }
        // FDD case.
        else {
          if (res.symbols.start() < NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell.cell_cfg.srs_cfg.max_nof_symbols.to_uint()) {
            continue;
          }
        }
        cell.srs_res_offset_free_list.emplace_back(res.cell_res_id, offset);
        ++offset_res_cnt;
      }
      cell.slot_resource_cnt.emplace_back(0U, offset_res_cnt);
    }
  }
}

bool du_srs_policy_max_ul_th::alloc_resources(cell_group_config& cell_grp_cfg)
{
  // TODO: Adapt this to the case of UEs with multiple cells configs.
  srsran_assert(
      cells[cell_grp_cfg.cells[0].serv_cell_cfg.cell_index].cell_cfg.ue_ded_serv_cell_cfg.ul_config.has_value(),
      "UE UL config is empty");

  // If periodic SRS is not enabled, don't allocate anything and exit with success.
  if (not cells[0].cell_cfg.srs_cfg.srs_period.has_value() or not cells[cell_grp_cfg.cells[0].serv_cell_cfg.cell_index]
                                                                      .cell_cfg.ue_ded_serv_cell_cfg.ul_config.value()
                                                                      .init_ul_bwp.srs_cfg.has_value()) {
    return true;
  }

  // The UE SRS configuration is taken from a base configuration, saved in the GNB. The details that are UE specific
  // will be added later on in this function.
  cell_grp_cfg.cells[0].serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.emplace(
      cells[cell_grp_cfg.cells[0].serv_cell_cfg.cell_index]
          .cell_cfg.ue_ded_serv_cell_cfg.ul_config.value()
          .init_ul_bwp.srs_cfg.value());
  srs_config& ue_srs_cfg    = cell_grp_cfg.cells[0].serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.value();
  auto&       free_srs_list = cells[cell_grp_cfg.cells[0].serv_cell_cfg.cell_index].srs_res_offset_free_list;

  // Verify where there are SRS resources to allocate a new UE.
  if (free_srs_list.empty()) {
    return false;
  }

  // Find the best resource ID and offset for this UE.
  auto srs_res_id_offset = cells[0].find_optimal_ue_srs_resource();

  if (srs_res_id_offset == free_srs_list.end()) {
    return false;
  }

  const auto& du_res_it = cells[0].get_du_srs_res_cfg(srs_res_id_offset->first);

  if (du_res_it == cells[0].cell_srs_res_list.end()) {
    return false;
  }

  const auto& du_res = *du_res_it;

  // Update the SRS configuration with the parameters that are specific to this resource and for this UE.
  auto&          only_ue_srs_res = ue_srs_cfg.srs_res_list.front();
  const unsigned srs_offset      = srs_res_id_offset->second;
  // NOTE: given that there is only 1 SRS resource per UE, we can assume that the SRS resource ID is 0.
  only_ue_srs_res.id.cell_res_id                        = du_res.cell_res_id;
  only_ue_srs_res.id.ue_res_id                          = static_cast<srs_config::srs_res_id>(0U);
  only_ue_srs_res.periodicity_and_offset.value().offset = srs_offset;
  only_ue_srs_res.tx_comb.tx_comb_offset                = du_res.tx_comb_offset.to_uint();
  only_ue_srs_res.freq_domain_pos                       = du_res.freq_dom_position;
  only_ue_srs_res.res_mapping.start_pos                 = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - du_res.symbols.start() - 1;
  only_ue_srs_res.res_mapping.nof_symb                  = static_cast<srs_nof_symbols>(du_res.symbols.length());
  only_ue_srs_res.sequence_id                           = du_res.sequence_id;

  // Update the SRS configuration with the parameters that are common to the cell.
  only_ue_srs_res.freq_hop.c_srs = cells[cell_grp_cfg.cells[0].serv_cell_cfg.cell_index].srs_common_params.c_srs;
  only_ue_srs_res.freq_domain_shift =
      cells[cell_grp_cfg.cells[0].serv_cell_cfg.cell_index].srs_common_params.freq_shift;

  // Update the SRS resource set with the SRS id.
  ue_srs_cfg.srs_res_set_list.front().srs_res_id_list.front() = only_ue_srs_res.id.ue_res_id;

  // Remove the allocated SRS resource from the free list.
  free_srs_list.erase(srs_res_id_offset);

  // Update the used_not_full slot vector.
  ++cells[0].slot_resource_cnt[srs_offset].first;

  return true;
}

std::vector<std::pair<unsigned, unsigned>>::const_iterator
du_srs_policy_max_ul_th::cell_context::find_optimal_ue_srs_resource()
{
  // The weights assigned here can be set to any value, as long as:
  // - symbol_weight_base is greater than 0;
  // - reuse_slot_discount less than symbol_weight_base;
  // - max_weight is > symbol_weight_base * (srs_builder_params::max_nof_symbols / srs_builder_params::nof_symbols).
  static constexpr unsigned max_weight         = 100U;
  static constexpr unsigned symbol_weight_base = 10U;

  const auto weight_function = [&](const pair_res_id_offset& srs_res) {
    if (cell_cfg.tdd_ul_dl_cfg_common.has_value() and
        is_partually_ul_slot(srs_res.second, cell_cfg.tdd_ul_dl_cfg_common.value())) {
      return 0U;
    }

    // SRS res config.
    const auto srs_res_cfg_it = get_du_srs_res_cfg(srs_res.first);

    if (srs_res_cfg_it == cell_srs_res_list.end()) {
      return max_weight;
    }

    // Give priority to the last symbols within a slot. This reduces the space used for the SRS in a slot.
    const unsigned symb_weight =
        (NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - srs_res_cfg_it->symbols.start()) * symbol_weight_base;

    // We consider a discount if the offset is already used but not full; this way, we give an incentive to the SRS
    // resources not to be allocated on a new slot, to avoid taking PUSCH symbols on a new slot.
    const unsigned reuse_slot_discount = offset_used_not_full(srs_res.second) ? symbol_weight_base / 2U : 0U;

    return symb_weight - reuse_slot_discount;
  };

  auto optimal_res_it = std::min_element(
      srs_res_offset_free_list.begin(),
      srs_res_offset_free_list.end(),
      [&weight_function](const std::pair<unsigned, unsigned>& lhs, const std::pair<unsigned, unsigned>& rhs) {
        return weight_function(lhs) < weight_function(rhs);
      });

  return optimal_res_it;
}

void du_srs_policy_max_ul_th::dealloc_resources(cell_group_config& cell_grp_cfg)
{
  if (not cells[0].cell_cfg.srs_cfg.srs_period.has_value() or
      not cell_grp_cfg.cells[0].serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.has_value()) {
    return;
  }

  const auto& ue_srs_cfg    = cell_grp_cfg.cells[0].serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.value();
  auto&       free_srs_list = cells[cell_grp_cfg.cells[0].serv_cell_cfg.cell_index].srs_res_offset_free_list;

  for (const auto& srs_res : ue_srs_cfg.srs_res_list) {
    const unsigned offset_to_deallocate = srs_res.periodicity_and_offset.value().offset;
    free_srs_list.push_back({srs_res.id.cell_res_id, offset_to_deallocate});

    // Update the used_not_full slot vector.
    srsran_assert(cells[0].slot_resource_cnt[offset_to_deallocate].first != 0, "The offset is expected to be non-zero");
    --cells[0].slot_resource_cnt[offset_to_deallocate].first;
  }
}
