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

#include "srsran/ofh/serdes/ofh_serdes_factories.h"
#include "ofh_cplane_message_builder_dynamic_compression_impl.h"
#include "ofh_cplane_message_builder_static_compression_impl.h"
#include "ofh_uplane_message_builder_dynamic_compression_impl.h"
#include "ofh_uplane_message_builder_static_compression_impl.h"
#include "ofh_uplane_message_decoder_dynamic_compression_impl.h"
#include "ofh_uplane_message_decoder_static_compression_impl.h"

using namespace srsran;
using namespace ofh;

std::unique_ptr<cplane_message_builder> srsran::ofh::create_ofh_control_plane_static_compression_message_builder()
{
  return std::make_unique<cplane_message_builder_static_compression_impl>();
}

std::unique_ptr<cplane_message_builder> srsran::ofh::create_ofh_control_plane_dynamic_compression_message_builder()
{
  return std::make_unique<cplane_message_builder_dynamic_compression_impl>();
}

std::unique_ptr<uplane_message_builder>
srsran::ofh::create_static_compr_method_ofh_user_plane_packet_builder(srslog::basic_logger& logger,
                                                                      iq_compressor&        compressor)
{
  return std::make_unique<ofh_uplane_message_builder_static_compression_impl>(logger, compressor);
}

std::unique_ptr<uplane_message_builder>
srsran::ofh::create_dynamic_compr_method_ofh_user_plane_packet_builder(srslog::basic_logger& logger,
                                                                       iq_compressor&        compressor)
{
  return std::make_unique<ofh_uplane_message_builder_dynamic_compression_impl>(logger, compressor);
}

std::unique_ptr<uplane_message_decoder>
srsran::ofh::create_static_compr_method_ofh_user_plane_packet_decoder(srslog::basic_logger&            logger,
                                                                      subcarrier_spacing               scs,
                                                                      cyclic_prefix                    cp,
                                                                      unsigned                         ru_nof_prbs,
                                                                      std::unique_ptr<iq_decompressor> decompressor,
                                                                      const ru_compression_params&     compr_params,
                                                                      const ru_compression_params& prach_compr_params)
{
  return std::make_unique<uplane_message_decoder_static_compression_impl>(
      logger, scs, get_nsymb_per_slot(cp), ru_nof_prbs, std::move(decompressor), compr_params, prach_compr_params);
}

std::unique_ptr<uplane_message_decoder>
srsran::ofh::create_dynamic_compr_method_ofh_user_plane_packet_decoder(srslog::basic_logger&            logger,
                                                                       subcarrier_spacing               scs,
                                                                       cyclic_prefix                    cp,
                                                                       unsigned                         ru_nof_prbs,
                                                                       std::unique_ptr<iq_decompressor> decompressor)
{
  return std::make_unique<uplane_message_decoder_dynamic_compression_impl>(
      logger, scs, get_nsymb_per_slot(cp), ru_nof_prbs, std::move(decompressor));
}
