/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "resource_grid.h"
#include "srsran/support/compiler.h"

using namespace srsran;

static subcarrier_spacing get_max_scs(const dl_config_common& dl_cfg)
{
  // Note: assumes sorted list, even though the corresponding TS struct \c scs-SpecificCarrierList might not be.
  return dl_cfg.freq_info_dl.scs_carrier_list.back().scs;
}

/////////////////////////////////////////////////////////////////////////

carrier_subslot_resource_grid::carrier_subslot_resource_grid(const scs_specific_carrier& carrier_cfg_) :
  carrier_cfg(carrier_cfg_), slot_rbs(NOF_OFDM_SYM_PER_SLOT_NORMAL_CP * carrier_cfg.carrier_bandwidth)
{
}

void carrier_subslot_resource_grid::clear()
{
  slot_rbs.reset();
}

void carrier_subslot_resource_grid::fill(ofdm_symbol_range symbols, crb_interval crbs)
{
  srsran_sanity_check(rb_dims().contains(crbs), "CRB interval out-of-bounds");
  srsran_sanity_check(symbols.stop() <= NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, "OFDM symbols out-of-bounds");

  // carrier bitmap RB bit=0 corresponds to CRB=carrier offset. Thus, we need to shift the CRB interval.
  crbs.displace_by(-(int)offset());
  for (unsigned i = symbols.start(); i < symbols.stop(); ++i) {
    slot_rbs.fill(crbs.start() + i * nof_rbs(), crbs.stop() + i * nof_rbs());
  }
}

void carrier_subslot_resource_grid::fill(ofdm_symbol_range symbols, span<const uint16_t> crb_list)
{
  srsran_sanity_check(symbols.stop() <= NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, "OFDM symbols out-of-bounds");

  // carrier bitmap RB bit=0 corresponds to CRB=carrier offset. Thus, we need to shift the CRB interval.
  for (unsigned i = symbols.start(); i < symbols.stop(); ++i) {
    for (uint16_t crb : crb_list) {
      srsran_sanity_check(rb_dims().contains(crb), "CRB interval out-of-bounds");
      crb -= offset();
      slot_rbs.set(crb + i * nof_rbs());
    }
  }
}

bool carrier_subslot_resource_grid::collides(ofdm_symbol_range symbols, crb_interval crbs) const
{
  srsran_sanity_check(rb_dims().contains(crbs), "CRB interval out-of-bounds");
  srsran_sanity_check(symbols.stop() <= NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, "OFDM symbols out-of-bounds");

  // carrier bitmap RB bit=0 corresponds to CRB=carrier offset. Thus, we need to shift the CRB interval.
  crbs.displace_by(-(int)offset());
  for (unsigned i = symbols.start(); i < symbols.stop(); ++i) {
    if (slot_rbs.any(crbs.start() + i * nof_rbs(), crbs.stop() + i * nof_rbs())) {
      return true;
    }
  }
  return false;
}

bool carrier_subslot_resource_grid::collides(ofdm_symbol_range symbols, span<const uint16_t> crb_list) const
{
  srsran_sanity_check(symbols.stop() <= NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, "OFDM symbols out-of-bounds");

  // carrier bitmap RB bit=0 corresponds to CRB=carrier offset. Thus, we need to shift the CRB interval.
  for (unsigned i = symbols.start(); i < symbols.stop(); ++i) {
    for (uint16_t crb : crb_list) {
      srsran_sanity_check(rb_dims().contains(crb), "CRB interval out-of-bounds");
      crb -= offset();
      if (slot_rbs.test(crb + i * nof_rbs())) {
        return true;
      }
    }
  }

  return false;
}

crb_bitmap carrier_subslot_resource_grid::used_crbs(crb_interval bwp_crb_lims, ofdm_symbol_range symbols) const
{
  srsran_sanity_check(symbols.stop() <= NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, "OFDM symbols out-of-bounds");
  slot_rb_bitmap slot_rbs_selected_symbols = slot_rbs.slice(symbols.start() * nof_rbs(), symbols.stop() * nof_rbs());
  crb_bitmap     crb_bits                  = fold_and_accumulate<MAX_NOF_PRBS>(slot_rbs_selected_symbols, nof_rbs());
  crb_bits.fill(0, bwp_crb_lims.start());
  crb_bits.fill(bwp_crb_lims.stop(), crb_bits.size());
  return crb_bits;
}

bool carrier_subslot_resource_grid::all_set(ofdm_symbol_range symbols, crb_interval crbs) const
{
  srsran_sanity_check(rb_dims().contains(crbs), "CRB interval out-of-bounds");
  srsran_sanity_check(symbols.stop() <= NOF_OFDM_SYM_PER_SLOT_NORMAL_CP, "OFDM symbols out-of-bounds");

  // carrier bitmap RB bit=0 corresponds to CRB=carrier offset. Thus, we need to shift the CRB interval.
  crbs.displace_by(-(int)offset());
  for (unsigned i = symbols.start(); i < symbols.stop(); ++i) {
    if (!slot_rbs.all(crbs.start() + i * nof_rbs(), crbs.stop() + i * nof_rbs())) {
      return false;
    }
  }
  return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cell_slot_resource_grid::carrier_resource_grid::carrier_resource_grid(const scs_specific_carrier& carrier_cfg) :
  subslot_rbs(carrier_cfg)
{
}

cell_slot_resource_grid::cell_slot_resource_grid(span<const scs_specific_carrier> scs_carriers)
{
  std::fill(numerology_to_grid_idx.begin(), numerology_to_grid_idx.end(), std::numeric_limits<unsigned>::max());

  carrier_grids.reserve(scs_carriers.size());
  for (const auto& scs_carrier : scs_carriers) {
    carrier_grids.emplace_back(scs_carrier);
    numerology_to_grid_idx[to_numerology_value(scs_carrier.scs)] = carrier_grids.size() - 1;
  }
}

static_vector<subcarrier_spacing, NOF_NUMEROLOGIES> cell_slot_resource_grid::active_scs() const
{
  static_vector<subcarrier_spacing, NOF_NUMEROLOGIES> ret;
  for (unsigned u = 0; u != numerology_to_grid_idx.size(); ++u) {
    if (numerology_to_grid_idx[u] < std::numeric_limits<unsigned>::max()) {
      ret.push_back(to_subcarrier_spacing(u));
    }
  }
  return ret;
}

void cell_slot_resource_grid::clear()
{
  for (auto& carrier : carrier_grids) {
    carrier.subslot_rbs.clear();
  }
}

void cell_slot_resource_grid::fill(grant_info grant)
{
  auto& carrier = get_carrier(grant.scs);

  // Fill RB grid.
  carrier.subslot_rbs.fill(grant.symbols, grant.crbs);
}

void cell_slot_resource_grid::fill(subcarrier_spacing scs, ofdm_symbol_range ofdm_symbols, span<const uint16_t> crbs)
{
  auto& carrier = get_carrier(scs);
  carrier.subslot_rbs.fill(ofdm_symbols, crbs);
}

bool cell_slot_resource_grid::collides(grant_info grant) const
{
  const carrier_resource_grid& carrier = get_carrier(grant.scs);
  return carrier.subslot_rbs.collides(grant.symbols, grant.crbs);
}

bool cell_slot_resource_grid::collides(subcarrier_spacing scs, ofdm_symbol_range ofdm_symbols, crb_interval crbs) const
{
  const carrier_resource_grid& carrier = get_carrier(scs);
  return carrier.subslot_rbs.collides(ofdm_symbols, crbs);
}

bool cell_slot_resource_grid::collides(subcarrier_spacing   scs,
                                       ofdm_symbol_range    ofdm_symbols,
                                       span<const uint16_t> crbs) const
{
  const carrier_resource_grid& carrier = get_carrier(scs);
  return carrier.subslot_rbs.collides(ofdm_symbols, crbs);
}

crb_bitmap cell_slot_resource_grid::used_crbs(const bwp_configuration& bwp_cfg, ofdm_symbol_range symbols) const
{
  const carrier_resource_grid& carrier = get_carrier(bwp_cfg.scs);
  return carrier.subslot_rbs.used_crbs(bwp_cfg.crbs, symbols);
}

crb_bitmap
cell_slot_resource_grid::used_crbs(subcarrier_spacing scs, crb_interval crb_lims, ofdm_symbol_range symbols) const
{
  const carrier_resource_grid& carrier = get_carrier(scs);
  return carrier.subslot_rbs.used_crbs(crb_lims, symbols);
}

bool cell_slot_resource_grid::all_set(grant_info grant) const
{
  const auto& carrier = get_carrier(grant.scs);
  return carrier.subslot_rbs.all_set(grant.symbols, grant.crbs);
}

bool cell_slot_resource_grid::all_set(subcarrier_spacing scs, ofdm_symbol_range ofdm_symbols, crb_interval crbs) const
{
  const auto& carrier = get_carrier(scs);
  return carrier.subslot_rbs.all_set(ofdm_symbols, crbs);
}

cell_slot_resource_grid::carrier_resource_grid& cell_slot_resource_grid::get_carrier(subcarrier_spacing scs)
{
  size_t idx = numerology_to_grid_idx[to_numerology_value(scs)];
  srsran_sanity_check(idx < carrier_grids.size(), "Invalid numerology={}", scs);
  return carrier_grids[idx];
}

const cell_slot_resource_grid::carrier_resource_grid& cell_slot_resource_grid::get_carrier(subcarrier_spacing scs) const
{
  size_t idx = numerology_to_grid_idx[to_numerology_value(scs)];
  srsran_sanity_check(idx < carrier_grids.size(), "Invalid numerology={}", scs);
  return carrier_grids[idx];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cell_slot_resource_allocator::cell_slot_resource_allocator(const cell_configuration& cfg_) :
  cfg(cfg_),
  dl_res_grid(cfg.dl_cfg_common.freq_info_dl.scs_carrier_list),
  ul_res_grid(cfg.dl_cfg_common.freq_info_dl.scs_carrier_list)
{
}

cell_slot_resource_allocator::cell_slot_resource_allocator(const cell_configuration&  cfg_,
                                                           span<scs_specific_carrier> dl_scs_carriers,
                                                           span<scs_specific_carrier> ul_scs_carriers) :
  cfg(cfg_), dl_res_grid(dl_scs_carriers), ul_res_grid(ul_scs_carriers)
{
}

void cell_slot_resource_allocator::slot_indication(slot_point new_slot)
{
  // Clear previous results.
  result.dl.dl_pdcchs.clear();
  result.dl.ul_pdcchs.clear();
  result.dl.bc.ssb_info.clear();
  result.dl.bc.sibs.clear();
  result.dl.rar_grants.clear();
  result.dl.paging_grants.clear();
  result.dl.ue_grants.clear();
  result.dl.csi_rs.clear();
  result.ul.puschs.clear();
  result.ul.prachs.clear();
  result.ul.pucchs.clear();
  dl_res_grid.clear();
  ul_res_grid.clear();

  // Initiate new slot in the same position.
  slot                     = new_slot;
  result.success           = true;
  result.dl.nof_dl_symbols = cfg.get_nof_dl_symbol_per_slot(new_slot);
  result.ul.nof_ul_symbols = cfg.get_nof_ul_symbol_per_slot(new_slot);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

cell_resource_allocator::cell_resource_allocator(const cell_configuration& cfg_) :
  cfg(cfg_),
  max_dl_slot_alloc_delay(SCHEDULER_MAX_K0),
  max_ul_slot_alloc_delay(SCHEDULER_MAX_K0 + std::max(SCHEDULER_MAX_K1, SCHEDULER_MAX_K2 + MAX_MSG3_DELTA) +
                          cfg.ntn_cs_koffset)
{
  // Create cell_slot_resource_allocator objects.
  std::vector<scs_specific_carrier> dl_scs_carriers, ul_scs_carriers;
  subcarrier_spacing                max_scs = cfg.dl_cfg_common.freq_info_dl.scs_carrier_list.back().scs;
  max_scs = std::max(max_scs, cfg.ul_cfg_common.freq_info_ul.scs_carrier_list.back().scs);
  slots.resize(RING_ALLOCATOR_SIZE);
  unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(max_scs);
  for (unsigned i = 0; i < slots.size(); ++i) {
    for (const auto& carrier : cfg.dl_cfg_common.freq_info_dl.scs_carrier_list) {
      unsigned current_slot_dur = nof_slots_per_subframe / get_nof_slots_per_subframe(carrier.scs);
      if (i % current_slot_dur == 0) {
        dl_scs_carriers.emplace_back(carrier);
      }
    }
    for (const auto& carrier : cfg.ul_cfg_common.freq_info_ul.scs_carrier_list) {
      unsigned current_slot_dur = nof_slots_per_subframe / get_nof_slots_per_subframe(carrier.scs);
      if (i % current_slot_dur == 0) {
        ul_scs_carriers.emplace_back(carrier);
      }
    }
    slots[i] = std::make_unique<cell_slot_resource_allocator>(cfg, dl_scs_carriers, ul_scs_carriers);
    dl_scs_carriers.clear();
    ul_scs_carriers.clear();
  }
}

void cell_resource_allocator::slot_indication(slot_point sl_tx)
{
  srsran_sanity_check(not last_slot_ind.valid() or last_slot_ind + 1 == sl_tx, "Slot indication was skipped");
  srsran_sanity_check(sl_tx.numerology() == to_numerology_value(get_max_scs(cfg.dl_cfg_common)),
                      "Invalid slot numerology");

  if (SRSRAN_UNLIKELY(not last_slot_ind.valid())) {
    // First call to slot_indication. Set slot of all slot cell resource grids.
    for (unsigned i = 0; i < slots.size(); ++i) {
      slots[(sl_tx + i).to_uint() % slots.size()]->slot_indication(sl_tx + i);
    }
  } else {
    // Reset old slot state and set its new future slot.
    slot_point slot_to_reset = sl_tx - static_cast<unsigned>(RING_MAX_HISTORY_SIZE);
    auto&      old_slot_res  = slots[slot_to_reset.to_uint() % slots.size()];
    old_slot_res->slot_indication(slot_to_reset + slots.size());
  }
  last_slot_ind = sl_tx;
}
