/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/ofh_controller.h"
#include "srsran/ofh/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ofh/ofh_ota_symbol_handler.h"
#include "srsran/ofh/ofh_receiver_configuration.h"
#include "srsran/ofh/ofh_sector.h"
#include "srsran/ofh/ofh_sector_config.h"
#include "srsran/ofh/ofh_timing_notifier.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "srsran/ofh/serdes/ofh_cplane_message_builder.h"
#include "srsran/ofh/serdes/ofh_uplane_message_builder.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder.h"
#include "srsran/ofh/transmitter/ofh_downlink_handler.h"
#include "srsran/ofh/transmitter/ofh_transmitter_configuration.h"
#include <memory>

namespace srsran {
class task_executor;

namespace ether {
class eth_frame_pool;
class gateway;
} // namespace ether

namespace ofh {
class iq_decompressor;
class iq_compressor;
class ota_symbol_boundary_notifier;

/// Creates an Open Fronthaul Control-Plane packet builder.
std::unique_ptr<cplane_message_builder> create_ofh_control_plane_packet_builder();

/// Creates an Open Fronthaul User-Plane packet builder with static compression header.
std::unique_ptr<uplane_message_builder>
create_static_comp_method_ofh_user_plane_packet_builder(srslog::basic_logger& logger, iq_compressor& compressor);

/// Creates an Open Fronthaul User-Plane packet builder with dynamic compression header.
std::unique_ptr<uplane_message_builder>
create_dynamic_comp_method_ofh_user_plane_packet_builder(srslog::basic_logger& logger, iq_compressor& compressor);

/// Creates an Open Fronthaul User-Plane packet decoder which supports static compression method.
std::unique_ptr<uplane_message_decoder>
create_static_comp_method_ofh_user_plane_packet_decoder(srslog::basic_logger&        logger,
                                                        subcarrier_spacing           scs,
                                                        cyclic_prefix                cp,
                                                        unsigned                     ru_nof_prbs,
                                                        iq_decompressor&             decompressor,
                                                        const ru_compression_params& comp_params,
                                                        const ru_compression_params& prach_comp_params);

/// Creates an Open Fronthaul User-Plane packet decoder which supports dynamic compression method.
std::unique_ptr<uplane_message_decoder>
create_dynamic_comp_method_ofh_user_plane_packet_decoder(srslog::basic_logger& logger,
                                                         subcarrier_spacing    scs,
                                                         cyclic_prefix         cp,
                                                         unsigned              ru_nof_prbs,
                                                         iq_decompressor&      decompressor);

/// Open Fronthaul controller config.
struct controller_config {
  /// Log.
  srslog::basic_logger* logger = nullptr;
  /// OTA symbol notifier.
  ota_symbol_boundary_notifier* notifier = nullptr;
  /// Executor
  task_executor* executor = nullptr;
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Highest subcarrier spacing.
  subcarrier_spacing scs;
  /// GPS Alpha - Valid value range: [0, 1.2288e7].
  unsigned gps_Alpha;
  /// GPS Beta - Valid value range: [-32768, 32767].
  int gps_Beta;
};

/// Creates an Open Fronthaul controller with the given parameters.
std::unique_ptr<controller> create_ofh_timing_controller(const controller_config& config);

/// Creates an Open Fronthaul OTA symbol notifier.
std::unique_ptr<ota_symbol_boundary_notifier>
create_ofh_ota_symbol_notifier(unsigned                         nof_slot_offset_du_ru,
                               unsigned                         nof_symbols_per_slot,
                               srslog::basic_logger&            logger,
                               std::unique_ptr<timing_notifier> timing_notifier,
                               span<ota_symbol_handler*>        symbol_handlers,
                               task_executor&                   executor);

struct symbol_handler_factory_config {
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Highest subcarrier spacing.
  subcarrier_spacing scs;
  /// Timing parameters.
  du_tx_window_timing_parameters tx_timing_params;
  /// Log.
  srslog::basic_logger* logger;
  /// Ethernet gateway
  ether::gateway* gw;
  /// Ethernet frame pool.
  ether::eth_frame_pool* frame_pool;
};

/// Creates an Open Fronthaul symbol handler.
std::unique_ptr<ota_symbol_handler> create_ofh_symbol_handler(symbol_handler_factory_config& config);

/// Creates an Open Fronthaul sector.
std::unique_ptr<sector> create_ofh_sector(const sector_configuration& sector_cfg);

} // namespace ofh
} // namespace srsran
