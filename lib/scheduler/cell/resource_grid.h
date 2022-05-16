/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RESOURCE_GRID_H
#define SRSGNB_RESOURCE_GRID_H

#include "../sched_prb.h"
#include "cell_configuration.h"
#include "srsgnb/adt/circular_array.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/scheduler/mac_scheduler.h"

namespace srsgnb {

/// Parameters of a SCH grant allocation within a BWP.
struct bwp_sch_grant_info {
  const bwp_configuration* bwp_cfg;
  ofdm_symbol_range        symbols;
  /// PRBs with BWP.
  prb_interval prbs;

  bwp_sch_grant_info(const bwp_configuration& bwp_, ofdm_symbol_range symbols_, prb_interval prbs_) :
    bwp_cfg(&bwp_), symbols(symbols_), prbs(prbs_)
  {
    srsran_sanity_check(symbols.stop() <= 14, "OFDM symbols do not fit slot");
    srsran_sanity_check(prbs.stop() <= bwp_cfg->prbs.length(), "PRBs={} do not fit BWP={}", prbs, bwp_cfg->prbs);
  }
};

/// Convert PRBs within a BWP into Common RBs, which use pointA as reference point.
inline prb_interval prb_to_crb(const bwp_configuration& bwp_cfg, prb_interval prbs)
{
  prbs.displace_by(bwp_cfg.prbs.start());
  srsran_sanity_check(
      bwp_cfg.prbs.contains(prbs), "Mismatch between PRB interval={} and BWP lims={}", prbs, bwp_cfg.prbs);
  return prbs;
}

/// Convert CRBs to PRBs within a BWP.
inline prb_interval crb_to_prb(const bwp_configuration& bwp_cfg, prb_interval crbs)
{
  crbs.displace_by(bwp_cfg.prbs.start());
  srsran_sanity_check(
      bwp_cfg.prbs.contains(crbs), "Mismatch between PRB interval={} and BWP lims={}", crbs, bwp_cfg.prbs);
  return crbs;
}

/// Parameters of a grant allocation in the cell resource grid.
struct grant_info {
  enum class channel { cch, sch, ssb };
  channel            ch;
  subcarrier_spacing scs;
  ofdm_symbol_range  symbols;
  /// Common RBs of the grant. RB==0 corresponds to the RB that overlaps with the pointA.
  prb_interval crbs;

  grant_info(grant_info::channel ch_, subcarrier_spacing scs_, ofdm_symbol_range symbols_, prb_interval crbs_) :
    ch(ch_), scs(scs_), symbols(symbols_), crbs(crbs_)
  {}
  grant_info(const bwp_sch_grant_info& grant) :
    ch(channel::sch), scs(grant.bwp_cfg->scs), symbols(grant.symbols), crbs(prb_to_crb(*grant.bwp_cfg, grant.prbs))
  {}
};

inline prb_interval get_carrier_rb_dims(const scs_specific_carrier& carrier_cfg)
{
  return prb_interval{carrier_cfg.offset_to_carrier, carrier_cfg.offset_to_carrier + carrier_cfg.carrier_bandwidth};
}

/// \brief Represents the Symbol x RB resource grid of a DL/UL Carrier. The number of RBs of the grid will depend on
/// the carrier bandwidth and numerology used.
class carrier_subslot_resource_grid
{
public:
  explicit carrier_subslot_resource_grid(const scs_specific_carrier& carrier_cfg);

  subcarrier_spacing scs() const { return carrier_cfg.scs; }
  unsigned           nof_prbs() const { return carrier_cfg.carrier_bandwidth; }
  unsigned           offset() const { return carrier_cfg.offset_to_carrier; }
  prb_interval       rb_dims() const { return get_carrier_rb_dims(carrier_cfg); }

  /// Clearer Carrier Resource Grid.
  void clear();

  /// Allocates the symbol x PRB range in the carrier resource grid.
  /// \param symbols OFDM symbol interval of the allocation. Interval must fall within [0, 14).
  /// \param prbs PRB interval, where PRB=0 corresponds to the PRB closest to pointA.
  void fill(ofdm_symbol_range symbols, prb_interval prbs);

  /// Checks whether the provided symbol x PRB range collides with any other allocation in the carrier resource grid.
  /// \param symbols OFDM symbol interval of the allocation. Interval must fall within [0, 14).
  /// \param prbs PRB interval, where PRB=0 corresponds to the PRB closest to pointA.
  /// \return true if a collision was detected. False otherwise.
  bool collides(ofdm_symbol_range symbols, prb_interval prbs) const;

private:
  using slot_rb_bitmap = bounded_bitset<NOF_OFDM_SYM_PER_SLOT_NORMAL_CP * MAX_NOF_PRBS>;

  scs_specific_carrier carrier_cfg;
  slot_rb_bitmap       slot_rbs;
};

/// \brief Manages the allocation of RBs in the resource grid of a given service cell for a given slot. This class
/// manages multiple carriers of different numerologies associated to the same cell.
class cell_slot_resource_grid
{
public:
  explicit cell_slot_resource_grid(span<const scs_specific_carrier> scs_carriers);

  /// Reset the resource grid to empty.
  void clear();

  /// Allocates the symbol x RB range in the cell resource grid.
  /// \param prbs PRB interval of the allocation. PRB=0 corresponds to the first PRB of the BWP.
  /// \param symbols OFDM symbol interval of the allocation.
  void fill(grant_info grant);

  /// Checks whether the provided symbol x RB range collides with any other allocation in the cell resource grid.
  /// \param grant contains the symbol x RB range whose available we want to check.
  /// \return true if at least one symbol x RB of grant is currently occupied in the resource grid.
  bool collides(grant_info grant) const;
  bool collides(subcarrier_spacing scs, ofdm_symbol_range ofdm_symbols, prb_interval crbs) const;

  prb_bitmap sch_prbs(const bwp_configuration& bwp_cfg) const;

private:
  friend struct cell_resource_allocator;

  struct carrier_resource_grid {
    /// Stores the sum of all RBs used for SCH grants.
    prb_bitmap sch_prbs;

    /// Stores the sum of all symbol x RB resources used for data and control grants.
    carrier_subslot_resource_grid subslot_prbs;

    /// List of overlapping carrier resource grids.
    std::vector<std::pair<carrier_resource_grid*, bool> > overlapped_carrier_slots;

    explicit carrier_resource_grid(const scs_specific_carrier& carrier_cfg);
  };

  carrier_resource_grid&       get_carrier(const subcarrier_spacing scs);
  const carrier_resource_grid& get_carrier(const subcarrier_spacing scs) const;

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
  cell_slot_resource_allocator(const cell_slot_resource_allocator&) = delete;
  cell_slot_resource_allocator(cell_slot_resource_allocator&&)      = delete;
  cell_slot_resource_allocator& operator=(const cell_slot_resource_allocator&) = delete;
  cell_slot_resource_allocator& operator=(cell_slot_resource_allocator&&) = delete;

  /// Sets new slot.
  void slot_indication(slot_point sl);

private:
  /// Clears all allocations.
  void clear();
};

/// Circular Ring of cell_slot_resource_grid objects. This class manages the automatic resetting of
/// cell_slot_resource_grid objects, once they become old.
struct cell_resource_allocator {
  /// Number of subframes managed by this container.
  static const size_t GRID_NOF_SUBFRAMES = 20;

  /// Highest difference between a slot and the last slot indication that avoids overflowing the ring pool.
  static const int MAXIMUM_SLOT_DIFF = GRID_NOF_SUBFRAMES / 2;

  /// Cell configuration
  const cell_configuration& cfg;

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

private:
  /// Ensure we are not overflowing the ring.
  void assert_valid_sl(unsigned slot_delay) const
  {
    srsran_sanity_check(slot_delay < MAXIMUM_SLOT_DIFF,
                        "The cell resource pool is too small for accessing a slot with delay: {}",
                        slot_delay);
  }

  /// The latest slot value indicated by the PHY to the MAC/scheduler.
  slot_point last_slot_ind;

  /// Circular pool of cell resource grids, where each entry represents a separate slot.
  std::vector<std::unique_ptr<cell_slot_resource_allocator> > slots;
};

} // namespace srsgnb

#endif // SRSGNB_RESOURCE_GRID_H
