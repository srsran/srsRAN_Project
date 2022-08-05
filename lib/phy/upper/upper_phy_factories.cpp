/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/upper_phy_factories.h"
#include "downlink_processor_pool_impl.h"
#include "downlink_processor_single_executor_impl.h"
#include "srsgnb/phy/support/support_factories.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/support/error_handling.h"
#include "uplink_processor_pool_impl.h"
#include "uplink_processor_single_executor_impl.h"
#include "upper_phy_impl.h"

// :TODO: remove this include list when the processor factories are moved to the channel_processor folder.
#include "srsgnb/phy/upper/channel_processors/ssb_processor.h"
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
  std::shared_ptr<crc_calculator_factory> crc_factory     = create_crc_calculator_factory_sw();
  std::shared_ptr<polar_factory>          encoder_factory = create_polar_factory_sw();

  std::shared_ptr<pdcch_modulator_factory> mod_factory = {
      create_pdcch_modulator_factory_sw(create_channel_modulation_sw_factory(),
                                        create_pseudo_random_generator_sw_factory())};
  std::shared_ptr<pdcch_encoder_factory> enc_factory = {create_pdcch_encoder_factory_sw(crc_factory, encoder_factory)};

public:
  std::unique_ptr<pdcch_processor> create()
  {
    pdcch_processor_config_t config;
    config.encoder = enc_factory->create();
    report_fatal_error_if_not(config.encoder, "Invalid PDCCH encoder.");
    config.modulator = mod_factory->create();
    report_fatal_error_if_not(config.modulator, "Invalid PDCCH modulator.");
    config.dmrs = create_dmrs_pdcch_processor();
    report_fatal_error_if_not(config.dmrs, "Invalid DMRS PDCCH processor.");

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

    pdsch_mod_factory = create_pdsch_modulator_factory_sw(create_channel_modulation_sw_factory(), prg_factory);
    dmrs_pdsch_fac    = create_dmrs_pdsch_processor_factory_sw(prg_factory);
  }

  std::unique_ptr<pdsch_processor> create()
  {
    pdsch_processor_configuration configuration;
    configuration.encoder = pdsch_enc_factory->create();
    report_fatal_error_if_not(configuration.encoder, "Invalid PDSCH encoder.");
    configuration.modulator = pdsch_mod_factory->create();
    report_fatal_error_if_not(configuration.modulator, "Invalid PDSCH modulator.");
    configuration.dmrs = dmrs_pdsch_fac->create();
    report_fatal_error_if_not(configuration.dmrs, "Invalid DMRS PDsCH processor.");

    return create_pdsch_processor(configuration);
  }
};

class ssb_processor_factory
{
  std::shared_ptr<pbch_modulator_factory> pbch_mod_factory = {
      create_pbch_modulator_factory_sw(create_channel_modulation_sw_factory(),
                                       create_pseudo_random_generator_sw_factory())};

public:
  std::unique_ptr<ssb_processor> create()
  {
    ssb_processor_config ssb_config;
    ssb_config.encoder = create_pbch_encoder();
    report_fatal_error_if_not(ssb_config.encoder, "Invalid PBCH encoder.");
    ssb_config.modulator = pbch_mod_factory->create();
    report_fatal_error_if_not(ssb_config.modulator, "Invalid PBCH modulator.");
    ssb_config.dmrs = create_dmrs_pbch_processor();
    report_fatal_error_if_not(ssb_config.dmrs, "Invalid DMRS PBCH processor.");
    ssb_config.pss = create_pss_processor();
    report_fatal_error_if_not(ssb_config.pss, "Invalid PSS processor.");
    ssb_config.sss = create_sss_processor();
    report_fatal_error_if_not(ssb_config.sss, "Invalid SSS processor.");

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

/// \brief Factory to create single executor uplink processors.
class uplink_processor_single_executor_factory : public uplink_processor_factory
{
public:
  uplink_processor_single_executor_factory(std::shared_ptr<prach_detector_factory> prach_factory_,
                                           task_executor&                          executor,
                                           upper_phy_rx_results_notifier&          results_notifier) :
    prach_factory(prach_factory_), executor(executor), results_notifier(results_notifier)
  {
    report_fatal_error_if_not(prach_factory, "Invalid PRACH factory.");
  }

  // See interface for documentation.
  std::unique_ptr<uplink_processor> create(const uplink_processor_config& config) override
  {
    std::unique_ptr<prach_detector> detector = prach_factory->create();
    report_fatal_error_if_not(detector, "Invalid PRACH detector.");

    return std::make_unique<uplink_processor_single_executor_impl>(std::move(detector), executor, results_notifier);
  }

private:
  std::shared_ptr<prach_detector_factory> prach_factory;
  task_executor&                          executor;
  upper_phy_rx_results_notifier&          results_notifier;
};

/// \brief Factory to create single executor downlink processors.
class downlink_processor_single_executor_factory : public downlink_processor_factory
{
public:
  downlink_processor_single_executor_factory(upper_phy_rg_gateway& gateway, task_executor& executor) :
    gateway(gateway),
    pdcch_proc_factory(std::make_shared<pdcch_processor_factory>()),
    pdsch_proc_factory(std::make_shared<pdsch_processor_factory>()),
    ssb_proc_factory(std::make_shared<ssb_processor_factory>()),
    executor(executor)
  {
    // Do nothing.
  }

  // See interface for documentation.
  std::unique_ptr<downlink_processor> create(const downlink_processor_config& config) override
  {
    std::unique_ptr<pdcch_processor> pdcch = pdcch_proc_factory->create();
    report_fatal_error_if_not(pdcch, "Invalid PDCCH processor.");

    std::unique_ptr<pdsch_processor> pdsch = pdsch_proc_factory->create();
    report_fatal_error_if_not(pdsch, "Invalid PDSCH processor.");

    std::unique_ptr<ssb_processor> ssb = ssb_proc_factory->create();
    report_fatal_error_if_not(ssb, "Invalid SSB processor.");

    return std::make_unique<downlink_processor_single_executor_impl>(gateway,
                                                                     std::move(pdcch),
                                                                     std::move(pdsch),
                                                                     std::move(ssb),
                                                                     std::make_unique<csi_rs_processor_dummy>(),
                                                                     executor);
  }

private:
  upper_phy_rg_gateway&                    gateway;
  std::shared_ptr<pdcch_processor_factory> pdcch_proc_factory;
  std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory;
  std::shared_ptr<ssb_processor_factory>   ssb_proc_factory;
  task_executor&                           executor;
};

static std::unique_ptr<downlink_processor_pool> build_dl_processor_pool(const upper_phy_config& config)
{
  downlink_processor_single_executor_factory factory(*config.gateway, *config.dl_executor);

  downlink_processor_pool_config config_pool;
  config_pool.num_sectors = 1;
  // :TODO: Change how to manage the numerologies in the future.
  downlink_processor_pool_config::sector_dl_processor info = {0, subcarrier_spacing::kHz15, {}};

  for (unsigned i = 0, e = config.nof_dl_processors; i != e; ++i) {
    std::unique_ptr<downlink_processor> dl_proc = factory.create({});
    report_fatal_error_if_not(dl_proc, "Invalid downlink processor.");
    info.procs.push_back(std::move(dl_proc));
  }

  config_pool.dl_processors.push_back(std::move(info));

  return create_dl_processor_pool(std::move(config_pool));
}

static std::unique_ptr<resource_grid_pool> build_dl_resource_grid_pool(const upper_phy_config& config)
{
  // Configure one pool per upper PHY.
  unsigned                                    nof_sectors = 1;
  unsigned                                    nof_slots   = config.nof_slots_dl_rg;
  std::vector<std::unique_ptr<resource_grid>> grids;
  for (unsigned sector_idx = 0; sector_idx != nof_sectors; ++sector_idx) {
    for (unsigned slot_id = 0; slot_id != nof_slots; ++slot_id) {
      std::unique_ptr<resource_grid> grid =
          create_resource_grid(config.nof_ports, MAX_NSYMB_PER_SLOT, config.dl_bw_rb * NRE);
      report_fatal_error_if_not(grid, "Invalid resource grid.");
      grids.push_back(std::move(grid));
    }
  }

  return create_resource_grid_pool(nof_sectors, nof_slots, std::move(grids));
}

class upper_phy_factory_impl : public upper_phy_factory
{
public:
  std::unique_ptr<upper_phy> create(const upper_phy_config& config) override
  {
    std::unique_ptr<resource_grid_pool> rg_pool = build_dl_resource_grid_pool(config);
    srsgnb_assert(rg_pool, "Invalid resource grid pool");
    std::unique_ptr<downlink_processor_pool> dl_pool = build_dl_processor_pool(config);
    srsgnb_assert(dl_pool, "Invalid downlink processor pool");

    return std::make_unique<upper_phy_impl>(config.sector_id, std::move(dl_pool), std::move(rg_pool));
  }
};

} // namespace

std::unique_ptr<uplink_processor_pool> srsgnb::create_uplink_processor_pool(uplink_processor_pool_config config)
{
  // Convert from pool config to pool_impl config.
  uplink_processor_pool_impl_config ul_processors;
  ul_processors.num_sectors = config.num_sectors;

  for (auto& proc : config.ul_processors) {
    ul_processors.procs.push_back({proc.sector, proc.scs, std::move(proc.procs)});
  }

  return std::make_unique<uplink_processor_pool_impl>(std::move(ul_processors));
}

std::unique_ptr<downlink_processor_pool> srsgnb::create_dl_processor_pool(downlink_processor_pool_config config)
{
  // Convert from pool config to pool_impl config.
  downlink_processor_pool_impl_config dl_processors;
  dl_processors.num_sectors = config.num_sectors;

  for (auto& proc : config.dl_processors) {
    dl_processors.procs.push_back({proc.sector, proc.scs, std::move(proc.procs)});
  }

  return std::make_unique<downlink_processor_pool_impl>(std::move(dl_processors));
}

std::unique_ptr<upper_phy_factory> srsgnb::create_upper_phy_factory()
{
  return std::make_unique<upper_phy_factory_impl>();
}
