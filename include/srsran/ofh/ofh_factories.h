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

#include "srsran/ofh/ofh_controller.h"
#include "srsran/ofh/ofh_sector.h"
#include "srsran/ofh/ofh_sector_config.h"
#include "srsran/ofh/ofh_uplane_rx_symbol_notifier.h"
#include "srsran/ofh/receiver/ofh_receiver_configuration.h"
#include "srsran/ofh/receiver/ofh_sequence_id_checker.h"
#include "srsran/ofh/serdes/ofh_cplane_message_builder.h"
#include "srsran/ofh/serdes/ofh_uplane_message_builder.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder.h"
#include "srsran/ofh/timing/ofh_ota_symbol_boundary_notifier.h"
#include "srsran/ofh/timing/ofh_timing_manager.h"
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

/// Creates an Open Fronthaul Control-Plane static compression message builder.
std::unique_ptr<cplane_message_builder> create_ofh_control_plane_static_compression_message_builder();

/// Creates an Open Fronthaul Control-Plane dynamic compression message builder.
std::unique_ptr<cplane_message_builder> create_ofh_control_plane_dynamic_compression_message_builder();

/// Creates an Open Fronthaul User-Plane packet builder with static compression header.
std::unique_ptr<uplane_message_builder>
create_static_compr_method_ofh_user_plane_packet_builder(srslog::basic_logger& logger, iq_compressor& compressor);

/// Creates an Open Fronthaul User-Plane packet builder with dynamic compression header.
std::unique_ptr<uplane_message_builder>
create_dynamic_compr_method_ofh_user_plane_packet_builder(srslog::basic_logger& logger, iq_compressor& compressor);

/// Creates an Open Fronthaul User-Plane packet decoder which supports static compression method.
std::unique_ptr<uplane_message_decoder>
create_static_compr_method_ofh_user_plane_packet_decoder(srslog::basic_logger&            logger,
                                                         subcarrier_spacing               scs,
                                                         cyclic_prefix                    cp,
                                                         unsigned                         ru_nof_prbs,
                                                         std::unique_ptr<iq_decompressor> decompressor,
                                                         const ru_compression_params&     compr_params,
                                                         const ru_compression_params&     prach_compr_params);

/// Creates an Open Fronthaul User-Plane packet decoder which supports dynamic compression method.
std::unique_ptr<uplane_message_decoder>
create_dynamic_compr_method_ofh_user_plane_packet_decoder(srslog::basic_logger&            logger,
                                                          subcarrier_spacing               scs,
                                                          cyclic_prefix                    cp,
                                                          unsigned                         ru_nof_prbs,
                                                          std::unique_ptr<iq_decompressor> decompressor);

/// Creates an Open Fronthaul sequence identifier checker.
std::unique_ptr<sequence_id_checker> create_sequence_id_checker();

/// Open Fronthaul controller config.
struct controller_config {
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// Highest subcarrier spacing.
  subcarrier_spacing scs;
  /// GPS Alpha - Valid value range: [0, 1.2288e7].
  unsigned gps_Alpha;
  /// GPS Beta - Valid value range: [-32768, 32767].
  int gps_Beta;
};

/// Creates an Open Fronthaul timing manager with the given parameters.
std::unique_ptr<timing_manager>
create_ofh_timing_manager(const controller_config& config, srslog::basic_logger& logger, task_executor& executor);

/// Creates an Open Fronthaul sector.
std::unique_ptr<sector> create_ofh_sector(const sector_configuration& sector_cfg, sector_dependencies&& sector_deps);

} // namespace ofh
} // namespace srsran
