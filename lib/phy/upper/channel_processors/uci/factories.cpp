/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/phy/upper/channel_processors/uci/factories.h"
#include "uci_decoder_impl.h"

using namespace srsran;
namespace {

class uci_decoder_factory_generic : public uci_decoder_factory
{
public:
  explicit uci_decoder_factory_generic(std::shared_ptr<short_block_detector_factory> detector_factory_,
                                       std::shared_ptr<polar_factory>                polar_dec_factory_,
                                       std::shared_ptr<crc_calculator_factory>       crc_calc_factory_) :
    detector_factory(std::move(detector_factory_)),
    polar_dec_factory(std::move(polar_dec_factory_)),
    crc_calc_factory(std::move(crc_calc_factory_))
  {
    srsran_assert(detector_factory, "Invalid detector factory.");
    srsran_assert(polar_dec_factory, "Invalid polar decoder factory.");
    srsran_assert(crc_calc_factory, "Invalid CRC calculator factory.");
  }

  std::unique_ptr<uci_decoder> create() override
  {
    return std::make_unique<uci_decoder_impl>(detector_factory->create(),
                                              polar_dec_factory->create_code(),
                                              polar_dec_factory->create_rate_dematcher(),
                                              polar_dec_factory->create_decoder(polar_code::NMAX_LOG),
                                              polar_dec_factory->create_deallocator(),
                                              crc_calc_factory->create(crc_generator_poly::CRC6),
                                              crc_calc_factory->create(crc_generator_poly::CRC11));
  }

private:
  std::shared_ptr<short_block_detector_factory> detector_factory;
  std::shared_ptr<polar_factory>                polar_dec_factory;
  std::shared_ptr<crc_calculator_factory>       crc_calc_factory;
};

} // namespace

std::shared_ptr<uci_decoder_factory>
srsran::create_uci_decoder_factory_generic(std::shared_ptr<short_block_detector_factory> decoder_factory,
                                           std::shared_ptr<polar_factory>                polar_factory,
                                           std::shared_ptr<crc_calculator_factory>       crc_calc_factory)
{
  return std::make_shared<uci_decoder_factory_generic>(
      std::move(decoder_factory), std::move(polar_factory), std::move(crc_calc_factory));
}
