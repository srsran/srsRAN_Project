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
#include "crc_calculator_lut_impl.h"
#include "ldpc/ldpc_decoder_impl.h"
#include "ldpc/ldpc_encoder_impl.h"
#include "ldpc/ldpc_rate_dematcher_impl.h"
#include "ldpc/ldpc_rate_matcher_impl.h"
#include "ldpc/ldpc_segmenter_impl.h"
#include "polar/polar_allocator_impl.h"
#include "polar/polar_code_impl.h"
#include "polar/polar_deallocator_impl.h"
#include "polar/polar_decoder_impl.h"
#include "polar/polar_encoder_impl.h"
#include "polar/polar_interleaver_impl.h"
#include "polar/polar_rate_dematcher_impl.h"
#include "polar/polar_rate_matcher_impl.h"
#include "short/short_block_detector_impl.h"
#include "srsgnb/support/cpu_features.h"

#ifdef __PCLMUL__
#include "crc_calculator_clmul_impl.h"
#endif // __PCLMUL__
#ifdef __AVX2__
#include "ldpc/ldpc_decoder_avx2.h"
#include "ldpc/ldpc_encoder_avx2.h"
#include "ldpc/ldpc_rate_dematcher_avx2_impl.h"
#endif // __AVX2__
#ifdef HAVE_AVX512
#include "ldpc/ldpc_decoder_avx512.h"
#endif // HAVE_AVX512

#if ENABLE_AVX512
#include "ldpc/ldpc_rate_dematcher_avx512_impl.h"
#endif // ENABLE_AVX512

using namespace srsgnb;

namespace {

class crc_calculator_factory_sw_impl : public crc_calculator_factory
{
public:
  crc_calculator_factory_sw_impl(const std::string& type_) : type(type_) {}

  std::unique_ptr<crc_calculator> create(crc_generator_poly poly) override
  {
#ifdef __PCLMUL__
    if (((type == "auto") || (type == "clmul")) && cpu_supports_feature(cpu_feature::pclmul)) {
      return std::make_unique<crc_calculator_clmul_impl>(poly);
    }
#endif // __PCLMUL__

    if ((type == "auto") || (type == "lut")) {
      return std::make_unique<crc_calculator_lut_impl>(poly);
    }

    return nullptr;
  }

private:
  std::string type;
};

class ldpc_decoder_factory_sw : public ldpc_decoder_factory
{
private:
  std::string dec_type;

public:
  explicit ldpc_decoder_factory_sw(std::string dec_type_) : dec_type(std::move(dec_type_)) {}

  std::unique_ptr<ldpc_decoder> create() override
  {
#ifdef HAVE_AVX512
    if ((dec_type == "auto") || (dec_type == "avx512")) {
      return std::make_unique<ldpc_decoder_avx512>();
    }
#endif // HAVE_AVX512
#ifdef HAVE_AVX2
    if ((dec_type == "auto") || (dec_type == "avx2")) {
      return std::make_unique<ldpc_decoder_avx2>();
    }
#endif // HAVE_AVX2
    if ((dec_type == "auto") || (dec_type == "generic")) {
      return std::make_unique<ldpc_decoder_generic>();
    }
    return {};
  }
};

class ldpc_encoder_factory_sw : public ldpc_encoder_factory
{
private:
  std::string enc_type;

public:
  explicit ldpc_encoder_factory_sw(std::string enc_type_) : enc_type(std::move(enc_type_)) {}

  std::unique_ptr<ldpc_encoder> create() override
  {
#ifdef __AVX2__
    if ((enc_type == "avx2") || (enc_type == "auto")) {
      return std::make_unique<ldpc_encoder_avx2>();
    }
#endif // __AVX2__
    if ((enc_type == "generic") || (enc_type == "auto")) {
      return std::make_unique<ldpc_encoder_generic>();
    }
    return {};
  }
};

class ldpc_rate_dematcher_factory_sw : public ldpc_rate_dematcher_factory
{
public:
  ldpc_rate_dematcher_factory_sw() = default;

  std::unique_ptr<ldpc_rate_dematcher> create() override
  {
#ifdef ENABLE_AVX512
    return std::make_unique<ldpc_rate_dematcher_avx512_impl>();
#endif // ENABLE_AVX512
#ifdef ENABLE_AVX2
    return std::make_unique<ldpc_rate_dematcher_avx2_impl>();
#endif // ENABLE_AVX2

    return std::make_unique<ldpc_rate_dematcher_impl>();
  }
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
  std::shared_ptr<crc_calculator_factory> crc_factory;

public:
  explicit ldpc_segmenter_tx_factory_sw(std::shared_ptr<crc_calculator_factory> crc_factory_) :
    crc_factory(std::move(crc_factory_))
  {
    srsgnb_assert(crc_factory, "Invalid CRC calculator factory.");
  }

  std::unique_ptr<ldpc_segmenter_tx> create() override
  {
    ldpc_segmenter_impl::sch_crc sch_crc = {
        crc_factory->create(crc_generator_poly::CRC16),
        crc_factory->create(crc_generator_poly::CRC24A),
        crc_factory->create(crc_generator_poly::CRC24B),
    };
    return ldpc_segmenter_impl::create_ldpc_segmenter_impl_tx(sch_crc);
  }
};

class ldpc_segmenter_rx_factory_sw : public ldpc_segmenter_rx_factory
{
public:
  std::unique_ptr<ldpc_segmenter_rx> create() override { return ldpc_segmenter_impl::create_ldpc_segmenter_impl_rx(); }
};

class polar_factory_sw : public polar_factory
{
public:
  std::unique_ptr<polar_allocator>   create_allocator() override { return std::make_unique<polar_allocator_impl>(); }
  std::unique_ptr<polar_code>        create_code() override { return std::make_unique<polar_code_impl>(); }
  std::unique_ptr<polar_deallocator> create_deallocator() override
  {
    return std::make_unique<polar_deallocator_impl>();
  }
  std::unique_ptr<polar_decoder> create_decoder(unsigned code_size_log) override
  {
    return std::make_unique<polar_decoder_impl>(create_encoder(), code_size_log);
  }
  std::unique_ptr<polar_encoder>     create_encoder() override { return std::make_unique<polar_encoder_impl>(); }
  std::unique_ptr<polar_interleaver> create_interleaver() override
  {
    return std::make_unique<polar_interleaver_impl>();
  }
  std::unique_ptr<polar_rate_dematcher> create_rate_dematcher() override
  {
    return std::make_unique<polar_rate_dematcher_impl>();
  }
  std::unique_ptr<polar_rate_matcher> create_rate_matcher() override
  {
    return std::make_unique<polar_rate_matcher_impl>();
  }
};

class short_block_detector_factory_sw : public short_block_detector_factory
{
public:
  explicit short_block_detector_factory_sw() {}

  std::unique_ptr<short_block_detector> create() override { return std::make_unique<short_block_detector_impl>(); }
};

} // namespace

std::shared_ptr<ldpc_decoder_factory> srsgnb::create_ldpc_decoder_factory_sw(const std::string& dec_type)
{
  return std::make_unique<ldpc_decoder_factory_sw>(dec_type);
}

std::shared_ptr<ldpc_encoder_factory> srsgnb::create_ldpc_encoder_factory_sw(const std::string& enc_type)
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
srsgnb::create_ldpc_segmenter_tx_factory_sw(std::shared_ptr<crc_calculator_factory> crc_factory)
{
  return std::make_shared<ldpc_segmenter_tx_factory_sw>(std::move(crc_factory));
}

std::shared_ptr<ldpc_segmenter_rx_factory> srsgnb::create_ldpc_segmenter_rx_factory_sw()
{
  return std::make_shared<ldpc_segmenter_rx_factory_sw>();
}

std::shared_ptr<crc_calculator_factory> srsgnb::create_crc_calculator_factory_sw(const std::string& type)
{
  return std::make_shared<crc_calculator_factory_sw_impl>(type);
}

std::shared_ptr<polar_factory> srsgnb::create_polar_factory_sw()
{
  return std::make_shared<polar_factory_sw>();
}

std::shared_ptr<short_block_detector_factory> srsgnb::create_short_block_detector_factory_sw()
{
  return std::make_unique<short_block_detector_factory_sw>();
}
