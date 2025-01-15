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

#include "srsran/ofh/ecpri/ecpri_packet_builder.h"
#include "srsran/ofh/ecpri/ecpri_packet_decoder.h"
#include "srsran/srslog/logger.h"
#include <memory>

namespace srsran {
namespace ecpri {

/// Creates and returns an eCPRI packet builder.
std::unique_ptr<packet_builder> create_ecpri_packet_builder();

/// Creates and returns an eCPRI packet decoder utilizing payload size encoded in eCPRI header.
std::unique_ptr<packet_decoder> create_ecpri_packet_decoder_using_payload_size(srslog::basic_logger& logger,
                                                                               unsigned              sector);

/// Creates and returns an eCPRI packet decoder ignoring payload size encoded in eCPRI header.
std::unique_ptr<packet_decoder> create_ecpri_packet_decoder_ignoring_payload_size(srslog::basic_logger& logger,
                                                                                  unsigned              sector);

} // namespace ecpri
} // namespace srsran
