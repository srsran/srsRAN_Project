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
#include "upper_phy_rx_results_notifier_proxy.h"

using namespace srsgnb;

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
  downlink_processor_single_executor_factory(std::shared_ptr<pdcch_processor_factory> pdcch_proc_factory_,
                                             std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory_,
                                             std::shared_ptr<ssb_processor_factory>   ssb_proc_factory_) :
    pdcch_proc_factory(pdcch_proc_factory_),
    pdsch_proc_factory(pdsch_proc_factory_),
    ssb_proc_factory(ssb_proc_factory_)
  {
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

    return std::make_unique<downlink_processor_single_executor_impl>(*config.gateway,
                                                                     std::move(pdcch),
                                                                     std::move(pdsch),
                                                                     std::move(ssb),
                                                                     std::make_unique<csi_rs_processor_dummy>(),
                                                                     *config.executor);
  }

private:
  std::shared_ptr<pdcch_processor_factory> pdcch_proc_factory;
  std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory;
  std::shared_ptr<ssb_processor_factory>   ssb_proc_factory;
};

static std::unique_ptr<downlink_processor_pool>
create_downlink_processor_pool(std::shared_ptr<downlink_processor_factory> factory, const upper_phy_config& config)
{
  downlink_processor_pool_config config_pool;
  config_pool.num_sectors = 1;
  // :TODO: Only processors for SCS 15kHz are being created. Fix this in the future.
  downlink_processor_pool_config::sector_dl_processor info = {0, subcarrier_spacing::kHz15, {}};

  for (unsigned i = 0, e = config.nof_dl_processors; i != e; ++i) {
    downlink_processor_config processor_config;
    processor_config.id                         = i;
    processor_config.gateway                    = config.gateway;
    processor_config.executor                   = config.dl_executor;
    std::unique_ptr<downlink_processor> dl_proc = factory->create(processor_config);
    report_fatal_error_if_not(dl_proc, "Invalid downlink processor.");
    info.procs.push_back(std::move(dl_proc));
  }

  config_pool.dl_processors.push_back(std::move(info));

  return create_dl_processor_pool(std::move(config_pool));
}

static std::unique_ptr<resource_grid_pool> create_dl_resource_grid_pool(const upper_phy_config& config)
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

static std::unique_ptr<uplink_processor_pool> create_ul_processor_pool(const upper_phy_config&        config,
                                                                       upper_phy_rx_results_notifier& notifier)
{
  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw();
  if (!dft_factory) {
    dft_factory = create_dft_processor_factory_generic();
    report_fatal_error_if_not(dft_factory, "Invalid DFT factory.");
  }

  std::shared_ptr<prach_generator_factory> prach_gen_factory = create_prach_generator_factory_sw(dft_factory);
  report_fatal_error_if_not(prach_gen_factory, "Invalid PRACH generator factory.");

  std::shared_ptr<prach_detector_factory> prach_detector_factory =
      create_prach_detector_factory_simple(dft_factory, prach_gen_factory);
  report_fatal_error_if_not(prach_detector_factory, "Invalid PRACH detector factory.");

  uplink_processor_single_executor_factory factory(prach_detector_factory, *config.ul_executor, notifier);

  uplink_processor_pool_config config_pool;
  config_pool.num_sectors = 1;
  // :TODO: Only processors for SCS 15kHz are being created. Fix this in the future.
  uplink_processor_pool_config::sector_ul_processors info = {0, subcarrier_spacing::kHz15, {}};

  for (unsigned i = 0, e = config.nof_ul_processors; i != e; ++i) {
    std::unique_ptr<uplink_processor> ul_proc = factory.create({});
    report_fatal_error_if_not(ul_proc, "Invalid uplink processor.");

    info.procs.push_back(std::move(ul_proc));
  }

  config_pool.ul_processors.push_back(std::move(info));

  return create_uplink_processor_pool(std::move(config_pool));
}

static std::unique_ptr<prach_buffer_pool> create_prach_pool(const upper_phy_config& config)
{
  std::vector<std::unique_ptr<prach_buffer>> prach_mem;
  prach_mem.reserve(config.nof_ul_processors);

  for (unsigned i = 0, e = config.nof_ul_processors; i != e; ++i) {
    std::unique_ptr<prach_buffer> buffer = create_prach_buffer_long();
    report_fatal_error_if_not(buffer, "Invalid PRACH buffer.");
    prach_mem.push_back(std::move(buffer));
  }

  return create_prach_buffer_pool(std::move(prach_mem));
}

class upper_phy_factory_impl : public upper_phy_factory
{
public:
  upper_phy_factory_impl(std::shared_ptr<downlink_processor_factory> downlink_proc_factory_) :
    downlink_proc_factory(downlink_proc_factory_)
  {
    srsgnb_assert(downlink_proc_factory, "Invalid downlink processor factory.");
  }

  std::unique_ptr<upper_phy> create(const upper_phy_config& config) override
  {
    std::unique_ptr<resource_grid_pool> rg_pool = create_dl_resource_grid_pool(config);
    report_fatal_error_if_not(rg_pool, "Invalid downlink resource grid pool.");

    std::unique_ptr<downlink_processor_pool> dl_pool = create_downlink_processor_pool(downlink_proc_factory, config);
    report_fatal_error_if_not(dl_pool, "Invalid downlink processor pool.");

    auto notifier_proxy = std::make_unique<upper_phy_rx_results_notifier_proxy>();

    std::unique_ptr<uplink_processor_pool> ul_pool = create_ul_processor_pool(config, *notifier_proxy);
    report_fatal_error_if_not(ul_pool, "Invalid uplink processor pool.");

    std::unique_ptr<prach_buffer_pool> prach_pool = create_prach_pool(config);
    report_fatal_error_if_not(prach_pool, "Invalid PRACH buffer pool.");

    return std::make_unique<upper_phy_impl>(config.sector_id,
                                            std::move(dl_pool),
                                            std::move(rg_pool),
                                            std::move(ul_pool),
                                            std::move(prach_pool),
                                            std::move(notifier_proxy),
                                            *config.symbol_request_notifier);
  }

private:
  std::shared_ptr<downlink_processor_factory> downlink_proc_factory;
};

} // namespace

std::shared_ptr<downlink_processor_factory>
srsgnb::create_downlink_processor_factory_sw(const downlink_processor_factory_sw_config& config)
{
  // Create channel coding factories - CRC
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw();
  report_fatal_error_if_not(crc_calc_factory, "Invalid CRC factory.");

  // Create channel coding factories - LDPC
  std::shared_ptr<ldpc_segmenter_tx_factory> ldpc_seg_tx_factory =
      create_ldpc_segmenter_tx_factory_sw(crc_calc_factory);
  report_fatal_error_if_not(ldpc_seg_tx_factory, "Invalid LDPC segmenter factory.");

  std::shared_ptr<ldpc_encoder_factory> ldpc_enc_factory = create_ldpc_encoder_factory_sw(config.ldpc_encoder_type);
  report_fatal_error_if_not(ldpc_enc_factory, "Invalid LDPC encoder factory of type {}.", config.ldpc_encoder_type);

  std::shared_ptr<ldpc_rate_matcher_factory> ldpc_rm_factory = create_ldpc_rate_matcher_factory_sw();
  report_fatal_error_if_not(ldpc_rm_factory, "Invalid LDPC RM factory.");

  // Create channel coding factories - Polar
  std::shared_ptr<polar_factory> polar_factory = create_polar_factory_sw();
  report_fatal_error_if_not(polar_factory, "Invalid CRC factory.");

  // Create sequence generators factories - PRG
  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  report_fatal_error_if_not(prg_factory, "Invalid PRG factory.");

  // Create modulation mapper factory.
  std::shared_ptr<channel_modulation_factory> mod_factory = create_channel_modulation_sw_factory();
  report_fatal_error_if_not(prg_factory, "Invalid modulation factory.");

  // Create channel processors encoder factories - PBCH
  std::shared_ptr<pbch_encoder_factory> pbch_enc_factory =
      create_pbch_encoder_factory_sw(crc_calc_factory, prg_factory, polar_factory);
  report_fatal_error_if_not(pbch_enc_factory, "Invalid PDCCH encoder factory.");

  // Create channel processors encoder factories - PDCCH
  std::shared_ptr<pdcch_encoder_factory> pdcch_enc_factory =
      create_pdcch_encoder_factory_sw(crc_calc_factory, polar_factory);
  report_fatal_error_if_not(pdcch_enc_factory, "Invalid PDCCH encoder factory.");

  // Create channel processors encoder factories - PDSCH
  pdsch_encoder_factory_sw_configuration pdsch_enc_factory_config;
  pdsch_enc_factory_config.segmenter_factory    = ldpc_seg_tx_factory;
  pdsch_enc_factory_config.encoder_factory      = ldpc_enc_factory;
  pdsch_enc_factory_config.rate_matcher_factory = ldpc_rm_factory;

  std::shared_ptr<pdsch_encoder_factory> pdsch_enc_factory = create_pdsch_encoder_factory_sw(pdsch_enc_factory_config);
  report_fatal_error_if_not(pdsch_enc_factory, "Invalid PDSCH encoder factory.");

  // Create channel processors modulation factories - PBCH
  std::shared_ptr<pbch_modulator_factory> pbch_mod_factory = create_pbch_modulator_factory_sw(mod_factory, prg_factory);
  report_fatal_error_if_not(pbch_mod_factory, "Invalid PBCH modulation factory.");

  // Create channel processors modulation factories - PDCCH
  std::shared_ptr<pdcch_modulator_factory> pdcch_mod_factory =
      create_pdcch_modulator_factory_sw(mod_factory, prg_factory);
  report_fatal_error_if_not(pdcch_mod_factory, "Invalid PDCCH modulation factory.");

  // Create channel processors modulation factories - PDSCH
  std::shared_ptr<pdsch_modulator_factory> pdsch_mod_factory =
      create_pdsch_modulator_factory_sw(mod_factory, prg_factory);
  report_fatal_error_if_not(pdsch_mod_factory, "Invalid PDSCH modulation factory.");

  // Create DMRS generators - PBCH, PSS, SSS
  std::shared_ptr<dmrs_pbch_processor_factory> dmrs_pbch_proc_factory =
      create_dmrs_pbch_processor_factory_sw(prg_factory);
  report_fatal_error_if_not(dmrs_pbch_proc_factory, "Invalid DMRS PBCH factory.");

  std::shared_ptr<pss_processor_factory> pss_proc_factory = create_pss_processor_factory_sw();
  report_fatal_error_if_not(pss_proc_factory, "Invalid PSS factory.");

  std::shared_ptr<sss_processor_factory> sss_proc_factory = create_sss_processor_factory_sw();
  report_fatal_error_if_not(sss_proc_factory, "Invalid SSS factory.");

  // Create DMRS generators - PDCCH
  std::shared_ptr<dmrs_pdcch_processor_factory> dmrs_pdcch_proc_factory =
      create_dmrs_pdcch_processor_factory_sw(prg_factory);
  report_fatal_error_if_not(dmrs_pdcch_proc_factory, "Invalid DMRS PDCCH factory.");

  // Create DMRS generators - PDSCH
  std::shared_ptr<dmrs_pdsch_processor_factory> dmrs_pdsch_proc_factory =
      create_dmrs_pdsch_processor_factory_sw(prg_factory);
  report_fatal_error_if_not(dmrs_pdsch_proc_factory, "Invalid DMRS PDSCH factory.");

  // Create channel processors - PDCCH
  std::shared_ptr<pdcch_processor_factory> pdcch_proc_factory =
      create_pdcch_processor_factory_sw(pdcch_enc_factory, pdcch_mod_factory, dmrs_pdcch_proc_factory);
  report_fatal_error_if_not(pdcch_proc_factory, "Invalid PDCCH processor factory.");

  // Create channel processors - PDSCH
  std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory =
      create_pdsch_processor_factory_sw(pdsch_enc_factory, pdsch_mod_factory, dmrs_pdsch_proc_factory);
  report_fatal_error_if_not(pdsch_proc_factory, "Invalid PDSCH processor factory.");

  // Create channel processors - SSB
  ssb_processor_factory_sw_configuration ssb_factory_config;
  ssb_factory_config.encoder_factory                      = pbch_enc_factory;
  ssb_factory_config.modulator_factory                    = pbch_mod_factory;
  ssb_factory_config.dmrs_factory                         = dmrs_pbch_proc_factory;
  ssb_factory_config.pss_factory                          = pss_proc_factory;
  ssb_factory_config.sss_factory                          = sss_proc_factory;
  std::shared_ptr<ssb_processor_factory> ssb_proc_factory = create_ssb_processor_factory_sw(ssb_factory_config);
  report_fatal_error_if_not(pdsch_proc_factory, "Invalid PDSCH processor factory.");

  return std::make_shared<downlink_processor_single_executor_factory>(
      pdcch_proc_factory, pdsch_proc_factory, ssb_proc_factory);
}

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

std::unique_ptr<upper_phy_factory>
srsgnb::create_upper_phy_factory(std::shared_ptr<downlink_processor_factory> downlink_proc_factory)
{
  return std::make_unique<upper_phy_factory_impl>(downlink_proc_factory);
}
