/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

#include "../support/uplink_cplane_context_repository.h"
#include "ofh_data_flow_cplane_scheduling_commands.h"
#include "sequence_identifier_generator.h"
#include "srsran/ofh/ecpri/ecpri_packet_builder.h"
#include "srsran/ofh/ethernet/ethernet_frame_pool.h"
#include "srsran/ofh/ethernet/vlan_ethernet_frame_builder.h"
#include "srsran/ofh/serdes/ofh_cplane_message_builder.h"

namespace srsran {
namespace ofh {

/// Open Fronthaul Control-Plane scheduling and beamforming commands data flow implementation configuration.
struct data_flow_cplane_scheduling_commands_impl_config {
  /// Logger.
  srslog::basic_logger* logger = nullptr;
  /// RU bandwidth in PRBs.
  unsigned ru_nof_prbs;
  /// VLAN frame parameters.
  ether::vlan_frame_params vlan_params;
  /// Downlink compression parameters.
  ru_compression_params dl_compr_params;
  /// Uplink compression parameters.
  ru_compression_params ul_compr_params;
  /// PRACH compression parameters.
  ru_compression_params prach_compr_params;
  /// Uplink Control-Plane context repository.
  std::shared_ptr<uplink_cplane_context_repository> ul_cplane_context_repo;
  /// Ethernet frame pool.
  std::shared_ptr<ether::eth_frame_pool> frame_pool;
  /// VLAN frame builder.
  std::unique_ptr<ether::vlan_frame_builder> eth_builder;
  /// eCPRI packet builder.
  std::unique_ptr<ecpri::packet_builder> ecpri_builder;
  /// Control-Plane message builder.
  std::unique_ptr<cplane_message_builder> cp_builder;
};

/// Open Fronthaul Control-Plane scheduling and beamforming commands data flow implementation.
class data_flow_cplane_scheduling_commands_impl : public data_flow_cplane_scheduling_commands
{
public:
  explicit data_flow_cplane_scheduling_commands_impl(data_flow_cplane_scheduling_commands_impl_config&& config);

  // See interface for documentation.
  void enqueue_section_type_1_message(const data_flow_cplane_type_1_context& context) override;

  // See interface for documentation.
  void enqueue_section_type_3_prach_message(const data_flow_cplane_scheduling_prach_context& context) override;

private:
  srslog::basic_logger&                             logger;
  const unsigned                                    ru_nof_prbs;
  const ru_compression_params                       dl_compr_params;
  const ru_compression_params                       ul_compr_params;
  const ru_compression_params                       prach_compr_params;
  const ether::vlan_frame_params                    vlan_params;
  sequence_identifier_generator                     cp_dl_seq_gen;
  sequence_identifier_generator                     cp_ul_seq_gen;
  std::shared_ptr<uplink_cplane_context_repository> ul_cplane_context_repo_ptr;
  std::shared_ptr<ether::eth_frame_pool>            frame_pool_ptr;
  uplink_cplane_context_repository&                 ul_cplane_context_repo;
  ether::eth_frame_pool&                            frame_pool;
  std::unique_ptr<ether::vlan_frame_builder>        eth_builder;
  std::unique_ptr<ecpri::packet_builder>            ecpri_builder;
  std::unique_ptr<cplane_message_builder>           cp_builder;
};

} // namespace ofh
} // namespace srsran
