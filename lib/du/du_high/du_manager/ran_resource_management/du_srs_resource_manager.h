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

#include "srs_resource_generator.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/ran/srs/srs_bandwidth_configuration.h"

namespace srsran {
namespace srs_du {

struct cell_group_config;

/// This abstract class defines the methods that the DU SRS resource manager must implement. The implementation of this
/// class defines different policies for the SRS allocation.
class du_srs_resource_manager
{
public:
  virtual ~du_srs_resource_manager() = default;

  /// \brief Allocate SRS resources for a given UE. The resources are stored in the UE's cell group config.
  /// The function allocates the UE the resources from a common pool.
  /// \return true if allocation was successful.
  virtual bool alloc_resources(cell_group_config& cell_grp_cfg) = 0;

  /// \brief Deallocate the SRS resources for a given UE and return the used resource to the common pool.
  virtual void dealloc_resources(cell_group_config& cell_grp_cfg) = 0;

  /// Gets the current number of free SRS resource ID and offset pairs.
  virtual unsigned get_nof_srs_free_res_offsets(du_cell_index_t cell_idx) const = 0;
};

/// This class implements the MAX UL throughput policy for the SRS allocation. The SRS resources are allocated with the
/// objective to minimize the number of slots and symbols that contain the SRS resources, to reduce as much as possible
/// the slots and symbols resources taken from the PUSCH. The drawback of this policy is that it can increase the
/// inter-slot SRS interference among different UEs.
class du_srs_policy_max_ul_rate : public du_srs_resource_manager
{
public:
  explicit du_srs_policy_max_ul_rate(span<const du_cell_config> cell_cfg_list_);

  /// The SRS resources are allocate according to the following policy:
  /// - Give priority to resources that are placed on partially-UL slots, first.
  /// - Then, give priority to SRS resources that is placed on the symbol interval (i.e, the symbols interval used by an
  /// SRS resource) closest to the end of the slot.
  /// - If a symbol interval on a particular slot is already used and not completely full, then give priority to this
  /// symbol interval over any other symbol intervals on the same or on different slots.
  bool alloc_resources(cell_group_config& cell_grp_cfg) override;

  void dealloc_resources(cell_group_config& cell_grp_cfg) override;

  unsigned get_nof_srs_free_res_offsets(du_cell_index_t cell_idx) const override
  {
    return cells[cell_idx].srs_res_offset_free_list.size();
  }

private:
  struct cell_context {
    cell_context(const du_cell_config& cfg);

    // Returns the DU SRS resource with the given cell resource ID from the cell list of resources.
    std::vector<du_srs_resource>::const_iterator get_du_srs_res_cfg(unsigned cell_res_id)
    {
      if (cell_res_id >= cell_srs_res_list.size() or cell_srs_res_list[cell_res_id].cell_res_id != cell_res_id) {
        srsran_assertion_failure("Cell resource ID out of range or invalid");
        return cell_srs_res_list.end();
      }
      return cell_srs_res_list.cbegin() + cell_res_id;
    }

    using pair_res_id_offset = std::pair<unsigned, unsigned>;

    // Returns the best SRS resource ID and offset for this UE, according to the policy defined in this class.
    std::vector<pair_res_id_offset>::const_iterator find_optimal_ue_srs_resource();

    // Check if this SRS offset has already some SRS resources allocated at a given symbol interval, but can still host
    // more.
    bool offset_interval_used_not_full(unsigned offset) const
    {
      // The counter of SRS resources counts how many resources have been allocated in the current slot (offset).
      // If the counter is 0, then the offset is free.
      // If the counter is a non-zero multiple (i.e, N) of nof_res_per_symb_interval, then the symbol interval of index
      // N-1 has been filled completely and the next symbol interval of index N is free. In any other case, the symbol
      // interval N-1 is partially filled.
      srsran_assert(offset < slot_resource_cnt.size(), "Offset out of range");
      return slot_resource_cnt[offset] % nof_res_per_symb_interval != 0U;
    }

    // Parameters that are common to all cell SRS resources.
    struct srs_cell_common {
      unsigned c_srs;
      unsigned freq_shift;
    };

    // Maximum number of SRS resources that can be generated in a cell.
    // [Implementation-defined] We assume each UE has one and only one resource.
    static const unsigned                        max_nof_srs_res = MAX_NOF_DU_UES;
    const du_cell_config&                        cell_cfg;
    const std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common;
    // Default SRS configuration for the cell.
    const srs_config default_srs_cfg;
    srs_cell_common  srs_common_params;
    // List of all SRS resources available to the cell; these resources can be allocated over to different UEs over
    // different offsets.
    std::vector<du_srs_resource> cell_srs_res_list;
    // List of SRS resource ID and offset that can be allocated to the cell's UEs.
    std::vector<pair_res_id_offset> srs_res_offset_free_list;
    unsigned                        nof_res_per_symb_interval = 0;
    // Counter of how many SRS resources that are allocated per slot (offset).
    std::vector<unsigned> slot_resource_cnt;
  };

  // Contains the resources for the different cells of the DU.
  static_vector<cell_context, MAX_NOF_DU_CELLS> cells;
};

} // namespace srs_du
} // namespace srsran
