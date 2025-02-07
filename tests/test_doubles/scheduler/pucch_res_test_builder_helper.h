/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "../lib/du/du_high/du_manager/ran_resource_management/du_pucch_resource_manager.h"
#include "srsran/du/du_cell_config.h"

namespace srsran {

class cell_configuration;

/// Provides a wrapper for \ref du_pucch_resource_manager to build the PUCCH dedicated configuration for UEs to be used
/// in tests.
class pucch_res_builder_test_helper
{
public:
  /// If variables needed to build \ref du_pucch_resource_manager are not available at the time of construction, use the
  /// default constructor and then call \ref setup.
  explicit pucch_res_builder_test_helper();

  explicit pucch_res_builder_test_helper(const cell_configuration&   cell_cfg,
                                         const pucch_builder_params& pucch_cfg = {});

  explicit pucch_res_builder_test_helper(const bwp_uplink_common&               init_ul_bwp,
                                         std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common,
                                         const pucch_builder_params&            pucch_cfg = {});

  /// Initializes the variables after this class has been constructed with default ctor.
  /// \remark This function won't do anything if the class was constructed with the non-default constructor.
  void setup(const bwp_uplink_common&               init_ul_bwp,
             std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common,
             const pucch_builder_params&            pucch_cfg = {});
  void setup(const cell_configuration& cell_cfg, const pucch_builder_params& pucch_cfg = {});

  /// Build a new UE's PUCCH config (embedded in \ref serving_cell_config) and add this to the
  /// \ref du_pucch_resource_manager to keep track of which cell PUCCH resources have been used.
  bool add_build_new_ue_pucch_cfg(serving_cell_config& serv_cell_cfg);

private:
  // Build the \ref du_pucch_resource_manager with the given configuration. This function is called only once, at the
  // time the firt UE is added.
  void init_pucch_res_mgr(const serving_cell_config& base_ue_cfg);

  struct pucch_res_builder_info {
    const bwp_uplink_common                init_ul_bwp;
    std::optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common;
    pucch_builder_params                   pucch_cfg;
  };

  std::optional<pucch_res_builder_info>            required_info;
  std::optional<srs_du::du_pucch_resource_manager> pucch_res_mgr;
  static constexpr unsigned                        max_pucch_grants_per_slot = 64;
};

} // namespace srsran
