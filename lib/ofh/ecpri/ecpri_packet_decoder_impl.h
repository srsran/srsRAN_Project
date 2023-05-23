/*
 *
 * Copyright 2013-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/ofh/ecpri/ecpri_packet_decoder.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ecpri {

/// eCPRI packet decoder implementation.
class packet_decoder_impl : public packet_decoder
{
public:
  explicit packet_decoder_impl(srslog::basic_logger& logger_) : logger(logger_) {}

  // See interface for documentation.
  span<const uint8_t> decode(span<const uint8_t> packet, packet_parameters& params) override;

private:
  srslog::basic_logger& logger;
};

} // namespace ecpri
} // namespace srsran
