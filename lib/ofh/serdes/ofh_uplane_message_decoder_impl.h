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

#include "srsran/ofh/ofh_uplane_message_decoder.h"
#include "srsran/srslog/logger.h"

namespace srsran {
namespace ofh {
class iq_decompressor;
class network_order_binary_deserializer;

/// Open Fronthaul User-Plane message decoder implementation.
class uplane_message_decoder_impl : public uplane_message_decoder
{
public:
  uplane_message_decoder_impl(srslog::basic_logger& logger_,
                              subcarrier_spacing    scs_,
                              unsigned              nof_symbols_,
                              unsigned              ru_nof_prbs_) :
    logger(logger_), scs(scs_), nof_symbols(nof_symbols_), ru_nof_prbs(ru_nof_prbs_)
  {
  }

  // See interface for documentation.
  bool decode(uplane_message_decoder_results& results, span<const uint8_t> message) override;

private:
  /// Decodes the User-Plane message header and returns true on success, otherwise false.
  bool decode_header(uplane_message_decoder_results& results, network_order_binary_deserializer& deserializer);

  /// Decodes the sections and returns true on success, otherwise false.
  bool decode_all_sections(uplane_message_decoder_results& results, network_order_binary_deserializer& deserializer);

  /// Decodes the section header and returns true on success, otherwise false.
  bool decode_section(uplane_message_decoder_results& results, network_order_binary_deserializer& deserializer);

  /// Decodes the section header and returns true on success, otherwise false.
  bool decode_section_header(uplane_section_params& results, network_order_binary_deserializer& deserializer);

  /// Decodes the compression header. Returns true on success, otherwise false.
  virtual bool decode_compression_header(uplane_section_params&             results,
                                         network_order_binary_deserializer& deserializer) = 0;

  /// Decodes the IQ data from the given deserializer. Returns true on success, otherwise false.
  virtual bool decode_iq_data(uplane_section_params& results, network_order_binary_deserializer& deserializer) = 0;

protected:
  srslog::basic_logger&    logger;
  const subcarrier_spacing scs;
  const unsigned           nof_symbols;
  const unsigned           ru_nof_prbs;
};

/// Open Fronthaul User-Plane message decoder implementation with static IQ compression.
class uplane_message_decoder_static_compression_impl : public uplane_message_decoder_impl
{
public:
  explicit uplane_message_decoder_static_compression_impl(srslog::basic_logger&        logger_,
                                                          subcarrier_spacing           scs_,
                                                          unsigned                     nof_symbols_,
                                                          unsigned                     ru_nof_prbs_,
                                                          iq_decompressor&             decompressor_,
                                                          const ru_compression_params& compression_params_) :
    uplane_message_decoder_impl(logger_, scs_, nof_symbols_, ru_nof_prbs_),
    decompressor(decompressor_),
    compression_params(compression_params_)
  {
  }

private:
  // See parent for documentation.
  bool decode_compression_header(uplane_section_params&             results,
                                 network_order_binary_deserializer& deserializer) override;

  // See parent for documentation.
  bool decode_iq_data(uplane_section_params& results, network_order_binary_deserializer& deserializer) override;

private:
  iq_decompressor&            decompressor;
  const ru_compression_params compression_params;
};

} // namespace ofh
} // namespace srsran
