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

#include "du_ue_resource_config.h"

namespace srsran {

struct scheduler_expert_config;

namespace srs_du {

struct ue_capability_summary {
  bool pdsch_qam256_supported = false;
  bool pusch_qam256_supported = false;
};

expected<ue_capability_summary, std::string> decode_ue_nr_cap_container(const byte_buffer& ue_cap_container);

class ue_capability_manager
{
public:
  explicit ue_capability_manager(const scheduler_expert_config& scheduler_cfg, srslog::basic_logger& logger_);

  void update(du_ue_resource_config& ue_res_cfg, const byte_buffer& ue_cap_rat_list);

private:
  bool decode_ue_capability_list(const byte_buffer& ue_cap_rat_list);

  const scheduler_expert_config& scheduler_cfg;
  srslog::basic_logger&          logger;

  std::optional<ue_capability_summary> ue_caps;
};

} // namespace srs_du
} // namespace srsran