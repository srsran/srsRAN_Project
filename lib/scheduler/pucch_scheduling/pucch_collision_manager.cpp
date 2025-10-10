/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pucch_collision_manager.h"
#include "../support/pucch/pucch_default_resource.h"
#include "srsran/ran/pucch/pucch_configuration.h"
#include "srsran/ran/pucch/pucch_constants.h"
#include "srsran/ran/pucch/pucch_mapping.h"
#include "srsran/ran/resource_allocation/ofdm_symbol_range.h"
#include "srsran/ran/resource_allocation/rb_interval.h"

using namespace srsran;

namespace {

/// Represents the time-frequency allocation of a PUCCH resource.
struct time_freq_grants {
  prb_interval      prbs1;
  ofdm_symbol_range symbols1;
  prb_interval      prbs2;
  ofdm_symbol_range symbols2;

  /// Constructor from a dedicated PUCCH resource.
  time_freq_grants(const pucch_resource& res)
  {
    unsigned nof_prbs = 1;
    if (const auto* format_2_3 = std::get_if<pucch_format_2_3_cfg>(&res.format_params)) {
      nof_prbs = format_2_3->nof_prbs;
    }

    if (res.second_hop_prb.has_value()) {
      prbs1    = {res.starting_prb, res.starting_prb + nof_prbs};
      symbols1 = {res.starting_sym_idx, res.starting_sym_idx + res.nof_symbols / 2};
      prbs2    = {res.second_hop_prb.value(), res.second_hop_prb.value() + nof_prbs};
      symbols2 = {res.starting_sym_idx + res.nof_symbols / 2, res.starting_sym_idx + res.nof_symbols};
    } else {
      prbs1    = {res.starting_prb, res.starting_prb + nof_prbs};
      symbols1 = {res.starting_sym_idx, res.starting_sym_idx + res.nof_symbols};
      prbs2    = prb_interval();
      symbols2 = ofdm_symbol_range();
    }
  }

  /// Constructor from a common PUCCH resource.
  time_freq_grants(const pucch_default_resource& res, unsigned r_pucch, unsigned size_ul_bwp)
  {
    // Compute PRB_first_hop and PRB_second_hop as per Section 9.2.1, TS 38.213.
    auto prbs = get_pucch_default_prb_index(r_pucch, res.rb_bwp_offset, res.cs_indexes.size(), size_ul_bwp);

    prbs1    = {prbs.first, prbs.first + pucch_constants::FORMAT0_1_4_MAX_NPRB};
    symbols1 = {res.first_symbol_index, res.first_symbol_index + res.nof_symbols / 2};
    prbs2    = {prbs.second, prbs.second + pucch_constants::FORMAT0_1_4_MAX_NPRB};
    symbols2 = {res.first_symbol_index + res.nof_symbols / 2, res.first_symbol_index + res.nof_symbols};
  }

  bool overlaps(const time_freq_grants& other) const
  {
    bool ret = symbols1.overlaps(other.symbols1) and prbs1.overlaps(other.prbs1);
    ret |= symbols1.overlaps(other.symbols2) and prbs1.overlaps(other.prbs2);
    ret |= symbols2.overlaps(other.symbols1) and prbs2.overlaps(other.prbs1);
    ret |= symbols2.overlaps(other.symbols2) and prbs2.overlaps(other.prbs2);
    return ret;
  }

  bool operator==(const time_freq_grants& other) const
  {
    return symbols1 == other.symbols1 and prbs1 == other.prbs1 and symbols2 == other.symbols2 and prbs2 == other.prbs2;
  }

  bool operator!=(const time_freq_grants& other) const { return not(*this == other); }
};

/// Represents the relevant information of a PUCCH resource for collision checking.
struct resource_info {
  time_freq_grants grants;
  pucch_format     format;
  unsigned         multiplexing_index;

  /// Constructor from a dedicated PUCCH resource.
  resource_info(const pucch_resource& res) : grants(res), format(res.format)
  {
    switch (res.format) {
      case pucch_format::FORMAT_0: {
        const auto& f0_params = std::get<pucch_format_0_cfg>(res.format_params);
        multiplexing_index    = f0_params.initial_cyclic_shift;
      } break;
      case pucch_format::FORMAT_1: {
        const auto& f1_params = std::get<pucch_format_1_cfg>(res.format_params);
        multiplexing_index    = f1_params.initial_cyclic_shift +
                             f1_params.time_domain_occ * pucch_constants::format0_initial_cyclic_shift_range.length();
      } break;
      case pucch_format::FORMAT_4: {
        // TODO: Find out if F4 resources with same OCC index but different OCC length collide or not.
        // i.e., {OCC length = 2, OCC index = 1} and {OCC length = 4, OCC index = 1}.
        // If that is the case, we need to include OCC length in the multiplexing index.
        const auto& f4_params = std::get<pucch_format_4_cfg>(res.format_params);
        multiplexing_index    = static_cast<unsigned>(f4_params.occ_index);
      } break;
      default:
        // Non multiplexed formats.
        multiplexing_index = 0;
        break;
    }
  }

  /// Constructor from a common PUCCH resource.
  resource_info(const pucch_default_resource& res, unsigned r_pucch, unsigned size_ul_bwp) :
    grants(res, r_pucch, size_ul_bwp),
    format(res.format),
    multiplexing_index(res.cs_indexes[get_pucch_default_cyclic_shift(r_pucch, res.cs_indexes.size())])
  {
  }
};

} // namespace

static bool do_resources_collide(const resource_info& res1, const resource_info& res2)
{
  if (not res1.grants.overlaps(res2.grants)) {
    // Resources that do not overlap in time and frequency do not collide.
    return false;
  }

  if (res1.format != res2.format) {
    // Resources with different formats always collide if they overlap in time and frequency.
    return true;
  }

  if (res1.grants != res2.grants) {
    // We can only make sure resources have orthogonal sequences if they have the same time/frequency allocation.
    // Note: this is a simplification, as the sequences may still be aligned for the colliding REs in some cases. But
    // checking that would require complex format-specific logic, so we assume they collide.
    return true;
  }

  // Resources with the same format and time/frequency grants only collide if they have the same multiplexing index.
  // Note: resources with Format 2/3 always collide as they are not multiplexed (multiplexing index is always 0).
  return res1.multiplexing_index == res2.multiplexing_index;
}

pucch_collision_manager::pucch_collision_manager(const cell_configuration& cell_cfg_) :
  cell_cfg(cell_cfg_),
  collision_matrix(nof_common_res + cell_cfg.ded_pucch_resources.size(),
                   bounded_bitset<max_nof_cell_resources>(nof_common_res + cell_cfg.ded_pucch_resources.size())),
  slots_ctx({bounded_bitset<max_nof_cell_resources>(nof_common_res + cell_cfg.ded_pucch_resources.size())})
{
  // Get the parameter N_bwp_size, which is the Initial UL BWP size in PRBs, as per TS 38.213, Section 9.2.1.
  const unsigned size_ul_bwp = cell_cfg.ul_cfg_common.init_ul_bwp.generic_params.crbs.length();

  // Get PUCCH common resource config from Table 9.2.1-1, TS 38.213.
  const pucch_default_resource common_default_res = get_pucch_default_resource(
      cell_cfg.ul_cfg_common.init_ul_bwp.pucch_cfg_common->pucch_resource_common, size_ul_bwp);

  // Collect all resources (common + dedicated).
  static_vector<resource_info, max_nof_cell_resources> all_resources;
  for (unsigned r_pucch = 0; r_pucch < nof_common_res; ++r_pucch) {
    all_resources.push_back(resource_info(common_default_res, r_pucch, size_ul_bwp));
  }
  for (const auto& res : cell_cfg.ded_pucch_resources) {
    all_resources.push_back(resource_info(res));
  }

  // Precompute the collision matrix.
  for (size_t i = 0; i != all_resources.size(); ++i) {
    // A resource always collides with itself.
    collision_matrix[i].set(i);

    // Note: The collision matrix is symmetric.
    for (size_t j = i + 1; j != all_resources.size(); ++j) {
      if (do_resources_collide(all_resources[i], all_resources[j])) {
        collision_matrix[i].set(j);
        collision_matrix[j].set(i);
      }
    }
  }
}

void pucch_collision_manager::slot_indication(slot_point sl_tx)
{
  // Clear previous slot context.
  slots_ctx[(sl_tx - 1).to_uint()].current_state.reset();
}

void pucch_collision_manager::stop()
{
  // Clear all slot contexts.
  for (auto& ctx : slots_ctx) {
    ctx.current_state.reset();
  }
}

bool pucch_collision_manager::check_ded_and_ded_collision(unsigned cell_res_id1, unsigned cell_res_id2) const
{
  srsran_assert(cell_res_id1 < cell_cfg.ded_pucch_resources.size(),
                "cell_res_id1 ({}) is out of range ({})",
                cell_res_id1,
                cell_cfg.ded_pucch_resources.size());
  srsran_assert(cell_res_id2 < cell_cfg.ded_pucch_resources.size(),
                "cell_res_id2 ({}) is out of range ({})",
                cell_res_id2,
                cell_cfg.ded_pucch_resources.size());

  return collision_matrix[nof_common_res + cell_res_id1].test(nof_common_res + cell_res_id2);
}

bool pucch_collision_manager::check_common_and_ded_collision(r_pucch_t r_pucch, unsigned cell_res_id) const
{
  srsran_assert(cell_res_id < cell_cfg.ded_pucch_resources.size(),
                "cell_res_id ({}) is out of range ({})",
                cell_res_id,
                cell_cfg.ded_pucch_resources.size());

  return collision_matrix[r_pucch.value()].test(nof_common_res + cell_res_id);
}

bool pucch_collision_manager::does_common_collide(slot_point sl, r_pucch_t r_pucch) const
{
  const auto& ctx = slots_ctx[sl.to_uint()];
  const auto& row = collision_matrix[r_pucch.value()];

  return (row & ctx.current_state).any();
}

bool pucch_collision_manager::does_ded_collide(slot_point sl, unsigned cell_res_id) const
{
  srsran_assert(cell_res_id < cell_cfg.ded_pucch_resources.size(), "cell_res_id out of range");

  const auto& ctx = slots_ctx[sl.to_uint()];
  const auto& row = collision_matrix[nof_common_res + cell_res_id];

  return (row & ctx.current_state).any();
}

void pucch_collision_manager::allocate_common(slot_point sl, r_pucch_t r_pucch)
{
  auto& ctx = slots_ctx[sl.to_uint()];
  ctx.current_state.set(r_pucch.value());
}

void pucch_collision_manager::allocate_ded(slot_point sl, unsigned cell_res_id)
{
  srsran_assert(cell_res_id < cell_cfg.ded_pucch_resources.size(), "cell_res_id out of range");

  auto& ctx = slots_ctx[sl.to_uint()];
  ctx.current_state.set(nof_common_res + cell_res_id);
}
