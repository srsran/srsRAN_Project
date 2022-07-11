/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "downlink_processor_factory_impl.h"
#include "downlink_processor_pool_impl.h"
#include "downlink_processor_single_executor_impl.h"

// :TODO: remove this include list when the processor factories are moved to the channel_processor folder.
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/phy/upper/channel_processors/ssb_processor.h"
#include "srsgnb/phy/upper/downlink_processor.h"
#include "srsgnb/phy/upper/downlink_processor_factory.h"
#include "srsgnb/phy/upper/signal_processors/pss_processor.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"
#include "srsgnb/phy/upper/signal_processors/sss_processor.h"

using namespace srsgnb;

// :TODO: let's see where to move this.
namespace srsgnb {

/// Struct that defines the configuration for the SSB processor.
struct ssb_processor_config {
  std::unique_ptr<pbch_encoder>        encoder;
  std::unique_ptr<pbch_modulator>      modulator;
  std::unique_ptr<dmrs_pbch_processor> dmrs;
  std::unique_ptr<pss_processor>       pss;
  std::unique_ptr<sss_processor>       sss;
};

extern std::unique_ptr<ssb_processor> create_ssb_processor(ssb_processor_config& config);

/// Struct that defines the configuration for the PDCCH processor.
struct pdcch_processor_config_t {
  std::unique_ptr<pdcch_encoder>        encoder;
  std::unique_ptr<pdcch_modulator>      modulator;
  std::unique_ptr<dmrs_pdcch_processor> dmrs;
};

extern std::unique_ptr<pdcch_processor> create_pdcch_processor(pdcch_processor_config_t& config);

/// Struct that defines the configuration for the PDSCH processor.
struct pdsch_processor_configuration {
  std::unique_ptr<pdsch_encoder>        encoder;
  std::unique_ptr<pdsch_modulator>      modulator;
  std::unique_ptr<dmrs_pdsch_processor> dmrs;
};

extern std::unique_ptr<pdsch_processor> create_pdsch_processor(pdsch_processor_configuration& config);

class pdcch_processor_factory
{
  std::shared_ptr<pdcch_modulator_factory> mod_factory = {
      create_pdcch_modulator_factory_sw(create_modulation_mapper_sw_factory(),
                                        create_pseudo_random_generator_sw_factory())};
  std::shared_ptr<pdcch_encoder_factory> enc_factory = {create_pdcch_encoder_factory_sw()};

public:
  std::unique_ptr<pdcch_processor> create()
  {
    pdcch_processor_config_t config;
    config.encoder   = enc_factory->create();
    config.modulator = mod_factory->create();
    config.dmrs      = create_dmrs_pdcch_processor();

    return create_pdcch_processor(config);
  }
};

class pdsch_processor_factory
{
  std::shared_ptr<crc_calculator_factory>       crc;
  std::shared_ptr<pdsch_encoder_factory>        pdsch_enc_factory;
  std::shared_ptr<pdsch_modulator_factory>      pdsch_mod_factory;
  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_fac;

public:
  explicit pdsch_processor_factory()
  {
    struct pdsch_encoder_factory_sw_configuration config_pdsch_encoder;
    config_pdsch_encoder.encoder_factory      = create_ldpc_encoder_factory_sw("generic");
    config_pdsch_encoder.rate_matcher_factory = create_ldpc_rate_matcher_factory_sw();

    ldpc_segmenter_tx_factory_sw_configuration pdsch_segmenter_config;
    crc                                    = create_crc_calculator_factory_sw();
    pdsch_segmenter_config.crc_factory     = crc;
    config_pdsch_encoder.segmenter_factory = create_ldpc_segmenter_tx_factory_sw(pdsch_segmenter_config);

    pdsch_enc_factory = create_pdsch_encoder_factory_sw(config_pdsch_encoder);

    std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();

    pdsch_mod_factory = create_pdsch_modulator_factory_sw(create_modulation_mapper_sw_factory(), prg_factory);
    dmrs_pdsch_fac    = create_dmrs_pdsch_processor_factory_sw(prg_factory);
  }

  std::unique_ptr<pdsch_processor> create()
  {
    pdsch_processor_configuration configuration;
    configuration.encoder   = pdsch_enc_factory->create();
    configuration.modulator = pdsch_mod_factory->create();
    configuration.dmrs      = dmrs_pdsch_fac->create();
    return create_pdsch_processor(configuration);
  }
};

class ssb_processor_factory
{
  std::shared_ptr<pbch_modulator_factory> pbch_mod_factory = {
      create_pbch_modulator_factory_sw(create_modulation_mapper_sw_factory(),
                                       create_pseudo_random_generator_sw_factory())};

public:
  std::unique_ptr<ssb_processor> create()
  {
    ssb_processor_config ssb_config;
    ssb_config.encoder   = create_pbch_encoder();
    ssb_config.modulator = pbch_mod_factory->create();
    ssb_config.dmrs      = create_dmrs_pbch_processor();
    ssb_config.pss       = create_pss_processor();
    ssb_config.sss       = create_sss_processor();
    return create_ssb_processor(ssb_config);
  }
};

} // namespace srsgnb

namespace {
/// Dummy NZP CSI RS processor.
class csi_rs_processor_dummy : public csi_rs_processor
{
public:
  void map(resource_grid_writer& grid, const config_t& config) override {}
};

} // namespace

downlink_processor_single_executor_factory::downlink_processor_single_executor_factory(upper_phy_rg_gateway& gateway,
                                                                                       task_executor&        executor) :
  gateway(gateway),
  pdcch_proc_factory(std::make_shared<pdcch_processor_factory>()),
  pdsch_proc_factory(std::make_shared<pdsch_processor_factory>()),
  ssb_proc_factory(std::make_shared<ssb_processor_factory>()),
  executor(executor)
{
}

std::unique_ptr<downlink_processor>
downlink_processor_single_executor_factory::create(const downlink_processor_config& config)
{
  std::unique_ptr<pdcch_processor> pdcch = pdcch_proc_factory->create();
  srsran_assert(pdcch, "Invalid PDCCH processor");
  std::unique_ptr<pdsch_processor> pdsch = pdsch_proc_factory->create();
  srsran_assert(pdsch, "Invalid PDSCH processor");
  std::unique_ptr<ssb_processor> ssb = ssb_proc_factory->create();
  srsran_assert(ssb, "Invalid SSB processor");

  return std::make_unique<downlink_processor_single_executor_impl>(gateway,
                                                                   std::move(pdcch),
                                                                   std::move(pdsch),
                                                                   std::move(ssb),
                                                                   std::make_unique<csi_rs_processor_dummy>(),
                                                                   executor);
}

std::unique_ptr<downlink_processor_pool> srsgnb::create_dl_processor_pool(downlink_processor_pool_config config)
{
  // Convert from pool config to pool_impl config.
  downlink_processor_pool_config_impl dl_processors;
  dl_processors.num_sectors = config.num_sectors;

  for (auto& proc : config.dl_processors) {
    dl_processors.procs.push_back({proc.sector, proc.numerology, std::move(proc.procs)});
  }

  return std::make_unique<downlink_processor_pool_impl>(std::move(dl_processors));
}
