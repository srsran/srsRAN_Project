/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "upper_phy_sample.h"
#include "srsgnb/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_encoder.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_modulator.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_modulator.h"
#include "srsgnb/phy/upper/channel_processors/ssb_processor.h"
#include "srsgnb/phy/upper/signal_processors/pss_processor.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"
#include "srsgnb/phy/upper/signal_processors/sss_processor.h"

using namespace srsgnb;

namespace {

class upper_phy_sample_sw : public upper_phy_sample
{
private:
  std::unique_ptr<csi_rs_processor> nzp_csi_rs;
  std::unique_ptr<pdcch_processor>  pdcch;
  std::unique_ptr<pdsch_processor>  pdsch;
  std::unique_ptr<ssb_processor>    ssb;

public:
  /// Default constructor.
  upper_phy_sample_sw(std::unique_ptr<csi_rs_processor> nzp_csi_rs_,
                      std::unique_ptr<pdcch_processor>  pdcch_,
                      std::unique_ptr<pdsch_processor>  pdsch_,
                      std::unique_ptr<ssb_processor>    ssb_) :
    nzp_csi_rs(std::move(nzp_csi_rs_)), pdcch(std::move(pdcch_)), pdsch(std::move(pdsch_)), ssb(std::move(ssb_))
  {
    srsran_assert(nzp_csi_rs, "Invalid NZP-CSI-RS processor.");
    srsran_assert(pdcch, "Invalid PDCCH processor.");
    srsran_assert(pdsch, "Invalid PDSCH processor.");
    srsran_assert(ssb, "Invalid SSB processor.");
  }

  // See interface for documentation.
  void map(resource_grid_writer& grid, const config_t& config) override {}

  // See interface for documentation.
  void process(resource_grid_writer& grid, const pdcch_processor::pdu_t& pdu) override {}

  // See interface for documentation.
  void process(resource_grid_writer&                                        grid,
               static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
               const pdsch_processor::pdu_t&                                pdu) override
  {
    pdsch->process(grid, data, pdu);
  }

  // See interface for documentation.
  void process(const ssb_processor::pdu_t& pdu, resource_grid_writer& grid) override { ssb->process(pdu, grid); }
};

} // namespace

namespace srsgnb {

struct pdcch_modulator_config_t {
  std::unique_ptr<modulation_mapper>       modulator;
  std::unique_ptr<pseudo_random_generator> scrambler;
};

std::unique_ptr<pdcch_modulator> create_pdcch_modulator(pdcch_modulator_config_t& config);

struct pdcch_processor_config_t {
  std::unique_ptr<pdcch_encoder>        encoder;
  std::unique_ptr<pdcch_modulator>      modulator;
  std::unique_ptr<dmrs_pdcch_processor> dmrs;
};
std::unique_ptr<pdcch_processor> create_pdcch_processor(pdcch_processor_config_t& config);

struct pdsch_modulator_config_t {
  std::unique_ptr<modulation_mapper>       modulator;
  std::unique_ptr<pseudo_random_generator> scrambler;
};
std::unique_ptr<pdsch_modulator> create_pdsch_modulator(pdsch_modulator_config_t& config);

struct pdsch_processor_configuration {
  std::unique_ptr<pdsch_encoder>        encoder;
  std::unique_ptr<pdsch_modulator>      modulator;
  std::unique_ptr<dmrs_pdsch_processor> dmrs;
};
std::unique_ptr<pdsch_processor> create_pdsch_processor(pdsch_processor_configuration& config);

struct ssb_processor_config {
  std::unique_ptr<pbch_encoder>        encoder;
  std::unique_ptr<pbch_modulator>      modulator;
  std::unique_ptr<dmrs_pbch_processor> dmrs;
  std::unique_ptr<pss_processor>       pss;
  std::unique_ptr<sss_processor>       sss;
};
std::unique_ptr<ssb_processor> create_ssb_processor(ssb_processor_config& config);

} // namespace srsgnb

#define ASSERT_FACTORY(FACTORY_PTR)                                                                                    \
  do {                                                                                                                 \
    if (!FACTORY_PTR) {                                                                                                \
      logger.error("{}:{}: {}: Error creating {}.", __FILE__, __LINE__, __PRETTY_FUNCTION__, #FACTORY_PTR);            \
      return nullptr;                                                                                                  \
    }                                                                                                                  \
  } while (false);

std::unique_ptr<upper_phy_sample> srsgnb::create_upper_phy_sample(const upper_phy_sample_configuration& config)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("UpperPHY", false);
  logger.set_level(srslog::str_to_basic_level(config.log_level));

  std::shared_ptr<crc_calculator_factory> crc_calculator_factory = create_crc_calculator_factory_sw();
  ASSERT_FACTORY(crc_calculator_factory);

  std::shared_ptr<ldpc_encoder_factory> ldpc_encoder_factory = create_ldpc_encoder_factory_sw(config.ldpc_encoder_type);
  ASSERT_FACTORY(ldpc_encoder_factory);

  std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();
  ASSERT_FACTORY(ldpc_rate_matcher_factory);

  ldpc_segmenter_tx_factory_sw_configuration ldpc_segmenter_tx_factory_config = {};
  ldpc_segmenter_tx_factory_config.crc_factory                                = crc_calculator_factory;
  std::shared_ptr<ldpc_segmenter_tx_factory> ldpc_segmenter_tx_factory =
      create_ldpc_segmenter_tx_factory_sw(ldpc_segmenter_tx_factory_config);
  ASSERT_FACTORY(ldpc_segmenter_tx_factory);

  pdsch_encoder_factory_sw_configuration pdsch_encoder_factory_config = {};
  pdsch_encoder_factory_config.encoder_factory                        = ldpc_encoder_factory;
  pdsch_encoder_factory_config.rate_matcher_factory                   = ldpc_rate_matcher_factory;
  pdsch_encoder_factory_config.segmenter_factory                      = ldpc_segmenter_tx_factory;
  std::shared_ptr<pdsch_encoder_factory> pdsch_encoder_factory =
      create_pdsch_encoder_factory_sw(pdsch_encoder_factory_config);
  ASSERT_FACTORY(ldpc_encoder_factory);

  std::shared_ptr<modulation_mapper_factory> modulator_factory = create_modulation_mapper_sw_factory();
  ASSERT_FACTORY(modulator_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  ASSERT_FACTORY(prg_factory);

  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_factory =
      create_dmrs_pdsch_processor_factory_sw(prg_factory);
  ASSERT_FACTORY(dmrs_pdsch_factory);

  std::unique_ptr<csi_rs_processor> nzp_csi_rs = create_csi_rs_processor();
  ASSERT_FACTORY(nzp_csi_rs);

  // Create PDCCH processor
  std::unique_ptr<pdcch_processor> pdcch = nullptr;
  {
    pdcch_modulator_config_t pdcch_modulator_config;
    pdcch_modulator_config.modulator = modulator_factory->create();
    pdcch_modulator_config.scrambler = prg_factory->create();

    pdcch_processor_config_t pdcch_processor_config;
    pdcch_processor_config.dmrs      = create_dmrs_pdcch_processor();
    pdcch_processor_config.encoder   = create_pdcch_encoder();
    pdcch_processor_config.modulator = create_pdcch_modulator(pdcch_modulator_config);
    pdcch                            = create_pdcch_processor(pdcch_processor_config);
    ASSERT_FACTORY(pdcch);
  }

  // Create PDSCH processor.
  std::unique_ptr<pdsch_processor> pdsch = nullptr;
  {
    pdsch_modulator_config_t modulator_config;
    modulator_config.modulator = modulator_factory->create();
    modulator_config.scrambler = prg_factory->create();

    pdsch_processor_configuration processor_config;
    processor_config.encoder   = pdsch_encoder_factory->create();
    processor_config.modulator = create_pdsch_modulator(modulator_config);
    processor_config.dmrs      = dmrs_pdsch_factory->create();

    pdsch = create_pdsch_processor(processor_config);
    ASSERT_FACTORY(pdsch);
  }

  std::unique_ptr<ssb_processor> ssb = nullptr;
  {
    ssb_processor_config ssb_config;
    ssb_config.encoder   = create_pbch_encoder();
    ssb_config.modulator = create_pbch_modulator();
    ssb_config.dmrs      = create_dmrs_pbch_processor();
    ssb_config.pss       = create_pss_processor();
    ssb_config.sss       = create_sss_processor();
    ssb                  = create_ssb_processor(ssb_config);
    ASSERT_FACTORY(ssb);
  }

  return std::make_unique<upper_phy_sample_sw>(
      std::move(nzp_csi_rs), std::move(pdcch), std::move(pdsch), std::move(ssb));
}
