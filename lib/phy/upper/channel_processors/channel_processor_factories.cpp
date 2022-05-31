/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "pusch_decoder_impl.h"
#include "pusch_processor_impl.h"

using namespace srsgnb;

std::unique_ptr<pusch_decoder> srsgnb::create_pusch_decoder(pusch_decoder_configuration& config)
{
  std::unique_ptr<ldpc_segmenter_rx>   seg  = create_ldpc_segmenter_rx();
  std::unique_ptr<ldpc_rate_dematcher> rdem = create_ldpc_rate_dematcher();
  std::unique_ptr<ldpc_decoder>        dec  = create_ldpc_decoder("generic");
  pusch_decoder_impl::sch_crc          crcs = {create_crc_calculator(crc_generator_poly::CRC16),
                                               create_crc_calculator(crc_generator_poly::CRC24A),
                                               create_crc_calculator(crc_generator_poly::CRC24B)};
  return std::make_unique<pusch_decoder_impl>(std::move(seg), std::move(rdem), std::move(dec), std::move(crcs));
}

std::unique_ptr<pusch_processor> srsgnb::create_pusch_processor(pusch_processor_configuration& config)
{
  pusch_processor_impl::configuration config_impl = {};
  return std::make_unique<pusch_processor_impl>(config_impl);
}
