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

#include "srsran/ofh/ofh_factories.h"
#include "ofh_sector_impl.h"
#include "receiver/ofh_receiver_impl.h"
#include "serdes/ofh_cplane_message_builder_impl.h"
#include "serdes/ofh_uplane_message_builder_impl.h"
#include "serdes/ofh_uplane_message_decoder_impl.h"
#include "support/uplink_context_repository.h"
#include "support/uplink_cplane_context_repository.h"
#include "timing/ofh_ota_symbol_dispatcher.h"
#include "timing/realtime_timing_worker.h"
#include "transmitter/ofh_data_flow_cplane_scheduling_commands.h"
#include "transmitter/ofh_data_flow_uplane_downlink_data_impl.h"
#include "transmitter/ofh_downlink_handler_broadcast_impl.h"
#include "transmitter/ofh_downlink_handler_impl.h"
#include "transmitter/ofh_message_transmitter_impl.h"
#include "transmitter/ofh_transmitter_impl.h"
#include "srsran/ofh/compression/compression_factory.h"
#include "srsran/ofh/ecpri/ecpri_factories.h"
#include "srsran/ofh/ethernet/ethernet_factories.h"
#include "srsran/ofh/ethernet/ethernet_properties.h"

using namespace srsran;
using namespace ofh;

static std::unique_ptr<data_flow_cplane_scheduling_commands>
create_data_flow_cplane_sched(const transmitter_config&                         tx_config,
                              srslog::basic_logger&                             logger,
                              std::shared_ptr<ether::eth_frame_pool>            frame_pool,
                              std::shared_ptr<uplink_cplane_context_repository> ul_cplane_context_repo)
{
  data_flow_cplane_scheduling_commands_impl_config config;

  config.logger                 = &logger;
  config.ul_cplane_context_repo = std::move(ul_cplane_context_repo);
  config.frame_pool             = std::move(frame_pool);
  config.eth_builder            = ether::create_vlan_frame_builder();
  config.ecpri_builder          = ecpri::create_ecpri_packet_builder();
  config.cp_builder             = ofh::create_ofh_control_plane_packet_builder();

  config.nof_symbols = get_nsymb_per_slot(tx_config.cp);
  config.ru_nof_prbs =
      get_max_Nprb(bs_channel_bandwidth_to_MHz(tx_config.ru_working_bw), tx_config.scs, srsran::frequency_range::FR1);
  config.dl_compr_params = tx_config.dl_compr_params;
  config.ul_compr_params = tx_config.ul_compr_params;

  // VLAN configuration.
  config.vlan_params.eth_type        = ether::ECPRI_ETH_TYPE;
  config.vlan_params.tci             = tx_config.tci;
  config.vlan_params.mac_dst_address = tx_config.mac_dst_address;
  config.vlan_params.mac_src_address = tx_config.mac_src_address;

  return std::make_unique<data_flow_cplane_scheduling_commands_impl>(std::move(config));
}

static std::unique_ptr<data_flow_uplane_downlink_data>
create_data_flow_uplane_data(const transmitter_config&              tx_config,
                             srslog::basic_logger&                  logger,
                             std::shared_ptr<ether::eth_frame_pool> frame_pool)
{
  data_flow_uplane_downlink_data_impl_config config;

  config.logger        = &logger;
  config.eth_builder   = ether::create_vlan_frame_builder();
  config.ecpri_builder = ecpri::create_ecpri_packet_builder();
  config.frame_pool    = std::move(frame_pool);

  std::array<std::unique_ptr<ofh::iq_compressor>, ofh::NOF_COMPRESSION_TYPES_SUPPORTED> compr;
  for (unsigned i = 0; i != ofh::NOF_COMPRESSION_TYPES_SUPPORTED; ++i) {
    compr[i] = create_iq_compressor(static_cast<ofh::compression_type>(i), tx_config.iq_scaling);
  }
  config.compressor_sel = ofh::create_iq_compressor_selector(std::move(compr));
  config.up_builder     = ofh::create_static_comp_method_ofh_user_plane_packet_builder(logger, *config.compressor_sel);

  config.ru_nof_prbs =
      get_max_Nprb(bs_channel_bandwidth_to_MHz(tx_config.ru_working_bw), tx_config.scs, srsran::frequency_range::FR1);
  config.compr_params = tx_config.dl_compr_params;

  // VLAN configuration.
  config.vlan_params.eth_type        = ether::ECPRI_ETH_TYPE;
  config.vlan_params.tci             = tx_config.tci;
  config.vlan_params.mac_dst_address = tx_config.mac_dst_address;
  config.vlan_params.mac_src_address = tx_config.mac_src_address;

  return std::make_unique<data_flow_uplane_downlink_data_impl>(std::move(config));
}

std::unique_ptr<cplane_message_builder> srsran::ofh::create_ofh_control_plane_packet_builder()
{
  return std::make_unique<cplane_message_builder_impl>();
}

std::unique_ptr<uplane_message_builder>
srsran::ofh::create_static_comp_method_ofh_user_plane_packet_builder(srslog::basic_logger& logger,
                                                                     iq_compressor&        compressor)
{
  return std::make_unique<ofh_uplane_message_builder_static_compression_impl>(logger, compressor);
}

std::unique_ptr<uplane_message_decoder>
srsran::ofh::create_static_comp_method_ofh_user_plane_packet_decoder(srslog::basic_logger&        logger,
                                                                     subcarrier_spacing           scs,
                                                                     cyclic_prefix                cp,
                                                                     unsigned                     ru_nof_prbs,
                                                                     iq_decompressor&             decompressor,
                                                                     const ru_compression_params& comp_params)
{
  return std::make_unique<uplane_message_decoder_static_compression_impl>(
      logger, scs, get_nsymb_per_slot(cp), ru_nof_prbs, decompressor, comp_params);
}

std::unique_ptr<controller> srsran::ofh::create_ofh_timing_controller(const controller_config& config)
{
  realtime_worker_cfg rt_cfg = {config.cp, config.scs, config.gps_Alpha, config.gps_Beta};

  return std::make_unique<realtime_timing_worker>(*config.logger, *config.notifier, *config.executor, rt_cfg);
}

std::unique_ptr<ota_symbol_boundary_notifier>
srsran::ofh::create_ofh_ota_symbol_notifier(unsigned                            nof_slot_offset_du_ru,
                                            unsigned                            nof_symbols_per_slot,
                                            srslog::basic_logger&               logger,
                                            std::unique_ptr<timing_notifier>    timing_notifier,
                                            span<symbol_handler*>               symbol_handlers,
                                            span<ota_symbol_boundary_notifier*> ota_notifiers)
{
  return std::make_unique<ota_symbol_dispatcher>(
      nof_slot_offset_du_ru, nof_symbols_per_slot, logger, std::move(timing_notifier), symbol_handlers, ota_notifiers);
}

static receiver_config generate_receiver_config(const sector_configuration& config)
{
  receiver_config rx_config;

  rx_config.scs = config.scs;
  rx_config.du_ul_slot_nof_prbs =
      get_max_Nprb(bs_channel_bandwidth_to_MHz(config.bw), config.scs, frequency_range::FR1);

  rx_config.ru_nof_prbs =
      get_max_Nprb(bs_channel_bandwidth_to_MHz(config.ru_operating_bw), config.scs, frequency_range::FR1);
  rx_config.is_prach_cp_enabled = config.is_prach_control_plane_enabled;
  rx_config.ul_prach_eaxc       = config.ul_prach_eaxc;
  rx_config.ul_eaxc             = config.ul_eaxc;
  rx_config.cp                  = config.cp;
  // In rx, dst and src addresses are swapped.
  rx_config.mac_dst_address = config.mac_src_address;
  rx_config.mac_src_address = config.mac_dst_address;
  rx_config.tci             = config.tci;

  return rx_config;
}

static receiver_impl_dependencies
resolve_receiver_dependencies(const sector_configuration&                                  sector_cfg,
                              const receiver_config&                                       rx_config,
                              std::shared_ptr<uplink_context_repository<ul_prach_context>> prach_context_repo,
                              std::shared_ptr<uplink_context_repository<ul_slot_context>>  ul_slot_context_repo,
                              std::shared_ptr<uplink_cplane_context_repository>            ul_cp_context_repo,
                              uplane_rx_symbol_notifier&                                   notifier)
{
  receiver_impl_dependencies depen;

  depen.logger = sector_cfg.logger;

  // Comrpessors.
  std::array<std::unique_ptr<ofh::iq_decompressor>, ofh::NOF_COMPRESSION_TYPES_SUPPORTED> decompr;
  for (unsigned i = 0; i != ofh::NOF_COMPRESSION_TYPES_SUPPORTED; ++i) {
    decompr[i] = create_iq_decompressor(static_cast<ofh::compression_type>(i));
  }
  depen.decompressor_sel = create_iq_decompressor_selector(std::move(decompr));

  // Open FronHaul decoder.
  depen.uplane_decoder = ofh::create_static_comp_method_ofh_user_plane_packet_decoder(*sector_cfg.logger,
                                                                                      rx_config.scs,
                                                                                      rx_config.cp,
                                                                                      rx_config.ru_nof_prbs,
                                                                                      *depen.decompressor_sel,
                                                                                      sector_cfg.ul_compression_params);

  depen.ecpri_decoder     = ecpri::create_ecpri_packet_decoder(*sector_cfg.logger);
  depen.eth_frame_decoder = ether::create_vlan_frame_decoder(*sector_cfg.logger);

  depen.prach_context_repo   = std::move(prach_context_repo);
  depen.ul_slot_context_repo = std::move(ul_slot_context_repo);
  depen.ul_cp_context_repo   = std::move(ul_cp_context_repo);
  depen.notifier             = &notifier;

  return depen;
}

static transmitter_config generate_transmitter_config(const sector_configuration& sector_cfg)
{
  transmitter_config tx_config;

  tx_config.bw                 = sector_cfg.bw;
  tx_config.scs                = sector_cfg.scs;
  tx_config.cp                 = sector_cfg.cp;
  tx_config.downlink_broadcast = sector_cfg.is_downlink_broadcast_enabled;
  if (sector_cfg.is_prach_control_plane_enabled) {
    tx_config.ul_prach_eaxc.emplace(sector_cfg.ul_prach_eaxc);
  }
  tx_config.ul_eaxc            = sector_cfg.ul_eaxc;
  tx_config.dl_eaxc            = sector_cfg.dl_eaxc;
  tx_config.mac_dst_address    = sector_cfg.mac_dst_address;
  tx_config.mac_src_address    = sector_cfg.mac_src_address;
  tx_config.tci                = sector_cfg.tci;
  tx_config.interface          = sector_cfg.interface;
  tx_config.ru_working_bw      = sector_cfg.ru_operating_bw;
  tx_config.ul_compr_params    = sector_cfg.ul_compression_params;
  tx_config.dl_compr_params    = sector_cfg.dl_compression_params;
  tx_config.ru_working_bw      = sector_cfg.ru_operating_bw;
  tx_config.symbol_handler_cfg = {
      sector_cfg.tx_window_timing_params, get_nsymb_per_slot(sector_cfg.cp), sector_cfg.scs};
  tx_config.iq_scaling = sector_cfg.iq_scaling;

  return tx_config;
}

static std::unique_ptr<downlink_handler>
create_downlink_handler(const transmitter_config&                         tx_config,
                        transmitter_impl_dependencies&                    tx_depen,
                        std::shared_ptr<uplink_cplane_context_repository> ul_cp_context_repo)
{
  auto data_flow_cplane =
      create_data_flow_cplane_sched(tx_config, *tx_depen.logger, tx_depen.frame_pool, std::move(ul_cp_context_repo));

  auto data_flow_uplane = create_data_flow_uplane_data(tx_config, *tx_depen.logger, tx_depen.frame_pool);

  if (tx_config.downlink_broadcast) {
    return std::make_unique<downlink_handler_broadcast_impl>(
        tx_config.dl_eaxc, std::move(data_flow_cplane), std::move(data_flow_uplane));
  }

  return std::make_unique<downlink_handler_impl>(
      tx_config.dl_eaxc, std::move(data_flow_cplane), std::move(data_flow_uplane));
}

static std::unique_ptr<uplink_request_handler>
create_uplink_request_handler(const transmitter_config&                                    tx_config,
                              transmitter_impl_dependencies&                               tx_depen,
                              std::shared_ptr<uplink_context_repository<ul_prach_context>> prach_context_repo,
                              std::shared_ptr<uplink_context_repository<ul_slot_context>>  ul_slot_context_repo,
                              std::shared_ptr<uplink_cplane_context_repository>            ul_cp_context_repo)
{
  uplink_request_handler_impl_config config;

  config.ul_prach_eaxc = tx_config.ul_prach_eaxc;
  config.ul_data_eaxc  = tx_config.ul_eaxc;
  config.ul_slot_repo  = std::move(ul_slot_context_repo);
  config.ul_prach_repo = std::move(prach_context_repo);
  config.data_flow =
      create_data_flow_cplane_sched(tx_config, *tx_depen.logger, tx_depen.frame_pool, std::move(ul_cp_context_repo));

  return std::make_unique<uplink_request_handler_impl>(std::move(config));
}

static transmitter_impl_dependencies
resolve_transmitter_dependencies(const sector_configuration&                                  sector_cfg,
                                 const transmitter_config&                                    tx_config,
                                 std::shared_ptr<uplink_context_repository<ul_prach_context>> prach_context_repo,
                                 std::shared_ptr<uplink_context_repository<ul_slot_context>>  ul_slot_context_repo,
                                 std::shared_ptr<uplink_cplane_context_repository>            ul_cp_context_repo)
{
  transmitter_impl_dependencies tx_depen;

  /// Control-Plane data flow for uplink request handler.
  std::unique_ptr<data_flow_cplane_scheduling_commands> data_flow_cplane;
  /// Downlink handler.
  std::unique_ptr<downlink_handler> dl_handler;

  tx_depen.logger     = sector_cfg.logger;
  tx_depen.frame_pool = std::make_shared<ether::eth_frame_pool>();
  ether::gw_config eth_cfg;
  eth_cfg.interface       = sector_cfg.interface;
  eth_cfg.mac_dst_address = sector_cfg.mac_dst_address;
  tx_depen.eth_gateway    = ether::create_gateway(eth_cfg, *sector_cfg.logger);

  tx_depen.dl_handler = create_downlink_handler(tx_config, tx_depen, ul_cp_context_repo);
  tx_depen.ul_request_handler =
      create_uplink_request_handler(tx_config, tx_depen, prach_context_repo, ul_slot_context_repo, ul_cp_context_repo);

  return tx_depen;
}

std::unique_ptr<sector> srsran::ofh::create_ofh_sector(const sector_configuration& sector_cfg)
{
  unsigned repository_size = sector_cfg.max_processing_delay_slots * 4;

  auto cp_repo    = std::make_shared<uplink_cplane_context_repository>(repository_size);
  auto prach_repo = std::make_shared<uplink_context_repository<ul_prach_context>>(repository_size);
  auto slot_repo  = std::make_shared<uplink_context_repository<ul_slot_context>>(repository_size);

  // Build the OFH receiver.
  auto rx_config = generate_receiver_config(sector_cfg);
  auto rx_depen =
      resolve_receiver_dependencies(sector_cfg, rx_config, prach_repo, slot_repo, cp_repo, *sector_cfg.notifier);
  auto receiver = std::make_unique<receiver_impl>(rx_config, std::move(rx_depen));

  // Build the OFH transmitter.
  auto tx_config   = generate_transmitter_config(sector_cfg);
  auto tx_depen    = resolve_transmitter_dependencies(sector_cfg, tx_config, prach_repo, slot_repo, cp_repo);
  auto transmitter = std::make_unique<transmitter_impl>(tx_config, std::move(tx_depen));

  // Build the ethernet receiver.
  auto eth_receiver = ether::create_receiver(
      sector_cfg.interface, *sector_cfg.receiver_executor, receiver->get_ethernet_frame_notifier(), *sector_cfg.logger);

  return std::make_unique<sector_impl>(std::move(receiver),
                                       std::move(transmitter),
                                       std::move(cp_repo),
                                       std::move(prach_repo),
                                       std::move(slot_repo),
                                       std::move(eth_receiver));
}
