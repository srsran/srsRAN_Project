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

#include "du_srs_resource_manager.h"
#include "du_ue_resource_config.h"
#include "srsran/ran/srs/srs_bandwidth_configuration.h"

using namespace srsran;
using namespace srs_du;

// Helper that computes the SRS bandwidth parameter \f$C_{SRS}\f$ based on the number of UL RBs.
static std::optional<unsigned> compute_srs_bw_param(unsigned nof_ul_rbs)
{
  // Iterate over Table 6.4.1.4.3-1, TS 38.211, and find the minimum \f$C_{SRS}\f$ value that maximizes \f$m_{SRS,0}\f$
  // under the constraint \f$m_{SRS,0}\f$ <= UL RBs.

  // As per Table 6.4.1.4.3-1, TS 38.211, the maximum value of \f$C_{SRS}\f$ is 63.
  constexpr unsigned max_non_valid_c_srs = 64;
  // As per Table 6.4.1.4.3-1, TS 38.211, we do not consider frequency hopping.
  constexpr uint8_t b_srs_0 = 0;
  // Defines the pair of C_SRS and m_SRS values.
  using pair_c_srs_m_srs                          = std::pair<unsigned, unsigned>;
  std::optional<pair_c_srs_m_srs> candidate_c_srs = std::nullopt;
  for (uint8_t c_srs = 0; c_srs != max_non_valid_c_srs; ++c_srs) {
    auto srs_params = srs_configuration_get(c_srs, b_srs_0);

    if (not srs_params.has_value()) {
      srsran_assertion_failure("C_SRS is not compatible with the current BW configuration");
      return std::nullopt;
    }

    // If there is no candidate C_SRS value, we set the first valid C_SRS value as the candidate.
    if (not candidate_c_srs.has_value()) {
      candidate_c_srs = pair_c_srs_m_srs{c_srs, srs_params.value().m_srs};
    }
    // NOTE: the condition srs_params.value().m_srs > candidate_c_srs->second is used to find the minimum C_SRS value
    // that maximizes m_SRS.
    else if (srs_params.value().m_srs <= nof_ul_rbs and srs_params.value().m_srs > candidate_c_srs->second) {
      candidate_c_srs = pair_c_srs_m_srs{c_srs, srs_params.value().m_srs};
    }
    // If we reach this point, no need to keep looking for a valid C_SRS value.
    if (srs_params.value().m_srs > nof_ul_rbs) {
      break;
    }
  }
  return candidate_c_srs.value().first;
}

// Helper that returns the frequency shift value for the SRS; the value is computed in such a way that the SRS resources
// are placed at the center of the band.
static unsigned compute_freq_shift(unsigned c_srs, unsigned nof_ul_rbs)
{
  // As per Section 6.4.1.4.3, the parameter \f$m_{SRS}\f$ = 0 is an index that, along with \f$C_{SRS}\f$, maps to the
  // bandwidth of the SRS resources.
  constexpr uint8_t                b_srs_0    = 0;
  std::optional<srs_configuration> srs_params = srs_configuration_get(c_srs, b_srs_0);
  srsran_sanity_check(srs_params.has_value() and nof_ul_rbs >= srs_params.value().m_srs,
                      "The SRS configuration is not valid");

  return (nof_ul_rbs - srs_params.value().m_srs) / 2;
}

static bool is_ul_slot(unsigned offset, const std::optional<tdd_ul_dl_config_common>& tdd_cfg)
{
  if (not tdd_cfg.has_value()) {
    return true;
  }
  const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.value().ref_scs));
  return has_active_tdd_ul_symbols(tdd_cfg.value(), slot_index);
}

static bool is_partially_ul_slot(unsigned offset, const std::optional<tdd_ul_dl_config_common>& tdd_cfg)
{
  if (not tdd_cfg.has_value()) {
    return false;
  }
  const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.value().ref_scs));
  return is_tdd_partial_ul_slot(tdd_cfg.value(), slot_index);
}

// Helper that updates the starting SRS config with user-defined parameters.
static srs_config build_default_srs_cfg(const du_cell_config& default_cell_cfg)
{
  srsran_assert(default_cell_cfg.ue_ded_serv_cell_cfg.ul_config.has_value() and
                    default_cell_cfg.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.has_value(),
                "DU cell config is not valid");

  // If the DU is not configured for periodic SRS, we don't need to update the SRS configuration.
  if (not default_cell_cfg.srs_cfg.srs_period.has_value()) {
    return default_cell_cfg.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.value();
  }

  auto srs_cfg = default_cell_cfg.ue_ded_serv_cell_cfg.ul_config.value().init_ul_bwp.srs_cfg.value();

  srsran_assert(srs_cfg.srs_res_list.size() == 1 and srs_cfg.srs_res_set_list.size() == 1,
                "The SRS resource list and the SRS resource set list are expected to have a single element");

  srs_config::srs_resource& res = srs_cfg.srs_res_list.back();
  res.res_type                  = srs_resource_type::periodic;
  // Set offset to 0. The offset will be updated later on, when the UE is allocated the SRS resources.
  res.periodicity_and_offset.emplace(
      srs_config::srs_periodicity_and_offset{.period = default_cell_cfg.srs_cfg.srs_period.value(), .offset = 0});
  // Set the SRS resource ID to 0, as there is only 1 SRS resource per UE.
  res.id.ue_res_id = static_cast<srs_config::srs_res_id>(0U);

  srs_config::srs_resource_set& res_set = srs_cfg.srs_res_set_list.back();
  res_set.res_type.emplace<srs_config::srs_resource_set::periodic_resource_type>(
      srs_config::srs_resource_set::periodic_resource_type{});
  // Set the SRS resource set ID to 0, as there is only 1 SRS resource set per UE.
  res_set.id = static_cast<srs_config::srs_res_set_id>(0U);

  return srs_cfg;
}

du_srs_policy_max_ul_rate::cell_context::cell_context(const du_cell_config& cfg) :
  cell_cfg(cfg), tdd_ul_dl_cfg_common(cfg.tdd_ul_dl_cfg_common), default_srs_cfg(build_default_srs_cfg(cfg))
{
}

du_srs_policy_max_ul_rate::du_srs_policy_max_ul_rate(span<const du_cell_config> cell_cfg_list_) :
  cells(cell_cfg_list_.begin(), cell_cfg_list_.end())
{
  for (auto& cell : cells) {
    if (not cell.cell_cfg.srs_cfg.srs_period.has_value()) {
      continue;
    }

    std::optional<unsigned> c_srs =
        compute_srs_bw_param(cell.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length()).value();
    srsran_assert(c_srs.has_value(), "SRS parameters didn't provide a valid C_SRS value");
    cell.srs_common_params.c_srs = c_srs.value();
    std::optional<unsigned> freq_shift =
        compute_freq_shift(c_srs.value(), cell.cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length());
    srsran_assert(freq_shift.has_value(), "SRS parameters didn't provide a valid freq_shift value");
    cell.srs_common_params.freq_shift = freq_shift.value();
    cell.srs_common_params.p0         = cell.cell_cfg.srs_cfg.p0;

    // TODO: evaluate whether we need to consider the case of multiple cells.
    cell.cell_srs_res_list = generate_cell_srs_list(cell.cell_cfg);

    const auto srs_period_slots = static_cast<unsigned>(cell.cell_cfg.srs_cfg.srs_period.value());
    // Reserve the size of the vector and set the SRS counter of each offset to 0.
    cell.slot_resource_cnt.reserve(srs_period_slots);
    cell.slot_resource_cnt.assign(srs_period_slots, 0U);
    cell.srs_res_offset_free_list.reserve(du_srs_policy_max_ul_rate::cell_context::max_nof_srs_res);
    cell.nof_res_per_symb_interval = static_cast<unsigned>(cell.cell_cfg.srs_cfg.tx_comb) *
                                     static_cast<unsigned>(cell.cell_cfg.srs_cfg.cyclic_shift_reuse_factor) *
                                     static_cast<unsigned>(cell.cell_cfg.srs_cfg.sequence_id_reuse_factor);

    for (unsigned offset = 0; offset != srs_period_slots; ++offset) {
      // We don't generate more than the maximum number of SRS resources per cell.
      if (cell.srs_res_offset_free_list.size() >= du_srs_policy_max_ul_rate::cell_context::max_nof_srs_res) {
        break;
      }

      // Verify whether the offset maps to a partially- or fully-UL slot.
      if (not is_ul_slot(offset, cell.tdd_ul_dl_cfg_common)) {
        continue;
      }

      for (auto& res : cell.cell_srs_res_list) {
        // Handle TDD and FDD configurations separately, as we treat partially-UL slots differently from
        // fully-UL slots.
        if (is_partially_ul_slot(offset, cell.tdd_ul_dl_cfg_common)) {
          // For partially-UL slots, we need to check if the SRS can be placed in the UL symbols of the slot.
          const auto&    tdd_cfg    = cell.tdd_ul_dl_cfg_common.value();
          const unsigned slot_index = offset % (NOF_SUBFRAMES_PER_FRAME * get_nof_slots_per_subframe(tdd_cfg.ref_scs));
          // As per Section 6.4.1.4.1, TS 38.211, the SRS resources can only be placed in the last 6 symbols of the
          // slot.
          static constexpr unsigned max_srs_symbols = 6U;
          const unsigned            nof_ul_symbols_for_srs =
              std::min(get_active_tdd_ul_symbols(tdd_cfg, slot_index, cyclic_prefix::NORMAL).length(), max_srs_symbols);
          if (res.symbols.start() < NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - nof_ul_symbols_for_srs) {
            continue;
          }
        }
        // NOTE: for TDD, the offset that are not UL slots are skipped above.
        // FDD case and TDD case for fully-UL slot.
        else if (res.symbols.start() <
                 NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - cell.cell_cfg.srs_cfg.max_nof_symbols.to_uint()) {
          continue;
        }
        cell.srs_res_offset_free_list.emplace_back(res.cell_res_id, offset);
      }
    }
  }
}

bool du_srs_policy_max_ul_rate::alloc_resources(cell_group_config& cell_grp_cfg)
{
  // First, check there are available SRS resources in each UE's cell.
  bool alloc_succeeded = true;
  for (auto& cell_cfg_ded : cell_grp_cfg.cells) {
    const auto& ue_du_cell = cells[cell_cfg_ded.serv_cell_cfg.cell_index];

    srsran_assert(cell_cfg_ded.serv_cell_cfg.ul_config.has_value() and
                      not cell_cfg_ded.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.has_value(),
                  "UE UL config should be non-empty but with an empty SRS config");

    cell_cfg_ded.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.emplace(ue_du_cell.default_srs_cfg);

    // If periodic SRS is not enabled on this cell, don't allocate anything and continue to the next cell.
    if (not ue_du_cell.cell_cfg.srs_cfg.srs_period.has_value()) {
      continue;
    }

    const auto& free_srs_list = ue_du_cell.srs_res_offset_free_list;
    // Verify whether there are SRS resources to allocate a new UE.
    if (free_srs_list.empty()) {
      // If the allocation failed, reset the SRS configuration.
      cell_cfg_ded.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.reset();
      alloc_succeeded = false;
      break;
    }
  }

  // Reset the UE configuration in each cell before existing with failure.
  if (not alloc_succeeded) {
    for (auto& cell_cfg_ded : cell_grp_cfg.cells) {
      cell_cfg_ded.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.reset();
    }
    return false;
  }

  // From this point on, the allocation is expected to succeed, as there are SRS resources available in each cell.
  for (auto& cell_cfg_ded : cell_grp_cfg.cells) {
    auto& ue_du_cell = cells[cell_cfg_ded.serv_cell_cfg.cell_index];

    // The UE SRS configuration is taken from a base configuration, saved in the GNB. The UE specific parameters will be
    // added later on in this function.
    // NOTE: This config could be as well for non-periodic SRS, therefore emplace anyway.
    cell_cfg_ded.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.emplace(ue_du_cell.default_srs_cfg);

    // If periodic SRS is not enabled on this cell, don't allocate anything and continue to the next cell.
    if (not ue_du_cell.cell_cfg.srs_cfg.srs_period.has_value()) {
      continue;
    }

    srs_config& ue_srs_cfg    = cell_cfg_ded.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.value();
    auto&       free_srs_list = ue_du_cell.srs_res_offset_free_list;

    // Find the best resource ID and offset for this UE, according to the class policy.
    auto srs_res_id_offset = ue_du_cell.find_optimal_ue_srs_resource();
    srsran_assert(srs_res_id_offset != free_srs_list.end(), "No SRS resource returned from a non-emtpy set");

    const auto& du_res_it = ue_du_cell.get_du_srs_res_cfg(srs_res_id_offset->first);
    srsran_assert(du_res_it != ue_du_cell.cell_srs_res_list.end(), "The provided cell-ID is invalid");

    const auto& du_res = *du_res_it;

    // Update the SRS configuration with the parameters that are specific to this resource and for this UE.
    auto&          only_ue_srs_res = ue_srs_cfg.srs_res_list.front();
    const unsigned srs_offset      = srs_res_id_offset->second;
    // NOTE: given that there is only 1 SRS resource per UE, we can assume that the SRS resource ID is 0.
    only_ue_srs_res.id.cell_res_id = du_res.cell_res_id;
    only_ue_srs_res.id.ue_res_id   = static_cast<srs_config::srs_res_id>(0U);
    srsran_assert(ue_du_cell.cell_cfg.ul_carrier.nof_ant == 1 or ue_du_cell.cell_cfg.ul_carrier.nof_ant == 2 or
                      ue_du_cell.cell_cfg.ul_carrier.nof_ant == 4,
                  "The number of UL antenna ports is not valid");
    only_ue_srs_res.nof_ports                    = srs_config::srs_resource::nof_srs_ports::port1;
    only_ue_srs_res.tx_comb.size                 = ue_du_cell.cell_cfg.srs_cfg.tx_comb;
    only_ue_srs_res.tx_comb.tx_comb_offset       = du_res.tx_comb_offset.to_uint();
    only_ue_srs_res.tx_comb.tx_comb_cyclic_shift = du_res.cs;
    only_ue_srs_res.freq_domain_pos              = du_res.freq_dom_position;
    only_ue_srs_res.res_mapping.start_pos        = NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - du_res.symbols.start() - 1;
    only_ue_srs_res.res_mapping.nof_symb         = static_cast<srs_nof_symbols>(du_res.symbols.length());
    only_ue_srs_res.sequence_id                  = du_res.sequence_id;

    // Update the SRS configuration with the parameters that are common to the cell.
    only_ue_srs_res.freq_hop.c_srs = ue_du_cell.srs_common_params.c_srs;
    // We assume that the frequency hopping is disabled and that the SRS occupies all possible RBs within the BWP. Refer
    // to Section 6.4.1.4.3, TS 38.211.
    only_ue_srs_res.freq_hop.b_srs    = 0U;
    only_ue_srs_res.freq_hop.b_hop    = 0U;
    only_ue_srs_res.freq_domain_shift = ue_du_cell.srs_common_params.freq_shift;

    only_ue_srs_res.periodicity_and_offset.emplace(srs_config::srs_periodicity_and_offset{
        .period = ue_du_cell.cell_cfg.srs_cfg.srs_period.value(), .offset = static_cast<uint16_t>(srs_offset)});

    // Update the SRS resource set with the SRS id.
    ue_srs_cfg.srs_res_set_list.front().srs_res_id_list.front() = only_ue_srs_res.id.ue_res_id;

    // Update the SRS resource set with the p0.
    ue_srs_cfg.srs_res_set_list.front().p0 = ue_du_cell.srs_common_params.p0;

    // Remove the allocated SRS resource from the free list.
    free_srs_list.erase(srs_res_id_offset);

    // Update the SRS resource per slot counter.
    ++ue_du_cell.slot_resource_cnt[srs_offset];
  }

  return true;
}

std::vector<du_srs_policy_max_ul_rate::cell_context::pair_res_id_offset>::const_iterator
du_srs_policy_max_ul_rate::cell_context::find_optimal_ue_srs_resource()
{
  // The weights assigned here can be set to arbitrarily value, as long as:
  // - symbol_weight_base is greater than 0;
  // - reuse_slot_discount is less than symbol_weight_base;
  // - max_weight > symbol_weight_base * (srs_builder_params::max_nof_symbols / srs_builder_params::nof_symbols).
  static constexpr unsigned max_weight         = 100U;
  static constexpr unsigned symbol_weight_base = 10U;
  // We give a discount to the symbol weight if the offset is already used but not full.
  static const unsigned partial_symb_interval_discount = symbol_weight_base / 2U;

  const auto weight_function = [&](const pair_res_id_offset& srs_res) {
    if (cell_cfg.tdd_ul_dl_cfg_common.has_value() and
        is_partially_ul_slot(srs_res.second, cell_cfg.tdd_ul_dl_cfg_common.value())) {
      return 0U;
    }

    // SRS res config.
    const auto srs_res_cfg_it = get_du_srs_res_cfg(srs_res.first);

    if (srs_res_cfg_it == cell_srs_res_list.end()) {
      return max_weight;
    }

    // Give priority to the last symbol intervals within a slot. This reduces the space used for the SRS in a slot.
    const unsigned symb_weight =
        symbol_weight_base *
        ((NOF_OFDM_SYM_PER_SLOT_NORMAL_CP - srs_res_cfg_it->symbols.start()) / srs_res_cfg_it->symbols.length());

    // We consider a discount if the offset is already used but not full; this way, we give an incentive
    // to the SRS resources not to be allocated on a new slot, to avoid taking PUSCH symbols on a new
    // slot.
    const unsigned reuse_slot_discount =
        offset_interval_used_not_full(srs_res.second) ? partial_symb_interval_discount : 0U;

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

void du_srs_policy_max_ul_rate::dealloc_resources(cell_group_config& cell_grp_cfg)
{
  for (auto& cell_cfg_ded : cell_grp_cfg.cells) {
    // This is the cell index inside the DU.
    auto& ue_du_cell = cells[cell_cfg_ded.serv_cell_cfg.cell_index];

    if (not ue_du_cell.cell_cfg.srs_cfg.srs_period.has_value() or
        not cell_cfg_ded.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.has_value()) {
      continue;
    }

    const auto& ue_srs_cfg    = cell_cfg_ded.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.value();
    auto&       free_srs_list = ue_du_cell.srs_res_offset_free_list;

    for (const auto& srs_res : ue_srs_cfg.srs_res_list) {
      const unsigned offset_to_deallocate = srs_res.periodicity_and_offset.value().offset;
      free_srs_list.emplace_back(srs_res.id.cell_res_id, offset_to_deallocate);

      // Update the used_not_full slot vector.gnb
      srsran_assert(ue_du_cell.slot_resource_cnt[offset_to_deallocate] != 0,
                    "The slot resource counter is expected to be non-zero");
      --ue_du_cell.slot_resource_cnt[offset_to_deallocate];
    }

    // Reset the SRS configuration in this UE. This makes sure the DU will exit this function immediately when it gets
    // called again for the same UE (upon destructor's call).
    cell_cfg_ded.serv_cell_cfg.ul_config->init_ul_bwp.srs_cfg.reset();
  }
}
