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

#include "../lib/du_manager/ran_resource_management/du_pucch_resource_manager.h"
#include "srsran/du/du_cell_config.h"

namespace srsran {

/// Provides a wrapper for \ref du_pucch_resource_manager to build the PUCCH dedicated configuration for UEs to be used
/// in tests.
class pucch_res_builder_test_helper
{
public:
  /// If variables needed to build \ref du_pucch_resource_manager are not available at the time of construction, use the
  /// default constructor and then call \ref setup.
  explicit pucch_res_builder_test_helper();
  /// Use this constructor if the variables needed to build \ref du_pucch_resource_manager are available at the time of
  /// construction. Do not call \ref setup after using this constructor.
  explicit pucch_res_builder_test_helper(const bwp_uplink_common&          init_ul_bwp,
                                         optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common,
                                         const serving_cell_config&        base_ue_cfg,
                                         const pucch_builder_params&       pucch_cfg = {});

  /// Initializes the \ref du_pucch_resource_manager after this class has been constructed with def. ctor.
  /// \remark This function should be called if the du_pucch_resource_manager has not initialized through this class's
  /// constructor.
  void setup(const bwp_uplink_common&          init_ul_bwp,
             optional<tdd_ul_dl_config_common> tdd_ul_dl_cfg_common,
             const serving_cell_config&        base_ue_cfg,
             const pucch_builder_params&       pucch_cfg = {});

  /// Build a new UE's PUCCH config (embedded in \ref serving_cell_config) and add this to the
  /// \ref du_pucch_resource_manager to keep track of which cell PUCCH resources have been used.
  bool add_build_new_ue_pucch_cfg(serving_cell_config& serv_cell_cfg);

private:
  optional<srs_du::du_pucch_resource_manager> pucch_res_mgr;
  constexpr static unsigned                   max_pucch_grants_per_slot = 64;
};

} // namespace srsran
