/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_coding/channel_coding_factories.h"
#include "crc_calculator_impl.h"
#include "ldpc/ldpc_decoder_impl.h"
#include "ldpc/ldpc_encoder_impl.h"
#include "ldpc/ldpc_rate_dematcher_impl.h"
#include "ldpc/ldpc_rate_matcher_impl.h"
#include "ldpc/ldpc_segmenter_impl.h"

using namespace srsgnb;

namespace {

class crc_calculator_factory_sw : public crc_calculator_factory
{
public:
  std::unique_ptr<crc_calculator> create(crc_generator_poly poly) override
  {
    return std::make_unique<crc_calculator_impl>(poly);
  }
};

class ldpc_decoder_factory_sw : public ldpc_decoder_factory
{
private:
  std::string dec_type;

public:
  explicit ldpc_decoder_factory_sw(std::string dec_type_) : dec_type(dec_type_) {}

  std::unique_ptr<ldpc_decoder> create() override
  {
    if (dec_type == "generic") {
      return std::make_unique<ldpc_decoder_generic>();
    }
    if (dec_type == "avx2") {
      return std::make_unique<ldpc_decoder_avx2>();
    }
    return {};
  }
};

class ldpc_encoder_factory_sw : public ldpc_encoder_factory
{
private:
  std::string enc_type;

public:
  explicit ldpc_encoder_factory_sw(const std::string& enc_type_) : enc_type(enc_type_) {}

  std::unique_ptr<ldpc_encoder> create() override
  {
    if (enc_type == "generic") {
      return std::make_unique<ldpc_encoder_generic>();
    }
    if (enc_type == "avx2") {
      return std::make_unique<ldpc_encoder_avx2>();
    }
    return {};
  }
};

class ldpc_rate_dematcher_factory_sw : public ldpc_rate_dematcher_factory
{
public:
  ldpc_rate_dematcher_factory_sw() = default;

  std::unique_ptr<ldpc_rate_dematcher> create() override { return std::make_unique<ldpc_rate_dematcher_impl>(); }
};

class ldpc_rate_matcher_factory_sw : public ldpc_rate_matcher_factory
{
public:
  ldpc_rate_matcher_factory_sw() = default;

  std::unique_ptr<ldpc_rate_matcher> create() override { return std::make_unique<ldpc_rate_matcher_impl>(); }
};

class ldpc_segmenter_tx_factory_sw : public ldpc_segmenter_tx_factory
{
private:
  crc_calculator_factory& crc_factory;

public:
  explicit ldpc_segmenter_tx_factory_sw(ldpc_segmenter_tx_factory_sw_configuration& config) :
    crc_factory(*config.crc_factory)
  {
    srsgnb_assert(config.crc_factory, "Invalid CRC calculator factory.");
  }

  std::unique_ptr<ldpc_segmenter_tx> create() override
  {
    ldpc_segmenter_impl::sch_crc sch_crc = {
        crc_factory.create(crc_generator_poly::CRC16),
        crc_factory.create(crc_generator_poly::CRC24A),
        crc_factory.create(crc_generator_poly::CRC24B),
    };
    return ldpc_segmenter_impl::create_ldpc_segmenter_impl_tx(sch_crc);
  }
};

class ldpc_segmenter_rx_factory_sw : public ldpc_segmenter_rx_factory
{
public:
  std::unique_ptr<ldpc_segmenter_rx> create() override { return ldpc_segmenter_impl::create_ldpc_segmenter_impl_rx(); }
};

} // namespace

std::shared_ptr<ldpc_decoder_factory> srsgnb::create_ldpc_decoder_factory_sw(std::string dec_type)
{
  return std::make_unique<ldpc_decoder_factory_sw>(dec_type);
}

std::shared_ptr<ldpc_encoder_factory> srsgnb::create_ldpc_encoder_factory_sw(std::string enc_type)
{
  return std::make_unique<ldpc_encoder_factory_sw>(enc_type);
}

std::shared_ptr<ldpc_rate_dematcher_factory> srsgnb::create_ldpc_rate_dematcher_factory_sw()
{
  return std::make_shared<ldpc_rate_dematcher_factory_sw>();
}

std::shared_ptr<ldpc_rate_matcher_factory> srsgnb::create_ldpc_rate_matcher_factory_sw()
{
  return std::make_shared<ldpc_rate_matcher_factory_sw>();
}

std::shared_ptr<ldpc_segmenter_tx_factory>
srsgnb::create_ldpc_segmenter_tx_factory_sw(ldpc_segmenter_tx_factory_sw_configuration& config)
{
  return std::make_shared<ldpc_segmenter_tx_factory_sw>(config);
}

std::shared_ptr<ldpc_segmenter_rx_factory> srsgnb::create_ldpc_segmenter_rx_factory_sw()
{
  return std::make_shared<ldpc_segmenter_rx_factory_sw>();
}

std::shared_ptr<crc_calculator_factory> srsgnb::create_crc_calculator_factory_sw()
{
  return std::make_shared<crc_calculator_factory_sw>();
}