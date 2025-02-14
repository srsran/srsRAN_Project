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

#include "logical_channel_list_config.h"
#include "srsran/adt/slotted_vector.h"
#include "srsran/ran/du_types.h"
#include "srsran/scheduler/config/bwp_configuration.h"

namespace srsran {

struct sched_ue_config_request;

struct bwp_pdcch_config {
  slotted_id_vector<coreset_id, coreset_configuration>           coresets;
  slotted_id_vector<search_space_id, search_space_configuration> search_spaces;

  bool operator==(const bwp_pdcch_config& other) const
  {
    return coresets == other.coresets and search_spaces == other.search_spaces;
  }
};
using bwp_pdcch_config_ptr = config_ptr<bwp_pdcch_config>;

struct dl_bwp_config {
  config_ptr<bwp_pdcch_config> pdcch;

  bool operator==(const dl_bwp_config& other) const { return pdcch == other.pdcch; }
};

using dl_bwp_config_ptr      = config_ptr<dl_bwp_config>;
using dl_bwp_config_list     = slotted_id_vector<bwp_id_t, dl_bwp_config_ptr>;
using dl_bwp_config_list_ptr = config_ptr<dl_bwp_config_list>;

struct ue_cell_creation_params {
  slotted_id_vector<bwp_id_t, dl_bwp_config_ptr> dl_bwps;
};

struct ue_creation_params {
  const sched_ue_config_request&                              cfg_req;
  logical_channel_config_list_ptr                             lc_ch_list;
  slotted_id_vector<du_cell_index_t, ue_cell_creation_params> cells;
};

struct ue_reconfig_params {
  const sched_ue_config_request&                              cfg_req;
  std::optional<logical_channel_config_list_ptr>              lc_ch_list;
  slotted_id_vector<du_cell_index_t, ue_cell_creation_params> cells;
};

} // namespace srsran
