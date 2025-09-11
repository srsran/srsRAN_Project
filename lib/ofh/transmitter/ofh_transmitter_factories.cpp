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
#include "ofh_data_flow_cplane_scheduling_commands_metrics_decorator.h"
#include "ofh_data_flow_cplane_scheduling_commands_task_dispatcher.h"
#include "ofh_data_flow_uplane_downlink_data_impl.h"
#include "ofh_data_flow_uplane_downlink_data_metrics_decorator.h"
#include "ofh_data_flow_uplane_downlink_task_dispatcher.h"
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

  frequency_range freq_range =
      (tx_config.scs > subcarrier_spacing::kHz60) ? frequency_range::FR2 : frequency_range::FR1;

  config.ru_nof_prbs = get_max_Nprb(bs_channel_bandwidth_to_MHz(tx_config.ru_working_bw), tx_config.scs, freq_range);
  config.sector      = tx_config.sector;
  config.dl_compr_params       = tx_config.dl_compr_params;
  config.ul_compr_params       = tx_config.ul_compr_params;
  config.prach_compr_params    = tx_config.prach_compr_params;
  config.cp                    = tx_config.cp;
  config.c_plane_prach_fft_len = tx_config.c_plane_prach_fft_len;

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
  dependencies.eth_builder =
      (tx_config.tci_cp) ? ether::create_vlan_frame_builder(ether_params) : ether::create_frame_builder(ether_params);
  dependencies.ecpri_builder = ecpri::create_ecpri_packet_builder();
  dependencies.cp_builder    = (static_compr_header_enabled)
                                   ? create_ofh_control_plane_static_compression_message_builder()
                                   : create_ofh_control_plane_dynamic_compression_message_builder();

  auto data_flow_cplane = std::make_unique<data_flow_cplane_scheduling_commands_impl>(config, std::move(dependencies));
  if (!tx_config.are_metrics_enabled) {
    return data_flow_cplane;
  }

  return std::make_unique<data_flow_cplane_metrics_decorator>(std::move(data_flow_cplane));
}

static std::unique_ptr<data_flow_uplane_downlink_data>
create_data_flow_uplane_data(const transmitter_config&              tx_config,
                             srslog::basic_logger&                  logger,
                             std::shared_ptr<ether::eth_frame_pool> frame_pool)
{
  frequency_range freq_range =
      (tx_config.scs > subcarrier_spacing::kHz60) ? frequency_range::FR2 : frequency_range::FR1;

  data_flow_uplane_downlink_data_impl_config config;
  config.ru_nof_prbs  = get_max_Nprb(bs_channel_bandwidth_to_MHz(tx_config.ru_working_bw), tx_config.scs, freq_range);
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
  dependencies.logger     = &logger;
  dependencies.frame_pool = std::move(frame_pool);
  dependencies.eth_builder =
      (tx_config.tci_up) ? ether::create_vlan_frame_builder(ether_params) : ether::create_frame_builder(ether_params);
  dependencies.ecpri_builder = ecpri::create_ecpri_packet_builder();

  std::array<std::unique_ptr<iq_compressor>, NOF_COMPRESSION_TYPES_SUPPORTED> compressors;
  for (unsigned i = 0; i != NOF_COMPRESSION_TYPES_SUPPORTED; ++i) {
    compressors[i] = create_iq_compressor(static_cast<compression_type>(i), logger, tx_config.iq_scaling);
  }
  dependencies.compressor_sel = create_iq_compressor_selector(std::move(compressors));

  dependencies.up_builder =
      (tx_config.is_downlink_static_compr_hdr_enabled)
          ? create_static_compr_method_ofh_user_plane_packet_builder(logger, *dependencies.compressor_sel)
          : create_dynamic_compr_method_ofh_user_plane_packet_builder(logger, *dependencies.compressor_sel);

  auto data_flow_uplane = std::make_unique<data_flow_uplane_downlink_data_impl>(config, std::move(dependencies));
  if (!tx_config.are_metrics_enabled) {
    return data_flow_uplane;
  }

  return std::make_unique<data_flow_uplane_downlink_metrics_decorator>(std::move(data_flow_uplane));
}

static std::shared_ptr<ether::eth_frame_pool> create_eth_frame_pool(const transmitter_config& tx_config,
                                                                    srslog::basic_logger&     logger,
                                                                    ofh::message_type         type,
                                                                    ofh::data_direction       direction,
                                                                    bool calculate_nof_frames_per_symbol = true)
{
  frequency_range freq_range =
      (tx_config.scs > subcarrier_spacing::kHz60) ? frequency_range::FR2 : frequency_range::FR1;

  ether::vlan_frame_params ether_params;
  auto eth_builder   = (tx_config.tci_up || tx_config.tci_cp) ? ether::create_vlan_frame_builder(ether_params)
                                                              : ether::create_frame_builder(ether_params);
  auto ecpri_builder = ecpri::create_ecpri_packet_builder();

  std::array<std::unique_ptr<iq_compressor>, NOF_COMPRESSION_TYPES_SUPPORTED> compressors;
  for (unsigned i = 0; i != NOF_COMPRESSION_TYPES_SUPPORTED; ++i) {
    compressors[i] = create_iq_compressor(compression_type::none, logger);
  }
  auto compressor_sel = create_iq_compressor_selector(std::move(compressors));

  std::unique_ptr<uplane_message_builder> uplane_builder =
      (tx_config.is_downlink_static_compr_hdr_enabled)
          ? create_static_compr_method_ofh_user_plane_packet_builder(logger, *compressor_sel)
          : create_dynamic_compr_method_ofh_user_plane_packet_builder(logger, *compressor_sel);

  units::bytes headers_size = eth_builder->get_header_size() +
                              ecpri_builder->get_header_size(ecpri::message_type::iq_data) +
                              uplane_builder->get_header_size(tx_config.dl_compr_params);

  unsigned nof_prbs = get_max_Nprb(bs_channel_bandwidth_to_MHz(tx_config.ru_working_bw), tx_config.scs, freq_range);

  unsigned nof_frames_per_symbol = 1;
  if (calculate_nof_frames_per_symbol) {
    nof_frames_per_symbol = ofh_uplane_fragment_size_calculator::calculate_nof_segments(
        tx_config.mtu_size, nof_prbs, tx_config.dl_compr_params, headers_size);
  }

  return std::make_shared<ether::eth_frame_pool>(logger, tx_config.mtu_size, nof_frames_per_symbol, type, direction);
}

static transmitter_impl_dependencies
resolve_transmitter_dependencies(const transmitter_config&                               tx_config,
                                 srslog::basic_logger&                                   logger,
                                 task_executor&                                          tx_executor,
                                 task_executor&                                          downlink_executor,
                                 error_notifier&                                         err_notifier,
                                 std::unique_ptr<ether::transmitter>                     eth_transmitter,
                                 std::shared_ptr<prach_context_repository>               prach_context_repo,
                                 std::shared_ptr<uplink_context_repository>              ul_slot_context_repo,
                                 std::shared_ptr<uplink_cplane_context_repository>       ul_cp_context_repo,
                                 std::shared_ptr<uplink_cplane_context_repository>       prach_cp_context_repo,
                                 std::shared_ptr<uplink_notified_grid_symbol_repository> notifier_symbol_repo)
{
  transmitter_impl_dependencies dependencies;

  dependencies.logger       = &logger;
  dependencies.executor     = &tx_executor;
  dependencies.dl_executor  = &downlink_executor;
  dependencies.err_notifier = &err_notifier;

  dependencies.frame_pool_dl_cp =
      create_eth_frame_pool(tx_config, logger, message_type::control_plane, data_direction::downlink, false);

  dependencies.dl_df_cplane = std::make_unique<data_flow_cplane_downlink_task_dispatcher>(
      logger,
      create_data_flow_cplane_sched(tx_config,
                                    tx_config.is_downlink_static_compr_hdr_enabled,
                                    logger,
                                    dependencies.frame_pool_dl_cp,
                                    ul_cp_context_repo,
                                    prach_cp_context_repo),
      downlink_executor,
      tx_config.sector);

  dependencies.frame_pool_dl_up =
      create_eth_frame_pool(tx_config, logger, message_type::user_plane, data_direction::downlink);

  dependencies.dl_df_uplane = std::make_unique<data_flow_uplane_downlink_task_dispatcher>(
      logger,
      create_data_flow_uplane_data(tx_config, logger, dependencies.frame_pool_dl_up),
      downlink_executor,
      tx_config.sector);

  dependencies.frame_pool_ul_cp =
      create_eth_frame_pool(tx_config, logger, message_type::control_plane, data_direction::uplink, false);

  dependencies.ul_df_cplane = create_data_flow_cplane_sched(tx_config,
                                                            tx_config.is_uplink_static_compr_hdr_enabled,
                                                            logger,
                                                            dependencies.frame_pool_ul_cp,
                                                            ul_cp_context_repo,
                                                            prach_cp_context_repo);

  dependencies.ul_slot_repo         = std::move(ul_slot_context_repo);
  dependencies.ul_prach_repo        = std::move(prach_context_repo);
  dependencies.eth_transmitter      = std::move(eth_transmitter);
  dependencies.notifier_symbol_repo = std::move(notifier_symbol_repo);

  return dependencies;
}

std::unique_ptr<transmitter>
srsran::ofh::create_transmitter(const transmitter_config&                               transmitter_cfg,
                                srslog::basic_logger&                                   logger,
                                task_executor&                                          tx_executor,
                                task_executor&                                          downlink_executor,
                                error_notifier&                                         err_notifier,
                                std::unique_ptr<ether::transmitter>                     eth_transmitter,
                                std::shared_ptr<prach_context_repository>               prach_context_repo,
                                std::shared_ptr<uplink_context_repository>              ul_slot_context_repo,
                                std::shared_ptr<uplink_cplane_context_repository>       ul_cp_context_repo,
                                std::shared_ptr<uplink_cplane_context_repository>       prach_cp_context_repo,
                                std::shared_ptr<uplink_notified_grid_symbol_repository> notifier_symbol_repo)
{
  return std::make_unique<transmitter_impl>(transmitter_cfg,
                                            resolve_transmitter_dependencies(transmitter_cfg,
                                                                             logger,
                                                                             tx_executor,
                                                                             downlink_executor,
                                                                             err_notifier,
                                                                             std::move(eth_transmitter),
                                                                             std::move(prach_context_repo),
                                                                             std::move(ul_slot_context_repo),
                                                                             std::move(ul_cp_context_repo),
                                                                             std::move(prach_cp_context_repo),
                                                                             std::move(notifier_symbol_repo)));
}
