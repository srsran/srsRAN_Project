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

#pragma once

#include "../config/cell_configuration.h"
#include "../support/bwp_helpers.h"
#include "../support/rb_helper.h"
#include "resource_grid_util.h"
#include "srsran/adt/circular_array.h"
#include "srsran/ran/slot_point.h"
#include "srsran/scheduler/mac_scheduler.h"

namespace srsran {

/// Bitset of CRBs with size up to 275.
using crb_bitmap = bounded_bitset<MAX_NOF_PRBS, true>;

/// Parameters of a PDSCH or PUSCH grant allocation within a BWP.
struct bwp_sch_grant_info {
  const bwp_configuration* bwp_cfg;
  ofdm_symbol_range        symbols;
  /// PRB interval within respective BWP.
  prb_interval prbs;

  bwp_sch_grant_info(const bwp_configuration& bwp_, ofdm_symbol_range symbols_, prb_interval prbs_) :
    bwp_cfg(&bwp_), symbols(symbols_), prbs(prbs_)
  {
    srsran_sanity_check(symbols.stop() <= get_nsymb_per_slot(bwp_cfg->cp), "OFDM symbols do not fit slot");
    srsran_sanity_check(prbs.stop() <= bwp_cfg->crbs.length(), "PRBs={} do not fit BWP={}", prbs, bwp_cfg->crbs);
  }
};

/// Parameters of a grant allocation in the cell resource grid.
struct grant_info {
  subcarrier_spacing scs;
  ofdm_symbol_range  symbols;
  /// Common RBs of the grant. RB==0 corresponds to the RB that overlaps with the pointA.
  crb_interval crbs;

  grant_info() = default;
  grant_info(subcarrier_spacing scs_, ofdm_symbol_range symbols_, crb_interval crbs_) :
    scs(scs_), symbols(symbols_), crbs(crbs_)
  {
  }
  grant_info(const bwp_sch_grant_info& grant) :
    scs(grant.bwp_cfg->scs), symbols(grant.symbols), crbs(prb_to_crb(*grant.bwp_cfg, grant.prbs))
  {
  }
};

/// Derives Carrier CRB limits from scs-SpecificCarrier.
inline crb_interval get_carrier_rb_dims(const scs_specific_carrier& carrier_cfg)
{
  return crb_interval{carrier_cfg.offset_to_carrier, carrier_cfg.offset_to_carrier + carrier_cfg.carrier_bandwidth};
}

/// \brief Represents the Symbol x CRB resource grid of a DL/UL Carrier. The number of CRBs of the grid will depend on
/// the carrier bandwidth and numerology used.
class carrier_subslot_resource_grid
{
public:
  explicit carrier_subslot_resource_grid(const scs_specific_carrier& carrier_cfg);

  subcarrier_spacing scs() const { return carrier_cfg.scs; }
  unsigned           nof_rbs() const { return carrier_cfg.carrier_bandwidth; }
  unsigned           offset() const { return carrier_cfg.offset_to_carrier; }
  crb_interval       rb_dims() const { return get_carrier_rb_dims(carrier_cfg); }

  /// Clearer Carrier Resource Grid.
  void clear();

  /// Allocates the symbol x CRB range in the carrier resource grid.
  /// \param symbols OFDM symbol interval of the allocation. Interval must fall within [0, 14).
  /// \param crbs CRB interval, where CRB=0 corresponds to the CRB closest to pointA.
  void fill(ofdm_symbol_range symbols, crb_interval crbs);

  /// Allocates the symbol x CRB list in the carrier resource grid.
  /// \param symbols OFDM symbol interval of the allocation. Interval must fall within [0, 14).
  /// \param crbs List of CRBs, where CRB=0 corresponds to the CRB closest to pointA.
  void fill(ofdm_symbol_range symbols, span<const uint16_t> crb_list);

  /// Clears allocated symbol x CRB list in the carrier resource grid.
  /// \param symbols OFDM symbol interval of the allocation. Interval must fall within [0, 14).
  /// \param crbs List of CRBs, where CRB=0 corresponds to the CRB closest to pointA.
  void clear(ofdm_symbol_range symbols, span<const uint16_t> crb_list);

  /// Checks whether the provided symbol x CRB range collides with any other allocation in the carrier resource grid.
  /// \param symbols OFDM symbol interval of the allocation. Interval must fall within [0, 14).
  /// \param crbs CRB interval, where CRB=0 corresponds to the CRB closest to pointA.
  /// \return true if a collision was detected. False otherwise.
  bool collides(ofdm_symbol_range symbols, crb_interval crbs) const;

  /// Checks whether the provided symbol x CRB list collides with any other allocation in the carrier resource grid.
  /// \param symbols OFDM symbol interval of the allocation. Interval must fall within [0, 14).
  /// \param crbs List of CRBs, where CRB=0 corresponds to the CRB closest to pointA.
  /// \return true if a collision was detected. False otherwise.
  bool collides(ofdm_symbol_range symbols, span<const uint16_t> crb_list) const;

  /// \brief Calculates a bitmap where each bit set one represents a CRB that is occupied or unavailable.
  /// A CRB is considered occupied if it is outside of the provided BWP CRB boundaries or if it is already allocated
  /// in at least one OFDM symbol of the provided OFDM symbol range.
  /// \param[in] bwp_crb_lims CRB range where the BWP is located in the frequency domain, and used for the CRB to PRB
  /// conversion.
  /// \param[in] symbols Range of OFDM symbols, where the search for available CRBs is carrier out.
  /// \return an CRB bitmap with bits set to one for unavailable CRBs.
  crb_bitmap used_crbs(crb_interval bwp_crb_lims, ofdm_symbol_range symbols) const;

  /// Checks whether the provided symbol x CRB range in the carrier resource grid is set.
  /// \param symbols OFDM symbol interval of the allocation. Interval must fall within [0, 14).
  /// \param crbs CRB interval, where CRB=0 corresponds to the CRB closest to pointA.
  /// \return true if all symbols and CRBs  of the provided range are set. False otherwise.
  bool all_set(ofdm_symbol_range symbols, crb_interval crbs) const;

private:
  /// Represents a matrix of symbol index x carrier RB index. The matrix dimensions get scaled based on the number
  /// of carrier RBs. RB index=0 corresponds to the carrier offset. Resources in the bitset are represented in the
  /// following order: [{symb=0,RB=0}, {symb=0,RB=1}, ..., {symb=1,RB=0}, ..., {symb=14,RB=carrierRBs}]
  using slot_rb_bitmap = bounded_bitset<NOF_OFDM_SYM_PER_SLOT_NORMAL_CP * MAX_NOF_PRBS, true>;

  /// Carrier configuration containining numerology, carrier offset and carrier bandwidth.
  scs_specific_carrier carrier_cfg;

  /// symbol x RB bitmap of this carrier.
  slot_rb_bitmap slot_rbs;
};

/// \brief Manages the allocation of RBs in the resource grid of a given service cell for a given slot. This class
/// manages multiple carriers of different numerologies associated to the same cell.
class cell_slot_resource_grid
{
public:
  explicit cell_slot_resource_grid(span<const scs_specific_carrier> scs_carriers);

  /// Checks active subcarrier spacings.
  static_vector<subcarrier_spacing, NOF_NUMEROLOGIES> active_scs() const;

  /// Reset the resource grid to empty.
  void clear();

  /// Allocates the symbol x RB range in the cell resource grid.
  /// \param prbs PRB interval of the allocation. PRB=0 corresponds to the first PRB of the BWP.
  /// \param symbols OFDM symbol interval of the allocation.
  void fill(grant_info grant);
  void fill(subcarrier_spacing scs, ofdm_symbol_range ofdm_symbols, span<const uint16_t> crbs);

  /// Clears allocated symbol x RB range in the cell resource grid.
  /// \param symbols OFDM symbol interval of the allocation.
  /// \param prbs PRB interval of the allocation. PRB=0 corresponds to the first PRB of the BWP.
  void clear(subcarrier_spacing scs, ofdm_symbol_range ofdm_symbols, span<const uint16_t> crbs);

  /// Checks whether the provided symbol x RB range collides with any other allocation in the cell resource grid.
  /// \param grant contains the symbol x RB range whose available we want to check.
  /// \return true if at least one symbol x RB of grant is currently occupied in the resource grid.
  bool collides(grant_info grant) const;
  bool collides(subcarrier_spacing scs, ofdm_symbol_range ofdm_symbols, crb_interval crbs) const;
  bool collides(subcarrier_spacing scs, ofdm_symbol_range ofdm_symbols, span<const uint16_t> crbs) const;

  /// \brief Calculates a bitmap where each bit set to one represents a CRB that is occupied or unavailable.
  /// A CRB is considered occupied if it is outside of the provided BWP CRB boundaries or if it is already allocated
  /// in at least one OFDM symbol of the provided OFDM symbol range.
  /// \param[in] bwp_cfg Bandwidth configuration for which the allocation is to be made.
  /// \param[in] symbols Range of OFDM symbols, where the search for available CRBs is carrier out.
  /// \return a CRB bitmap with bits set to one for unavailable CRBs.
  crb_bitmap used_crbs(const bwp_configuration& bwp_cfg, ofdm_symbol_range symbols) const;

  /// \brief Calculates a bitmap where each bit set to one represents a CRB that is occupied or unavailable.
  /// A CRB is considered occupied if it is outside of the provided BWP CRB boundaries or if it is already allocated
  /// in at least one OFDM symbol of the provided OFDM symbol range.
  /// \param[in] scs Subcarrier spacing of interest.
  /// \param[in] crb_lims CRB limits used for the allocation.
  /// \param[in] symbols Range of OFDM symbols, where the search for available CRBs is carrier out.
  /// \return a CRB bitmap with bits set to one for unavailable CRBs.
  crb_bitmap used_crbs(subcarrier_spacing scs, crb_interval crb_lims, ofdm_symbol_range symbols) const;

  /// Checks whether all the provided symbol x RB range in the cell resource grid are set.
  /// \param grant contains the symbol x RB range to be tested.
  /// \return true if all symbols x RBs of grant are currently set in the resource grid.
  bool all_set(grant_info grant) const;
  bool all_set(subcarrier_spacing scs, ofdm_symbol_range ofdm_symbols, crb_interval crbs) const;

  const carrier_subslot_resource_grid& get_carrier_res_grid(subcarrier_spacing scs) const
  {
    return get_carrier(scs).subslot_rbs;
  }

private:
  friend struct cell_resource_allocator;

  struct carrier_resource_grid {
    /// Stores the sum of all symbol x CRB resources used for data and control grants.
    carrier_subslot_resource_grid subslot_rbs;

    explicit carrier_resource_grid(const scs_specific_carrier& carrier_cfg);
  };

  carrier_resource_grid&       get_carrier(subcarrier_spacing scs);
  const carrier_resource_grid& get_carrier(subcarrier_spacing scs) const;

  /// Carrier Resource Grids.
  std::vector<carrier_resource_grid>     carrier_grids;
  std::array<unsigned, NOF_NUMEROLOGIES> numerology_to_grid_idx;
};

/// Stores all the scheduled information relative to a {slot, cell}.
struct cell_slot_resource_allocator {
  const cell_configuration& cfg;

  /// Current slot that "this" slot resource grid refers to.
  slot_point slot;

  /// Saves grants allocated for the given slot and cell.
  sched_result result;

  /// Stores all the allocated RBs and symbols for DL.
  cell_slot_resource_grid dl_res_grid;

  /// Stores all the allocated RBs and symbols for UL.
  cell_slot_resource_grid ul_res_grid;

  explicit cell_slot_resource_allocator(const cell_configuration& cfg_);
  explicit cell_slot_resource_allocator(const cell_configuration&  cfg_,
                                        span<scs_specific_carrier> dl_scs_carriers,
                                        span<scs_specific_carrier> ul_scs_carriers);

  /// copies and moves are disabled to ensure pointer/reference validity.
  cell_slot_resource_allocator(const cell_slot_resource_allocator&)            = delete;
  cell_slot_resource_allocator(cell_slot_resource_allocator&&)                 = delete;
  cell_slot_resource_allocator& operator=(const cell_slot_resource_allocator&) = delete;
  cell_slot_resource_allocator& operator=(cell_slot_resource_allocator&&)      = delete;

  /// Sets new slot.
  void slot_indication(slot_point sl);
};

/// Circular Ring of cell_slot_resource_grid objects. This class manages the automatic resetting of
/// cell_slot_resource_grid objects, once they become old.
struct cell_resource_allocator {
  /// \brief Number of previous slot results to keep in history before they get deleted.
  ///
  /// Having access to past decisions is useful during the handling of error indications.
  static const size_t RING_MAX_HISTORY_SIZE = 8;
  /// Number of slots managed by this container.
  static const size_t RING_ALLOCATOR_SIZE = get_allocator_ring_size_gt_min(
      RING_MAX_HISTORY_SIZE + get_max_slot_ul_alloc_delay(NTN_CELL_SPECIFIC_KOFFSET_MAX));

  /// Cell configuration
  const cell_configuration& cfg;

  /// Maximum number of slots that can be allocated in advance.
  const unsigned max_dl_slot_alloc_delay;
  const unsigned max_ul_slot_alloc_delay;

  explicit cell_resource_allocator(const cell_configuration& cfg_);

  /// Indicate the processing of a new slot in the scheduler.
  void slot_indication(slot_point sl_tx);

  /// Cell index of the resource grid.
  du_cell_index_t cell_index() const { return cfg.cell_index; }

  /// Last slot indicated to the scheduler.
  slot_point slot_tx() const { return last_slot_ind; }

  /// Access resource allocator for a given {slot, cell}. The accessed slot corresponds to the latest slot indication
  /// provided to the cell_resource_grid_manager + slot_delay provided as argument.
  /// Given that slot_delay is unsigned, this class can only access the present and future slots.
  /// \param slot_delay delay in #slots added to the last slot indication value.
  /// \return the cell resource grid corresponding to the accessed slot.
  const cell_slot_resource_allocator& operator[](unsigned slot_delay) const
  {
    assert_valid_sl(slot_delay);
    slot_point                          sl_tx = last_slot_ind + slot_delay;
    const cell_slot_resource_allocator& r     = *slots[sl_tx.to_uint() % slots.size()];
    srsran_assert(r.slot == sl_tx, "Bad access to uninitialized cell_resource_grid");
    return r;
  }
  cell_slot_resource_allocator& operator[](unsigned slot_delay)
  {
    assert_valid_sl(slot_delay);
    slot_point                    sl_tx = last_slot_ind + slot_delay;
    cell_slot_resource_allocator& r     = *slots[sl_tx.to_uint() % slots.size()];
    srsran_assert(r.slot == sl_tx, "Bad access to uninitialized cell_resource_grid");
    return r;
  }

  cell_slot_resource_allocator&       operator[](slot_point slot) { return this->operator[](slot - last_slot_ind); }
  const cell_slot_resource_allocator& operator[](slot_point slot) const
  {
    return this->operator[](slot - last_slot_ind);
  }

  /// \brief Access a past slot decision made by the scheduler for the given cell.
  const cell_slot_resource_allocator* get_history(slot_point slot) const
  {
    int diff = last_slot_ind - slot;
    if (diff < 0 or diff >= static_cast<int>(RING_MAX_HISTORY_SIZE)) {
      return nullptr;
    }
    const cell_slot_resource_allocator& r = *slots[slot.to_uint() % slots.size()];
    srsran_assert(r.slot == slot, "Bad access to uninitialized cell_resource_grid");
    return &r;
  }

private:
  /// Ensure we are not overflowing the ring.
  void assert_valid_sl(unsigned slot_delay) const
  {
    srsran_sanity_check(slot_delay <= max_ul_slot_alloc_delay,
                        "The cell resource pool is too small for accessing a slot with delay: {}",
                        slot_delay);
  }

  /// The latest slot value indicated by the PHY to the MAC/scheduler.
  slot_point last_slot_ind;

  /// Circular pool of cell resource grids, where each entry represents a separate slot.
  std::vector<std::unique_ptr<cell_slot_resource_allocator>> slots;
};

} // namespace srsran

namespace fmt {

template <>
struct formatter<srsran::carrier_subslot_resource_grid> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::carrier_subslot_resource_grid& grid, FormatContext& ctx)
  {
    for (unsigned i = 0; i != srsran::NOF_OFDM_SYM_PER_SLOT_NORMAL_CP; ++i) {
      format_to(ctx.out(), "\n{}", grid.used_crbs({0, grid.nof_rbs()}, {i, i + 1}));
    }
    return ctx.out();
  }
};

template <>
struct formatter<srsran::cell_slot_resource_grid> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx)
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsran::cell_slot_resource_grid& grid, FormatContext& ctx)
  {
    auto scs_list = grid.active_scs();
    for (srsran::subcarrier_spacing scs : scs_list) {
      const auto& carrier_grid = grid.get_carrier_res_grid(scs);
      format_to(ctx.out(), "{}kHz: {}\n", scs_to_khz(scs), carrier_grid);
    }
    return ctx.out();
  }
};

} // namespace fmt