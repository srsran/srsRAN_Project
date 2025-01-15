/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
                                                                      unsigned                         sector_id_,
                                                                      std::unique_ptr<iq_decompressor> decompressor,
                                                                      const ru_compression_params&     compr_params)
{
  return std::make_unique<uplane_message_decoder_static_compression_impl>(
      logger, scs, get_nsymb_per_slot(cp), ru_nof_prbs, sector_id_, std::move(decompressor), compr_params);
}

std::unique_ptr<uplane_message_decoder>
srsran::ofh::create_dynamic_compr_method_ofh_user_plane_packet_decoder(srslog::basic_logger&            logger,
                                                                       subcarrier_spacing               scs,
                                                                       cyclic_prefix                    cp,
                                                                       unsigned                         ru_nof_prbs,
                                                                       unsigned                         sector_id_,
                                                                       std::unique_ptr<iq_decompressor> decompressor)
{
  return std::make_unique<uplane_message_decoder_dynamic_compression_impl>(
      logger, scs, get_nsymb_per_slot(cp), ru_nof_prbs, sector_id_, std::move(decompressor));
}
