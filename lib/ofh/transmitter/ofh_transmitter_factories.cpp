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

#include "ofh_transmitter_factories.h"
#include "ofh_data_flow_cplane_scheduling_commands_impl.h"
#include "ofh_data_flow_cplane_scheduling_commands_task_dispatcher.h"
#include "ofh_data_flow_uplane_downlink_data_impl.h"
#include "ofh_data_flow_uplane_downlink_task_dispatcher.h"
#include "ofh_downlink_handler_impl.h"
#include "ofh_downlink_manager_broadcast_impl.h"
#include "ofh_downlink_manager_impl.h"
#include "ofh_transmitter_impl.h"
#include "ofh_uplane_fragment_size_calculator.h"
#include "srsran/ofh/compression/compression_factory.h"
#include "srsran/ofh/compression/iq_compressor.h"
#include "srsran/ofh/ecpri/ecpri_factories.h"
#include "srsran/ofh/ethernet/ethernet_factories.h"
#include "srsran/ofh/serdes/ofh_serdes_factories.h"

using namespace srsran;
using namespace ofh;

static std::unique_ptr<data_flow_cplane_scheduling_commands>
create_data_flow_cplane_sched(const transmitter_config&                         tx_config,
                              bool                                              static_compr_header_enabled,
                              srslog::basic_logger&                             logger,
                              std::shared_ptr<ether::eth_frame_pool>            frame_pool,
                              std::shared_ptr<uplink_cplane_context_repository> ul_cplane_context_repo,
                              std::shared_ptr<uplink_cplane_context_repository> prach_cplane_context_repo)
{
  data_flow_cplane_scheduling_commands_impl_config config;

  config.ru_nof_prbs =
      get_max_Nprb(bs_channel_bandwidth_to_MHz(tx_config.ru_working_bw), tx_config.scs, srsran::frequency_range::FR1);
  config.sector             = tx_config.sector;
  config.dl_compr_params    = tx_config.dl_compr_params;
  config.ul_compr_params    = tx_config.ul_compr_params;
  config.prach_compr_params = tx_config.prach_compr_params;
  config.cp                 = tx_config.cp;

  ether::vlan_frame_params ether_params;
  ether_params.eth_type        = ether::ECPRI_ETH_TYPE;
  ether_params.tci             = tx_config.tci_cp;
  ether_params.mac_dst_address = tx_config.mac_dst_address;
  ether_params.mac_src_address = tx_config.mac_src_address;

  data_flow_cplane_scheduling_commands_impl_dependencies dependencies;
  dependencies.logger                    = &logger;
  dependencies.ul_cplane_context_repo    = std::move(ul_cplane_context_repo);
  dependencies.prach_cplane_context_repo = std::move(prach_cplane_context_repo);
  dependencies.frame_pool                = std::move(frame_pool);
  dependencies.eth_builder   = (tx_config.tci_cp.has_value()) ? ether::create_vlan_frame_builder(ether_params)
                                                              : ether::create_frame_builder(ether_params);
  dependencies.ecpri_builder = ecpri::create_ecpri_packet_builder();
  dependencies.cp_builder    = (static_compr_header_enabled)
                                   ? ofh::create_ofh_control_plane_static_compression_message_builder()
                                   : ofh::create_ofh_control_plane_dynamic_compression_message_builder();

  return std::make_unique<data_flow_cplane_scheduling_commands_impl>(config, std::move(dependencies));
}

static std::unique_ptr<data_flow_uplane_downlink_data>
create_data_flow_uplane_data(const transmitter_config&              tx_config,
                             srslog::basic_logger&                  logger,
                             std::shared_ptr<ether::eth_frame_pool> frame_pool)
{
  data_flow_uplane_downlink_data_impl_config config;
  config.ru_nof_prbs =
      get_max_Nprb(bs_channel_bandwidth_to_MHz(tx_config.ru_working_bw), tx_config.scs, srsran::frequency_range::FR1);
  config.sector       = tx_config.sector;
  config.dl_eaxc      = tx_config.dl_eaxc;
  config.compr_params = tx_config.dl_compr_params;
  config.cp           = tx_config.cp;

  ether::vlan_frame_params ether_params;
  ether_params.eth_type        = ether::ECPRI_ETH_TYPE;
  ether_params.tci             = tx_config.tci_up;
  ether_params.mac_dst_address = tx_config.mac_dst_address;
  ether_params.mac_src_address = tx_config.mac_src_address;

  data_flow_uplane_downlink_data_impl_dependencies dependencies;
  dependencies.logger        = &logger;
  dependencies.frame_pool    = std::move(frame_pool);
  dependencies.eth_builder   = (tx_config.tci_up.has_value()) ? ether::create_vlan_frame_builder(ether_params)
                                                              : ether::create_frame_builder(ether_params);
  dependencies.ecpri_builder = ecpri::create_ecpri_packet_builder();

  const unsigned nof_prbs =
      get_max_Nprb(bs_channel_bandwidth_to_MHz(tx_config.bw), tx_config.scs, srsran::frequency_range::FR1);
  const double bw_scaling = 1.0 / (std::sqrt(nof_prbs * NOF_SUBCARRIERS_PER_RB));

  std::array<std::unique_ptr<ofh::iq_compressor>, ofh::NOF_COMPRESSION_TYPES_SUPPORTED> compressors;
  for (unsigned i = 0; i != ofh::NOF_COMPRESSION_TYPES_SUPPORTED; ++i) {
    compressors[i] =
        create_iq_compressor(static_cast<ofh::compression_type>(i), logger, tx_config.iq_scaling * bw_scaling);
  }
  dependencies.compressor_sel = ofh::create_iq_compressor_selector(std::move(compressors));

  dependencies.up_builder =
      (tx_config.is_downlink_static_compr_hdr_enabled)
          ? ofh::create_static_compr_method_ofh_user_plane_packet_builder(logger, *dependencies.compressor_sel)
          : ofh::create_dynamic_compr_method_ofh_user_plane_packet_builder(logger, *dependencies.compressor_sel);

  return std::make_unique<data_flow_uplane_downlink_data_impl>(config, std::move(dependencies));
}

static std::unique_ptr<downlink_manager>
create_downlink_manager(const transmitter_config&                         tx_config,
                        srslog::basic_logger&                             logger,
                        std::shared_ptr<ether::eth_frame_pool>            frame_pool,
                        std::shared_ptr<uplink_cplane_context_repository> ul_cp_context_repo,
                        std::shared_ptr<uplink_cplane_context_repository> prach_cp_context_repo,
                        task_executor&                                    executor)
{
  auto data_flow_cplane = std::make_unique<data_flow_cplane_downlink_task_dispatcher>(
      create_data_flow_cplane_sched(tx_config,
                                    tx_config.is_downlink_static_compr_hdr_enabled,
                                    logger,
                                    frame_pool,
                                    std::move(ul_cp_context_repo),
                                    std::move(prach_cp_context_repo)),
      executor,
      tx_config.sector);
  auto data_flow_uplane = std::make_unique<data_flow_uplane_downlink_task_dispatcher>(
      create_data_flow_uplane_data(tx_config, logger, frame_pool), executor, tx_config.sector);

  if (tx_config.downlink_broadcast) {
    downlink_handler_broadcast_impl_config dl_config;
    dl_config.dl_eaxc            = tx_config.dl_eaxc;
    dl_config.tdd_config         = tx_config.tdd_config;
    dl_config.cp                 = tx_config.cp;
    dl_config.scs                = tx_config.scs;
    dl_config.dl_processing_time = tx_config.dl_processing_time;
    dl_config.tx_timing_params   = tx_config.tx_timing_params;
    dl_config.sector             = tx_config.sector;

    downlink_handler_broadcast_impl_dependencies dl_dependencies;
    dl_dependencies.logger           = &logger;
    dl_dependencies.data_flow_cplane = std::move(data_flow_cplane);
    dl_dependencies.data_flow_uplane = std::move(data_flow_uplane);
    dl_dependencies.frame_pool       = frame_pool;

    return std::make_unique<downlink_manager_broadcast_impl>(dl_config, std::move(dl_dependencies));
  }

  downlink_handler_impl_config dl_config;
  dl_config.dl_eaxc            = tx_config.dl_eaxc;
  dl_config.tdd_config         = tx_config.tdd_config;
  dl_config.cp                 = tx_config.cp;
  dl_config.scs                = tx_config.scs;
  dl_config.dl_processing_time = tx_config.dl_processing_time;
  dl_config.tx_timing_params   = tx_config.tx_timing_params;
  dl_config.sector             = tx_config.sector;

  downlink_handler_impl_dependencies dl_dependencies;
  dl_dependencies.logger           = &logger;
  dl_dependencies.data_flow_cplane = std::move(data_flow_cplane);
  dl_dependencies.data_flow_uplane = std::move(data_flow_uplane);
  dl_dependencies.frame_pool       = frame_pool;

  return std::make_unique<downlink_manager_impl>(dl_config, std::move(dl_dependencies));
}

static std::shared_ptr<ether::eth_frame_pool> create_eth_frame_pool(const transmitter_config& tx_config,
                                                                    srslog::basic_logger&     logger)
{
  ether::vlan_frame_params ether_params;
  auto eth_builder   = (tx_config.tci_up || tx_config.tci_cp) ? ether::create_vlan_frame_builder(ether_params)
                                                              : ether::create_frame_builder(ether_params);
  auto ecpri_builder = ecpri::create_ecpri_packet_builder();

  std::array<std::unique_ptr<ofh::iq_compressor>, ofh::NOF_COMPRESSION_TYPES_SUPPORTED> compressors;
  for (unsigned i = 0; i != ofh::NOF_COMPRESSION_TYPES_SUPPORTED; ++i) {
    compressors[i] = create_iq_compressor(ofh::compression_type::none, logger);
  }
  auto compressor_sel = ofh::create_iq_compressor_selector(std::move(compressors));

  std::unique_ptr<uplane_message_builder> uplane_builder =
      (tx_config.is_downlink_static_compr_hdr_enabled)
          ? ofh::create_static_compr_method_ofh_user_plane_packet_builder(logger, *compressor_sel)
          : ofh::create_dynamic_compr_method_ofh_user_plane_packet_builder(logger, *compressor_sel);

  units::bytes headers_size = eth_builder->get_header_size() +
                              ecpri_builder->get_header_size(ecpri::message_type::iq_data) +
                              uplane_builder->get_header_size(tx_config.dl_compr_params);

  unsigned nof_prbs =
      get_max_Nprb(bs_channel_bandwidth_to_MHz(tx_config.ru_working_bw), tx_config.scs, srsran::frequency_range::FR1);

  unsigned nof_frames_per_symbol = ofh_uplane_fragment_size_calculator::calculate_nof_segments(
      tx_config.mtu_size, nof_prbs, tx_config.dl_compr_params, headers_size);

  return std::make_shared<ether::eth_frame_pool>(tx_config.mtu_size, nof_frames_per_symbol);
}

static transmitter_impl_dependencies
resolve_transmitter_dependencies(const transmitter_config&                         tx_config,
                                 srslog::basic_logger&                             logger,
                                 task_executor&                                    tx_executor,
                                 task_executor&                                    downlink_executor,
                                 std::unique_ptr<ether::gateway>                   eth_gateway,
                                 std::shared_ptr<prach_context_repository>         prach_context_repo,
                                 std::shared_ptr<uplink_context_repository>        ul_slot_context_repo,
                                 std::shared_ptr<uplink_cplane_context_repository> ul_cp_context_repo,
                                 std::shared_ptr<uplink_cplane_context_repository> prach_cp_context_repo)
{
  transmitter_impl_dependencies dependencies;

  dependencies.logger   = &logger;
  dependencies.executor = &tx_executor;

  auto frame_pool = create_eth_frame_pool(tx_config, logger);

  dependencies.dl_manager = create_downlink_manager(
      tx_config, logger, frame_pool, ul_cp_context_repo, prach_cp_context_repo, downlink_executor);

  dependencies.data_flow = std::make_unique<data_flow_cplane_downlink_task_dispatcher>(
      create_data_flow_cplane_sched(tx_config,
                                    tx_config.is_uplink_static_compr_hdr_enabled,
                                    logger,
                                    frame_pool,
                                    std::move(ul_cp_context_repo),
                                    std::move(prach_cp_context_repo)),
      downlink_executor,
      tx_config.sector);

  dependencies.ul_slot_repo  = std::move(ul_slot_context_repo);
  dependencies.ul_prach_repo = std::move(prach_context_repo);

  dependencies.eth_gateway = std::move(eth_gateway);

  dependencies.frame_pool = frame_pool;

  return dependencies;
}

std::unique_ptr<transmitter>
srsran::ofh::create_transmitter(const transmitter_config&                         transmitter_cfg,
                                srslog::basic_logger&                             logger,
                                task_executor&                                    tx_executor,
                                task_executor&                                    downlink_executor,
                                std::unique_ptr<ether::gateway>                   eth_gateway,
                                std::shared_ptr<prach_context_repository>         prach_context_repo,
                                std::shared_ptr<uplink_context_repository>        ul_slot_context_repo,
                                std::shared_ptr<uplink_cplane_context_repository> ul_cp_context_repo,
                                std::shared_ptr<uplink_cplane_context_repository> prach_cp_context_repo)
{
  return std::make_unique<transmitter_impl>(transmitter_cfg,
                                            resolve_transmitter_dependencies(transmitter_cfg,
                                                                             logger,
                                                                             tx_executor,
                                                                             downlink_executor,
                                                                             std::move(eth_gateway),
                                                                             std::move(prach_context_repo),
                                                                             std::move(ul_slot_context_repo),
                                                                             std::move(ul_cp_context_repo),
                                                                             std::move(prach_cp_context_repo)));
}
