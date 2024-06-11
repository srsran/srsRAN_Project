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

#include "srsran/ofh/serdes/ofh_cplane_message_builder.h"
#include "srsran/ofh/serdes/ofh_uplane_message_builder.h"
#include "srsran/ofh/serdes/ofh_uplane_message_decoder.h"
#include "srsran/srslog/logger.h"
#include <memory>

namespace srsran {
namespace ofh {

class iq_decompressor;
class iq_compressor;

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
                                                         const ru_compression_params&     compr_params);

/// Creates an Open Fronthaul User-Plane packet decoder which supports dynamic compression method.
std::unique_ptr<uplane_message_decoder>
create_dynamic_compr_method_ofh_user_plane_packet_decoder(srslog::basic_logger&            logger,
                                                          subcarrier_spacing               scs,
                                                          cyclic_prefix                    cp,
                                                          unsigned                         ru_nof_prbs,
                                                          std::unique_ptr<iq_decompressor> decompressor);

} // namespace ofh
} // namespace srsran
