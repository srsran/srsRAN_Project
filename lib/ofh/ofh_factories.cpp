/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "receiver/ofh_receiver_factories.h"
#include "timing/ofh_timing_manager_impl.h"
#include "transmitter/ofh_transmitter_factories.h"
#include "srsran/ofh/ethernet/ethernet_factories.h"

#ifdef DPDK_FOUND
#include "ethernet/dpdk/dpdk_ethernet_factories.h"
#endif

using namespace srsran;
using namespace ofh;

std::unique_ptr<timing_manager> srsran::ofh::create_ofh_timing_manager(const controller_config& config,
                                                                       srslog::basic_logger&    logger,
                                                                       task_executor&           executor)
{
  realtime_worker_cfg rt_cfg = {config.cp, config.scs, config.gps_Alpha, config.gps_Beta};

  return std::make_unique<timing_manager_impl>(logger, executor, rt_cfg);
}

static receiver_config generate_receiver_config(const sector_configuration& config)
{
  receiver_config rx_config;
  rx_config.ru_operating_bw                    = config.ru_operating_bw;
  rx_config.scs                                = config.scs;
  rx_config.cp                                 = config.cp;
  rx_config.prach_eaxc                         = config.prach_eaxc;
  rx_config.ul_eaxc                            = config.ul_eaxc;
  rx_config.is_uplink_static_compr_hdr_enabled = config.is_uplink_static_compr_hdr_enabled;
  rx_config.prach_compression_params           = config.prach_compression_params;
  rx_config.ul_compression_params              = config.ul_compression_params;
  rx_config.is_prach_control_plane_enabled     = config.is_prach_control_plane_enabled;
  rx_config.ignore_ecpri_payload_size_field    = config.ignore_ecpri_payload_size_field;
  rx_config.ignore_ecpri_seq_id_field          = config.ignore_ecpri_seq_id_field;

  // In rx, dst and src addresses are swapped.
  rx_config.mac_dst_address  = config.mac_src_address;
  rx_config.mac_src_address  = config.mac_dst_address;
  rx_config.tci              = config.tci;
  rx_config.rx_timing_params = config.rx_window_timing_params;

  return rx_config;
}

static transmitter_config generate_transmitter_config(const sector_configuration& sector_cfg)
{
  transmitter_config tx_config;

  tx_config.bw                          = sector_cfg.bw;
  tx_config.scs                         = sector_cfg.scs;
  tx_config.cp                          = sector_cfg.cp;
  tx_config.dl_eaxc                     = sector_cfg.dl_eaxc;
  tx_config.ul_eaxc                     = sector_cfg.ul_eaxc;
  tx_config.prach_eaxc                  = sector_cfg.prach_eaxc;
  tx_config.is_prach_cp_enabled         = sector_cfg.is_prach_control_plane_enabled;
  tx_config.mac_dst_address             = sector_cfg.mac_dst_address;
  tx_config.mac_src_address             = sector_cfg.mac_src_address;
  tx_config.tci                         = sector_cfg.tci;
  tx_config.interface                   = sector_cfg.interface;
  tx_config.is_promiscuous_mode_enabled = sector_cfg.is_promiscuous_mode_enabled;
  tx_config.mtu_size                    = sector_cfg.mtu_size;
  tx_config.ru_working_bw               = sector_cfg.ru_operating_bw;
  tx_config.symbol_handler_cfg          = {
               sector_cfg.tx_window_timing_params, get_nsymb_per_slot(sector_cfg.cp), sector_cfg.scs};
  tx_config.dl_compr_params                      = sector_cfg.dl_compression_params;
  tx_config.ul_compr_params                      = sector_cfg.ul_compression_params;
  tx_config.prach_compr_params                   = sector_cfg.prach_compression_params;
  tx_config.is_downlink_static_compr_hdr_enabled = sector_cfg.is_downlink_static_compr_hdr_enabled;
  tx_config.downlink_broadcast                   = sector_cfg.is_downlink_broadcast_enabled;
  tx_config.iq_scaling                           = sector_cfg.iq_scaling;
  tx_config.dl_processing_time                   = sector_cfg.dl_processing_time;
  tx_config.tdd_config                           = sector_cfg.tdd_config;
  tx_config.uses_dpdk                            = sector_cfg.uses_dpdk;

  return tx_config;
}

std::unique_ptr<sector> srsran::ofh::create_ofh_sector(const sector_configuration& sector_cfg,
                                                       sector_dependencies&&       sector_deps)
{
  unsigned repository_size = sector_cfg.max_processing_delay_slots * 4;

  auto cp_repo    = std::make_shared<uplink_cplane_context_repository>(repository_size);
  auto prach_repo = std::make_shared<prach_context_repository>(repository_size, *sector_deps.logger);
  auto slot_repo  = std::make_shared<uplink_context_repository>(repository_size, *sector_deps.logger);

  // Build the OFH receiver.
  auto rx_config = generate_receiver_config(sector_cfg);
  auto receiver = create_receiver(rx_config, *sector_deps.logger, sector_deps.notifier, prach_repo, slot_repo, cp_repo);

  // Build the OFH transmitter.
  auto tx_config   = generate_transmitter_config(sector_cfg);
  auto transmitter = create_transmitter(tx_config,
                                        *sector_deps.logger,
                                        *sector_deps.transmitter_executor,
                                        *sector_deps.downlink_executor,
                                        std::move(sector_deps.eth_gateway),
                                        prach_repo,
                                        slot_repo,
                                        cp_repo);

  // Build the ethernet receiver.
#ifdef DPDK_FOUND
  auto eth_receiver = (sector_cfg.uses_dpdk) ? ether::create_dpdk_receiver(*sector_deps.receiver_executor,
                                                                           receiver->get_ethernet_frame_notifier(),
                                                                           *sector_deps.logger)
                                             : ether::create_receiver(sector_cfg.interface,
                                                                      sector_cfg.is_promiscuous_mode_enabled,
                                                                      *sector_deps.receiver_executor,
                                                                      receiver->get_ethernet_frame_notifier(),
                                                                      *sector_deps.logger);
#else
  auto eth_receiver = ether::create_receiver(sector_cfg.interface,
                                             sector_cfg.is_promiscuous_mode_enabled,
                                             *sector_deps.receiver_executor,
                                             receiver->get_ethernet_frame_notifier(),
                                             *sector_deps.logger);
#endif

  // OFH receiver manages the Ethernet receiver lifecycle.
  receiver->set_ethernet_receiver(std::move(eth_receiver));

  return std::make_unique<sector_impl>(
      std::move(receiver), std::move(transmitter), std::move(cp_repo), std::move(prach_repo), std::move(slot_repo));
}
