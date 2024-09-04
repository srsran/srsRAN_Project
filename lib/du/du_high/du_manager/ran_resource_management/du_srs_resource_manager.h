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

#include "srs_resource_generator.h"
#include "srsran/adt/optional.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/ran/srs/srs_configuration.h"

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
};

/// This class implements the MAX UL throughput policy for the SRS allocation. The SRS resources are allocated to
/// minimize the number of slots that contains the SRS resources; furthermore, within a given slot, the SRS resources
/// are allocated to minimize the number of symbols that are used for SRS. The drawback of this policy is that it can
/// increase the inter slot SRS interference among different UEs.
class du_srs_policy_max_ul_th : public du_srs_resource_manager
{
public:
  explicit du_srs_policy_max_ul_th(span<const du_cell_config> cell_cfg_list_);

  bool alloc_resources(cell_group_config& cell_grp_cfg) override;

  void dealloc_resources(cell_group_config& cell_grp_cfg) override;

private:
  struct cell_context {
    cell_context(const du_cell_config& cfg) : cell_cfg(cfg){};

    using pair_res_id_offset = std::pair<unsigned, unsigned>;

    // Returns the DU SRS resource with the given cell resource ID from the cell list of resources.
    std::vector<du_srs_resource>::const_iterator get_du_srs_res_cfg(unsigned cell_res_id)
    {
      return std::find_if(cell_srs_res_list.begin(),
                          cell_srs_res_list.end(),
                          [cell_res_id](const du_srs_resource& res) { return res.cell_res_id == cell_res_id; });
    }

    // Returns the best SRS resource ID and offset for this UE, according to the policy defined in this class.
    std::vector<pair_res_id_offset>::const_iterator find_optimal_ue_srs_resource();

    // Check if this SRS offset has already some SRS resources allocated, but can still host more.
    bool offset_used_not_full(unsigned offset) const
    {
      srsran_assert(offset < slot_resource_cnt.size(), "Offset out of range");
      return slot_resource_cnt[offset].first != 0 and
             slot_resource_cnt[offset].first < slot_resource_cnt[offset].second;
    }

    // Parameters that are common to all cell SRS resources.
    struct srs_cell_common {
      unsigned c_srs;
      unsigned freq_shift;
    };

    using pair_cnt_max = std::pair<unsigned, const unsigned>;

    const du_cell_config& cell_cfg;
    srs_cell_common       srs_common_params;
    // List of all SRS resources available to the cell; these resources can be allocated over to different UEs over
    // different offsets.
    std::vector<du_srs_resource> cell_srs_res_list;
    // List of SRS resource ID and offset that can be allocated to the cell's UEs.
    std::vector<pair_res_id_offset> srs_res_offset_free_list;
    // Counter of how many SRS resources (current counter, max_counter) that can be allocated in the same slot (offset).
    std::vector<pair_cnt_max> slot_resource_cnt;
  };

  // Contains the resources for the different cells of the DU.
  static_vector<cell_context, MAX_NOF_DU_CELLS> cells;
};

} // namespace srs_du
} // namespace srsran
