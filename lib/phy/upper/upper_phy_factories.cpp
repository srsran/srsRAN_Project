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

#include "srsran/phy/upper/upper_phy_factories.h"
#include "downlink_processor_pool_impl.h"
#include "downlink_processor_single_executor_impl.h"
#include "logging_downlink_processor_decorator.h"
#include "logging_uplink_processor_decorator.h"
#include "uplink_processor_concurrent.h"
#include "uplink_processor_impl.h"
#include "uplink_processor_pool_impl.h"
#include "uplink_processor_task_dispatcher.h"
#include "upper_phy_impl.h"
#include "upper_phy_pdu_validators.h"
#include "upper_phy_rx_symbol_handler_printer_decorator.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/unique_rx_softbuffer.h"
#include "srsran/support/error_handling.h"

using namespace srsran;

namespace {

class uplink_processor_base_factory : public uplink_processor_factory
{
public:
  uplink_processor_base_factory(std::shared_ptr<pucch_processor_factory> pucch_factory_,
                                std::shared_ptr<pusch_processor_factory> pusch_factory_,
                                std::shared_ptr<prach_detector_factory>  prach_factory_) :
    pucch_factory(std::move(pucch_factory_)),
    pusch_factory(std::move(pusch_factory_)),
    prach_factory(std::move(prach_factory_))
  {
    report_fatal_error_if_not(prach_factory, "Invalid PRACH factory.");
    report_fatal_error_if_not(pusch_factory, "Invalid PUSCH factory.");
    report_fatal_error_if_not(pucch_factory, "Invalid PUCCH factory.");
  }

  std::unique_ptr<uplink_processor> create(const uplink_processor_config& config) override
  {
    std::unique_ptr<prach_detector> prach = prach_factory->create();
    report_fatal_error_if_not(prach, "Invalid PRACH detector.");

    std::unique_ptr<pusch_processor> pusch_proc = pusch_factory->create();
    report_fatal_error_if_not(pusch_proc, "Invalid PUSCH processor.");

    std::unique_ptr<pucch_processor> pucch_proc = pucch_factory->create();
    report_fatal_error_if_not(pucch_proc, "Invalid PUCCH processor.");

    return std::make_unique<uplink_processor_impl>(std::move(prach), std::move(pusch_proc), std::move(pucch_proc));
  }

  std::unique_ptr<uplink_processor>
  create(const uplink_processor_config& config, srslog::basic_logger& logger, bool log_all_opportunities) override
  {
    std::unique_ptr<prach_detector> prach = prach_factory->create(logger, log_all_opportunities);
    report_fatal_error_if_not(prach, "Invalid PRACH detector.");

    std::unique_ptr<pusch_processor> pusch_proc = pusch_factory->create(logger);
    report_fatal_error_if_not(pusch_proc, "Invalid PUSCH processor.");

    std::unique_ptr<pucch_processor> pucch_proc = pucch_factory->create(logger);
    report_fatal_error_if_not(pucch_proc, "Invalid PUCCH processor.");

    return std::make_unique<uplink_processor_impl>(std::move(prach), std::move(pusch_proc), std::move(pucch_proc));
  }

  std::unique_ptr<uplink_pdu_validator> create_pdu_validator() override
  {
    return std::make_unique<uplink_processor_validator_impl>(
        prach_factory->create_validator(), pucch_factory->create_validator(), pusch_factory->create_validator());
  }

private:
  std::shared_ptr<pucch_processor_factory> pucch_factory;
  std::shared_ptr<pusch_processor_factory> pusch_factory;
  std::shared_ptr<prach_detector_factory>  prach_factory;
};

class uplink_processor_concurrent_factory : public uplink_processor_factory
{
public:
  uplink_processor_concurrent_factory(unsigned nof_processors_, std::shared_ptr<uplink_processor_factory> factory_) :
    nof_processors(nof_processors_), factory(std::move(factory_))
  {
    report_fatal_error_if_not(factory, "Invalid uplink factory.");
  }

  // See interface for documentation.
  std::unique_ptr<uplink_processor> create(const uplink_processor_config& config) override
  {
    if (common_processor_pool == nullptr) {
      // Create base uplink processor.
      std::vector<std::unique_ptr<uplink_processor>> uplink_procs;
      uplink_procs.reserve(nof_processors);

      // Create processors.
      for (unsigned i_proc = 0; i_proc != nof_processors; ++i_proc) {
        uplink_procs.emplace_back(factory->create(config));
        report_fatal_error_if_not(uplink_procs.back(), "Invalid uplink processor.");
      }

      common_processor_pool = std::make_shared<uplink_processor_concurrent::processor_pool>(std::move(uplink_procs));
    }

    // Create concurrent processor.
    return std::make_unique<uplink_processor_concurrent>(common_processor_pool);
  }

  // See interface for documentation.
  std::unique_ptr<uplink_processor>
  create(const uplink_processor_config& config, srslog::basic_logger& logger, bool log_all_opportunities) override
  {
    if (common_processor_pool == nullptr) {
      // Create base uplink processor.
      std::vector<std::unique_ptr<uplink_processor>> uplink_procs;
      uplink_procs.reserve(nof_processors);

      // Create processors.
      for (unsigned i_proc = 0; i_proc != nof_processors; ++i_proc) {
        uplink_procs.emplace_back(factory->create(config, logger, log_all_opportunities));
        report_fatal_error_if_not(uplink_procs.back(), "Invalid uplink processor.");
      }

      common_processor_pool = std::make_shared<uplink_processor_concurrent::processor_pool>(std::move(uplink_procs));
    }

    // Create concurrent processor.
    return std::make_unique<uplink_processor_concurrent>(common_processor_pool);
  }

  std::unique_ptr<uplink_pdu_validator> create_pdu_validator() override { return factory->create_pdu_validator(); }

private:
  unsigned                                                     nof_processors;
  std::shared_ptr<uplink_processor_factory>                    factory;
  std::shared_ptr<uplink_processor_concurrent::processor_pool> common_processor_pool;
};

/// \brief Factory to create single executor uplink processors.
class uplink_processor_task_dispatcher_factory : public uplink_processor_factory
{
public:
  uplink_processor_task_dispatcher_factory(std::shared_ptr<uplink_processor_factory> factory_,
                                           task_executor&                            pucch_executor_,
                                           task_executor&                            pusch_executor_,
                                           task_executor&                            prach_executor_) :
    factory(std::move(factory_)),
    pucch_executor(pucch_executor_),
    pusch_executor(pusch_executor_),
    prach_executor(prach_executor_)
  {
    report_fatal_error_if_not(factory, "Invalid uplink factory.");
  }

  // See interface for documentation.
  std::unique_ptr<uplink_processor> create(const uplink_processor_config& config) override
  {
    // Create base uplink processor.
    std::unique_ptr<uplink_processor> uplink_proc = factory->create(config);
    report_fatal_error_if_not(uplink_proc, "Invalid uplink processor.");

    // Wrap uplink processor with executor.
    uplink_proc = std::make_unique<uplink_processor_task_dispatcher>(
        std::move(uplink_proc), pucch_executor, pusch_executor, prach_executor);

    return uplink_proc;
  }

  // See interface for documentation.
  std::unique_ptr<uplink_processor>
  create(const uplink_processor_config& config, srslog::basic_logger& logger, bool log_all_opportunities) override
  {
    // Create base uplink processor.
    std::unique_ptr<uplink_processor> uplink_proc = factory->create(config, logger, log_all_opportunities);
    report_fatal_error_if_not(uplink_proc, "Invalid uplink processor.");

    // Wrap uplink processor with executor.
    uplink_proc = std::make_unique<uplink_processor_task_dispatcher>(
        std::move(uplink_proc), pucch_executor, pusch_executor, prach_executor);

    return uplink_proc;
  }

  std::unique_ptr<uplink_pdu_validator> create_pdu_validator() override { return factory->create_pdu_validator(); }

private:
  std::shared_ptr<uplink_processor_factory> factory;
  task_executor&                            pucch_executor;
  task_executor&                            pusch_executor;
  task_executor&                            prach_executor;
};

/// \brief Factory to create single executor uplink processors.
class uplink_processor_logging_decorator_factory : public uplink_processor_factory
{
public:
  uplink_processor_logging_decorator_factory(srslog::basic_levels                      log_level_,
                                             unsigned                                  log_hex_limit_,
                                             std::shared_ptr<uplink_processor_factory> factory_) :
    log_level(log_level_), log_hex_limit(log_hex_limit_), factory(std::move(factory_))
  {
    report_fatal_error_if_not(factory, "Invalid uplink factory.");
  }

  // See interface for documentation.
  std::unique_ptr<uplink_processor> create(const uplink_processor_config& config) override
  {
    return factory->create(config);
  }

  // See interface for documentation.
  std::unique_ptr<uplink_processor>
  create(const uplink_processor_config& config, srslog::basic_logger& /**/, bool log_all_opportunities) override
  {
    // Setup logger.
    srslog::basic_logger& logger = srslog::fetch_basic_logger("UL-PHY" + std::to_string(proc_count++), true);
    logger.set_level(log_level);
    logger.set_hex_dump_max_size(log_hex_limit);

    // Create base uplink processor.
    std::unique_ptr<uplink_processor> uplink_proc = factory->create(config, logger, log_all_opportunities);
    report_fatal_error_if_not(uplink_proc, "Invalid uplink processor.");

    // Wrap uplink processor with executor.
    uplink_proc = std::make_unique<logging_uplink_processor_decorator>(std::move(uplink_proc), logger);

    return uplink_proc;
  }

  std::unique_ptr<uplink_pdu_validator> create_pdu_validator() override { return factory->create_pdu_validator(); }

private:
  unsigned                                  proc_count = 0;
  srslog::basic_levels                      log_level;
  unsigned                                  log_hex_limit;
  std::shared_ptr<uplink_processor_factory> factory;
};

/// \brief Factory to create single executor downlink processors.
class downlink_processor_single_executor_factory : public downlink_processor_factory
{
public:
  downlink_processor_single_executor_factory(std::shared_ptr<pdcch_processor_factory>      pdcch_proc_factory_,
                                             std::shared_ptr<pdsch_processor_factory>      pdsch_proc_factory_,
                                             std::shared_ptr<ssb_processor_factory>        ssb_proc_factory_,
                                             std::shared_ptr<nzp_csi_rs_generator_factory> nzp_csi_rs_factory_) :
    pdcch_proc_factory(pdcch_proc_factory_),
    pdsch_proc_factory(pdsch_proc_factory_),
    ssb_proc_factory(ssb_proc_factory_),
    nzp_csi_rs_factory(nzp_csi_rs_factory_)
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

    std::unique_ptr<nzp_csi_rs_generator> nzp_csi = nzp_csi_rs_factory->create();
    report_fatal_error_if_not(nzp_csi, "Invalid NZP-CSI-RS generator.");

    return std::make_unique<downlink_processor_single_executor_impl>(
        *config.gateway, std::move(pdcch), std::move(pdsch), std::move(ssb), std::move(nzp_csi), *config.executor);
  }

  // See interface for documentation.
  std::unique_ptr<downlink_processor>
  create(const downlink_processor_config& config, srslog::basic_logger& logger, bool enable_broadcast) override
  {
    std::unique_ptr<pdcch_processor> pdcch = pdcch_proc_factory->create(logger, enable_broadcast);
    report_fatal_error_if_not(pdcch, "Invalid PDCCH processor.");

    std::unique_ptr<pdsch_processor> pdsch = pdsch_proc_factory->create(logger, enable_broadcast);
    report_fatal_error_if_not(pdsch, "Invalid PDSCH processor.");

    std::unique_ptr<ssb_processor> ssb;
    if (enable_broadcast) {
      ssb = ssb_proc_factory->create(logger);
    } else {
      ssb = ssb_proc_factory->create();
    }
    report_fatal_error_if_not(ssb, "Invalid SSB processor.");

    std::unique_ptr<nzp_csi_rs_generator> nzp_csi;
    if (enable_broadcast) {
      nzp_csi = nzp_csi_rs_factory->create(logger);
    } else {
      nzp_csi = nzp_csi_rs_factory->create();
    }
    report_fatal_error_if_not(nzp_csi, "Invalid NZP-CSI-RS generator.");

    std::unique_ptr<downlink_processor> downlink_proc = std::make_unique<downlink_processor_single_executor_impl>(
        *config.gateway, std::move(pdcch), std::move(pdsch), std::move(ssb), std::move(nzp_csi), *config.executor);

    return std::make_unique<logging_downlink_processor_decorator>(std::move(downlink_proc), logger);
  }

  std::unique_ptr<downlink_pdu_validator> create_pdu_validator() override
  {
    return std::make_unique<downlink_processor_validator_impl>(ssb_proc_factory->create_validator(),
                                                               pdcch_proc_factory->create_validator(),
                                                               pdsch_proc_factory->create_validator(),
                                                               nzp_csi_rs_factory->create_validator());
  }

private:
  std::shared_ptr<pdcch_processor_factory>      pdcch_proc_factory;
  std::shared_ptr<pdsch_processor_factory>      pdsch_proc_factory;
  std::shared_ptr<ssb_processor_factory>        ssb_proc_factory;
  std::shared_ptr<nzp_csi_rs_generator_factory> nzp_csi_rs_factory;
};

static std::unique_ptr<downlink_processor_pool>
create_downlink_processor_pool(std::shared_ptr<downlink_processor_factory> factory, const upper_phy_config& config)
{
  srslog::basic_logger& dl_phy_logger = srslog::fetch_basic_logger("DL-PHY", true);
  dl_phy_logger.set_level(config.log_level);
  dl_phy_logger.set_hex_dump_max_size(config.logger_max_hex_size);

  downlink_processor_pool_config config_pool;
  config_pool.num_sectors = 1;

  for (unsigned numerology = 0, numerology_end = to_numerology_value(subcarrier_spacing::invalid);
       numerology != numerology_end;
       ++numerology) {
    // Skip SCS if not active.
    if (!config.active_scs[numerology]) {
      continue;
    }

    downlink_processor_pool_config::sector_dl_processor info = {0, to_subcarrier_spacing(numerology), {}};

    for (unsigned i_proc = 0, nof_procs = config.nof_dl_processors; i_proc != nof_procs; ++i_proc) {
      downlink_processor_config processor_config;
      processor_config.id      = i_proc;
      processor_config.gateway = config.rg_gateway;

      // Assign an executor to each DL processor from the list in round-robin fashion.
      processor_config.executor = config.dl_executors[i_proc % config.dl_executors.size()];

      std::unique_ptr<downlink_processor> dl_proc;
      if (config.log_level == srslog::basic_levels::none) {
        dl_proc = factory->create(processor_config);
      } else {
        // Fetch and configure logger.
        srslog::basic_logger& logger = srslog::fetch_basic_logger("DL-PHY" + std::to_string(i_proc), true);
        logger.set_level(config.log_level);
        logger.set_hex_dump_max_size(config.logger_max_hex_size);

        dl_proc = factory->create(processor_config, logger, config.enable_logging_broadcast);
      }
      report_fatal_error_if_not(dl_proc, "Invalid downlink processor.");
      info.procs.push_back(std::move(dl_proc));
    }

    config_pool.dl_processors.push_back(std::move(info));
  }

  return create_dl_processor_pool(std::move(config_pool));
}

static std::unique_ptr<resource_grid_pool>
create_dl_resource_grid_pool(const upper_phy_config& config, std::shared_ptr<resource_grid_factory> rg_factory)
{
  // Configure one pool per upper PHY.
  report_fatal_error_if_not(rg_factory, "Invalid resource grid factory.");
  unsigned                                    nof_sectors = 1;
  unsigned                                    nof_slots   = config.nof_slots_dl_rg;
  std::vector<std::unique_ptr<resource_grid>> grids;
  grids.reserve(nof_sectors * nof_slots);
  for (unsigned sector_idx = 0; sector_idx != nof_sectors; ++sector_idx) {
    for (unsigned slot_id = 0; slot_id != nof_slots; ++slot_id) {
      std::unique_ptr<resource_grid> grid =
          rg_factory->create(config.nof_tx_ports, MAX_NSYMB_PER_SLOT, config.dl_bw_rb * NRE);
      report_fatal_error_if_not(grid, "Invalid resource grid.");
      grids.push_back(std::move(grid));
    }
  }

  return create_resource_grid_pool(nof_sectors, nof_slots, std::move(grids));
}

static std::unique_ptr<resource_grid_pool>
create_ul_resource_grid_pool(const upper_phy_config& config, std::shared_ptr<resource_grid_factory> rg_factory)
{
  report_fatal_error_if_not(rg_factory, "Invalid resource grid factory.");
  unsigned                                    nof_sectors = 1;
  unsigned                                    nof_slots   = config.nof_slots_ul_rg;
  std::vector<std::unique_ptr<resource_grid>> grids;
  grids.reserve(nof_sectors * nof_slots);
  for (unsigned sector_idx = 0; sector_idx != nof_sectors; ++sector_idx) {
    for (unsigned slot_id = 0; slot_id != nof_slots; ++slot_id) {
      std::unique_ptr<resource_grid> grid =
          rg_factory->create(config.nof_rx_ports, MAX_NSYMB_PER_SLOT, config.ul_bw_rb * NRE);
      report_fatal_error_if_not(grid, "Invalid resource grid.");
      grids.push_back(std::move(grid));
    }
  }

  // Create UL resource grid pool.
  return create_resource_grid_pool(nof_sectors, nof_slots, std::move(grids));
}

static std::shared_ptr<uplink_processor_factory> create_ul_processor_factory(const upper_phy_config& config)
{
  std::shared_ptr<dft_processor_factory> dft_factory = create_dft_processor_factory_fftw_fast();
  if (!dft_factory) {
    dft_factory = create_dft_processor_factory_generic();
    report_fatal_error_if_not(dft_factory, "Invalid DFT factory.");
  }

  std::shared_ptr<prach_generator_factory> prach_gen_factory = create_prach_generator_factory_sw();
  report_fatal_error_if_not(prach_gen_factory, "Invalid PRACH generator factory.");

  std::shared_ptr<prach_detector_factory> prach_factory =
      create_prach_detector_factory_sw(dft_factory, prach_gen_factory);
  report_fatal_error_if_not(prach_factory, "Invalid PRACH detector factory.");

  /// PUSCH FACTORY.

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  std::shared_ptr<port_channel_estimator_factory>  ch_estimator_factory =
      create_port_channel_estimator_factory_sw(dft_factory);

  std::shared_ptr<channel_equalizer_factory>  equalizer_factory    = create_channel_equalizer_factory_zf();
  std::shared_ptr<channel_modulation_factory> demodulation_factory = create_channel_modulation_sw_factory();

  std::shared_ptr<crc_calculator_factory> crc_calc_factory =
      create_crc_calculator_factory_sw(config.crc_calculator_type);
  report_fatal_error_if_not(crc_calc_factory, "Invalid CRC calculator factory of type {}.", config.crc_calculator_type);

  pusch_decoder_factory_sw_configuration decoder_config;
  decoder_config.crc_factory     = crc_calc_factory;
  decoder_config.decoder_factory = create_ldpc_decoder_factory_sw(config.ldpc_decoder_type);
  report_fatal_error_if_not(
      decoder_config.decoder_factory, "Invalid LDPC decoder factory of type {}.", config.crc_calculator_type);
  decoder_config.dematcher_factory = create_ldpc_rate_dematcher_factory_sw(config.ldpc_rate_dematcher_type);
  report_fatal_error_if_not(decoder_config.dematcher_factory,
                            "Invalid LDPC Rate Dematcher factory of type {}.",
                            config.ldpc_rate_dematcher_type);
  decoder_config.segmenter_factory = create_ldpc_segmenter_rx_factory_sw();
  report_fatal_error_if_not(decoder_config.segmenter_factory, "Invalid LDPC Rx segmenter factory.");
  decoder_config.nof_pusch_decoder_threads = config.nof_pusch_decoder_threads;
  decoder_config.executor                  = config.pusch_decoder_executor;

  std::shared_ptr<short_block_detector_factory> short_block_det_factory = create_short_block_detector_factory_sw();
  report_fatal_error_if_not(short_block_det_factory, "Invalid short block detector factory.");

  std::shared_ptr<polar_factory> polar_dec_factory = create_polar_factory_sw();
  report_fatal_error_if_not(polar_dec_factory, "Invalid polar decoder factory.");

  std::shared_ptr<uci_decoder_factory> uci_dec_factory =
      create_uci_decoder_factory_sw(short_block_det_factory, polar_dec_factory, crc_calc_factory);
  report_fatal_error_if_not(uci_dec_factory, "Invalid UCI decoder factory.");

  // Enable EVM calculation if PUSCH SINR is obtained from EVM or if it is logged by the PHY.
  bool enable_evm = (config.pusch_sinr_calc_method == channel_state_information::sinr_type::evm) ||
                    (config.log_level == srslog::basic_levels::debug);

  // Enable post-equalization SINR if selected as PUSCH SINR method or if it is logged by the PHY.
  bool enable_eq_sinr = (config.pusch_sinr_calc_method == channel_state_information::sinr_type::post_equalization) ||
                        (config.log_level == srslog::basic_levels::debug);

  pusch_processor_factory_sw_configuration pusch_config;
  pusch_config.estimator_factory   = create_dmrs_pusch_estimator_factory_sw(prg_factory, ch_estimator_factory);
  pusch_config.demodulator_factory = create_pusch_demodulator_factory_sw(
      equalizer_factory, demodulation_factory, prg_factory, enable_evm, enable_eq_sinr);
  pusch_config.demux_factory         = create_ulsch_demultiplex_factory_sw();
  pusch_config.decoder_factory       = create_pusch_decoder_factory_sw(decoder_config);
  pusch_config.uci_dec_factory       = uci_dec_factory;
  pusch_config.dec_nof_iterations    = config.ldpc_decoder_iterations;
  pusch_config.dec_enable_early_stop = config.ldpc_decoder_early_stop;
  pusch_config.csi_sinr_calc_method  = config.pusch_sinr_calc_method;

  // :TODO: check these values in the future. Extract them to more public config.
  pusch_config.ch_estimate_dimensions.nof_symbols   = 14;
  pusch_config.ch_estimate_dimensions.nof_tx_layers = 1;
  pusch_config.ch_estimate_dimensions.nof_prb       = config.ul_bw_rb;
  pusch_config.ch_estimate_dimensions.nof_rx_ports  = config.nof_rx_ports;

  std::shared_ptr<pusch_processor_factory> pusch_factory = create_pusch_processor_factory_sw(pusch_config);
  report_fatal_error_if_not(pusch_factory, "Invalid PUSCH processor factory.");

  // Create PUSCH processor pool factory.
  pusch_factory = create_pusch_processor_pool(std::move(pusch_factory), config.max_pusch_concurrency);
  report_fatal_error_if_not(pusch_factory, "Invalid PUSCH processor pool factory.");

  std::shared_ptr<low_papr_sequence_generator_factory>  lpg_factory = create_low_papr_sequence_generator_sw_factory();
  std::shared_ptr<low_papr_sequence_collection_factory> lpc_factory =
      create_low_papr_sequence_collection_sw_factory(lpg_factory);

  // Create channel estimator factory.
  std::shared_ptr<port_channel_estimator_factory> port_chan_estimator_factory =
      create_port_channel_estimator_factory_sw(dft_factory);
  report_fatal_error_if_not(port_chan_estimator_factory, "Invalid port channel estimator factory.");

  std::shared_ptr<dmrs_pucch_estimator_factory> pucch_dmrs_factory =
      create_dmrs_pucch_estimator_factory_sw(prg_factory, lpc_factory, port_chan_estimator_factory);
  report_fatal_error_if_not(pucch_dmrs_factory, "Invalid PUCCH DM-RS estimator factory.");

  std::shared_ptr<pseudo_random_generator_factory> pseudorandom = create_pseudo_random_generator_sw_factory();
  std::shared_ptr<pucch_detector_factory>          pucch_detector_fact =
      create_pucch_detector_factory_sw(lpc_factory, pseudorandom, equalizer_factory);
  report_fatal_error_if_not(pucch_detector_fact, "Invalid PUCCH detector factory.");

  // Create PUCCH demodulator factory.
  std::shared_ptr<pucch_demodulator_factory> pucch_demod_factory =
      create_pucch_demodulator_factory_sw(equalizer_factory, demodulation_factory, prg_factory);
  report_fatal_error_if_not(pucch_demod_factory, "Invalid PUCCH demodulator factory.");

  channel_estimate::channel_estimate_dimensions channel_estimate_dimensions;
  channel_estimate_dimensions.nof_tx_layers = 1;
  channel_estimate_dimensions.nof_rx_ports  = config.nof_rx_ports;
  channel_estimate_dimensions.nof_symbols   = MAX_NSYMB_PER_SLOT;
  channel_estimate_dimensions.nof_prb       = config.ul_bw_rb;

  std::shared_ptr<pucch_processor_factory> pucch_factory = create_pucch_processor_factory_sw(
      pucch_dmrs_factory, pucch_detector_fact, pucch_demod_factory, uci_dec_factory, channel_estimate_dimensions);
  report_fatal_error_if_not(pucch_factory, "Invalid PUCCH processor factory.");

  // Create base factory.
  std::shared_ptr<uplink_processor_factory> factory = std::make_shared<uplink_processor_base_factory>(
      std::move(pucch_factory), std::move(pusch_factory), std::move(prach_factory));
  report_fatal_error_if_not(factory, "Invalid Uplink processor factory.");

  // Create logger decorator processor factory.
  factory = std::make_shared<uplink_processor_logging_decorator_factory>(
      config.log_level, config.logger_max_hex_size, std::move(factory));
  report_fatal_error_if_not(factory, "Invalid Uplink processor factory.");

  // Create concurrent processor factory.
  factory = std::make_shared<uplink_processor_concurrent_factory>(config.max_ul_thread_concurrency, std::move(factory));
  report_fatal_error_if_not(factory, "Invalid Uplink processor factory.");

  // Create task dispatcher processor factory.
  factory = std::make_shared<uplink_processor_task_dispatcher_factory>(
      std::move(factory), *config.pucch_executor, *config.pusch_executor, *config.prach_executor);
  report_fatal_error_if_not(factory, "Invalid Uplink processor factory.");

  return factory;
}

static std::unique_ptr<uplink_processor_pool> create_ul_processor_pool(uplink_processor_factory& factory,
                                                                       const upper_phy_config&   config)
{
  uplink_processor_pool_config config_pool;
  config_pool.num_sectors = 1;

  // Fetch and configure logger.
  srslog::basic_logger& logger = srslog::fetch_basic_logger("UL-PHY", true);
  logger.set_level(config.log_level);
  logger.set_hex_dump_max_size(config.logger_max_hex_size);

  for (unsigned scs = 0, scs_end = static_cast<unsigned>(subcarrier_spacing::invalid); scs != scs_end; ++scs) {
    // Skip SCS if not active.
    if (!config.active_scs[scs]) {
      continue;
    }

    uplink_processor_pool_config::sector_ul_processors info = {0, to_subcarrier_spacing(scs), {}};

    for (unsigned i = 0, e = config.nof_ul_processors; i != e; ++i) {
      std::unique_ptr<uplink_processor> ul_proc;
      if (config.log_level != srslog::basic_levels::none) {
        ul_proc = factory.create({}, logger, config.enable_logging_broadcast);
      } else {
        ul_proc = factory.create({});
      }
      report_fatal_error_if_not(ul_proc, "Invalid uplink processor.");

      info.procs.push_back(std::move(ul_proc));
    }

    config_pool.ul_processors.push_back(std::move(info));
  }

  return create_uplink_processor_pool(std::move(config_pool));
}

static std::unique_ptr<prach_buffer_pool> create_prach_pool(const upper_phy_config& config)
{
  std::vector<std::unique_ptr<prach_buffer>> prach_mem;
  prach_mem.reserve(config.nof_prach_buffer);

  for (unsigned i = 0, e = config.nof_prach_buffer; i != e; ++i) {
    std::unique_ptr<prach_buffer> buffer;

    if (config.is_prach_long_format) {
      buffer = create_prach_buffer_long(config.nof_rx_ports, config.max_nof_fd_prach_occasions);
    } else {
      buffer = create_prach_buffer_short(
          config.nof_rx_ports, config.max_nof_td_prach_occasions, config.max_nof_fd_prach_occasions);
    }

    report_fatal_error_if_not(buffer, "Invalid PRACH buffer.");
    prach_mem.push_back(std::move(buffer));
  }

  return create_prach_buffer_pool(std::move(prach_mem));
}

class upper_phy_factory_impl : public upper_phy_factory
{
public:
  upper_phy_factory_impl(std::shared_ptr<downlink_processor_factory> downlink_proc_factory_,
                         std::shared_ptr<resource_grid_factory>      rg_factory_) :
    downlink_proc_factory(std::move(downlink_proc_factory_)), rg_factory(std::move(rg_factory_))
  {
    srsran_assert(downlink_proc_factory, "Invalid downlink processor factory.");
    srsran_assert(rg_factory, "Invalid resource grid factory.");
  }

  std::unique_ptr<upper_phy> create(const upper_phy_config& config) override
  {
    upper_phy_impl_config phy_config;
    phy_config.sector_id                   = config.sector_id;
    phy_config.ul_bw_rb                    = config.ul_bw_rb;
    phy_config.log_level                   = config.log_level;
    phy_config.rx_symbol_printer_filename  = config.rx_symbol_printer_filename;
    phy_config.rx_symbol_request_notifier  = config.rx_symbol_request_notifier;
    phy_config.nof_slots_ul_pdu_repository = config.nof_ul_processors * 2;

    phy_config.dl_rg_pool = create_dl_resource_grid_pool(config, rg_factory);
    report_fatal_error_if_not(phy_config.dl_rg_pool, "Invalid downlink resource grid pool.");

    std::shared_ptr<uplink_processor_factory> ul_processor_fact = create_ul_processor_factory(config);

    phy_config.ul_rg_pool = create_ul_resource_grid_pool(config, rg_factory);
    report_fatal_error_if_not(phy_config.ul_rg_pool, "Invalid uplink resource grid pool.");

    phy_config.dl_processor_pool = create_downlink_processor_pool(downlink_proc_factory, config);
    report_fatal_error_if_not(phy_config.dl_processor_pool, "Invalid downlink processor pool.");

    phy_config.softbuffer_pool = create_rx_softbuffer_pool(config.softbuffer_config);
    report_fatal_error_if_not(phy_config.softbuffer_pool, "Invalid softbuffer processor pool.");

    phy_config.ul_processor_pool = create_ul_processor_pool(*ul_processor_fact, config);
    report_fatal_error_if_not(phy_config.ul_processor_pool, "Invalid uplink processor pool.");

    phy_config.prach_pool = create_prach_pool(config);
    report_fatal_error_if_not(phy_config.prach_pool, "Invalid PRACH buffer pool.");

    // Create the validators.
    phy_config.dl_pdu_validator = downlink_proc_factory->create_pdu_validator();
    phy_config.ul_pdu_validator = ul_processor_fact->create_pdu_validator();

    phy_config.timing_handler_executor = config.pusch_executor;

    return std::make_unique<upper_phy_impl>(std::move(phy_config));
  }

private:
  std::shared_ptr<downlink_processor_factory> downlink_proc_factory;
  std::shared_ptr<resource_grid_factory>      rg_factory;
};

} // namespace

std::shared_ptr<downlink_processor_factory>
srsran::create_downlink_processor_factory_sw(const downlink_processor_factory_sw_config& config)
{
  // Create channel coding factories - CRC
  std::shared_ptr<crc_calculator_factory> crc_calc_factory =
      create_crc_calculator_factory_sw(config.crc_calculator_type);
  report_fatal_error_if_not(crc_calc_factory, "Invalid CRC calculator factory of type {}.", config.crc_calculator_type);

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
  report_fatal_error_if_not(polar_factory, "Invalid POLAR factory.");

  // Create sequence generators factories - PRG
  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  report_fatal_error_if_not(prg_factory, "Invalid PRG factory.");

  // Create modulation mapper factory.
  std::shared_ptr<channel_modulation_factory> mod_factory = create_channel_modulation_sw_factory();
  report_fatal_error_if_not(mod_factory, "Invalid modulation factory.");

  // Create channel processors encoder factories - PBCH
  std::shared_ptr<pbch_encoder_factory> pbch_enc_factory =
      create_pbch_encoder_factory_sw(crc_calc_factory, prg_factory, polar_factory);
  report_fatal_error_if_not(pbch_enc_factory, "Invalid PBCH encoder factory.");

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
  std::shared_ptr<pdsch_processor_factory> pdsch_proc_factory;
  if (variant_holds_alternative<pdsch_processor_generic_configuration>(config.pdsch_processor)) {
    pdsch_proc_factory =
        create_pdsch_processor_factory_sw(pdsch_enc_factory, pdsch_mod_factory, dmrs_pdsch_proc_factory);
  } else if (variant_holds_alternative<pdsch_processor_concurrent_configuration>(config.pdsch_processor)) {
    const pdsch_processor_concurrent_configuration& pdsch_processor_config =
        variant_get<pdsch_processor_concurrent_configuration>(config.pdsch_processor);
    report_fatal_error_if_not(pdsch_processor_config.nof_pdsch_codeblock_threads >= 2,
                              "The number of threads (i.e., {}) must be equal to or greater than 2.");
    report_fatal_error_if_not(pdsch_processor_config.pdsch_codeblock_task_executor != nullptr,
                              "Invalid codeblock executor.");

    // Create concurrent PDSCH processor factory base.
    pdsch_proc_factory =
        create_pdsch_concurrent_processor_factory_sw(crc_calc_factory,
                                                     ldpc_enc_factory,
                                                     ldpc_rm_factory,
                                                     prg_factory,
                                                     mod_factory,
                                                     dmrs_pdsch_proc_factory,
                                                     *pdsch_processor_config.pdsch_codeblock_task_executor,
                                                     pdsch_processor_config.nof_pdsch_codeblock_threads);
    report_fatal_error_if_not(pdsch_proc_factory, "Invalid PDSCH processor factory.");

    // Wrap PDSCH processor factory with a pool to allow concurrent execution.
    pdsch_proc_factory =
        create_pdsch_processor_pool(std::move(pdsch_proc_factory), pdsch_processor_config.max_nof_simultaneous_pdsch);
  } else if (variant_holds_alternative<pdsch_processor_lite_configuration>(config.pdsch_processor)) {
    pdsch_proc_factory = create_pdsch_lite_processor_factory_sw(
        ldpc_seg_tx_factory, ldpc_enc_factory, ldpc_rm_factory, prg_factory, mod_factory, dmrs_pdsch_proc_factory);
  }
  report_fatal_error_if_not(pdsch_proc_factory, "Invalid PDSCH processor factory.");

  // Create channel processors - SSB
  ssb_processor_factory_sw_configuration ssb_factory_config;
  ssb_factory_config.encoder_factory                      = pbch_enc_factory;
  ssb_factory_config.modulator_factory                    = pbch_mod_factory;
  ssb_factory_config.dmrs_factory                         = dmrs_pbch_proc_factory;
  ssb_factory_config.pss_factory                          = pss_proc_factory;
  ssb_factory_config.sss_factory                          = sss_proc_factory;
  std::shared_ptr<ssb_processor_factory> ssb_proc_factory = create_ssb_processor_factory_sw(ssb_factory_config);
  report_fatal_error_if_not(ssb_proc_factory, "Invalid SSB processor factory.");

  // Create signal generators - NZP-CSI-RS
  std::shared_ptr<nzp_csi_rs_generator_factory> nzp_csi_rs_factory =
      create_nzp_csi_rs_generator_factory_sw(prg_factory);
  report_fatal_error_if_not(nzp_csi_rs_factory, "Invalid NZP-CSI-RS generator factory.");

  return std::make_shared<downlink_processor_single_executor_factory>(
      pdcch_proc_factory, pdsch_proc_factory, ssb_proc_factory, nzp_csi_rs_factory);
}

std::shared_ptr<downlink_processor_factory>
srsran::create_downlink_processor_factory_hw(const downlink_processor_factory_hw_config& config)
{
  // Create channel coding factories - CRC
  std::shared_ptr<crc_calculator_factory> crc_calc_factory = config.crc_calc_factory;
  report_fatal_error_if_not(crc_calc_factory, "Invalid CRC calculator factory.");

  // Create channel coding factories - Polar
  std::shared_ptr<polar_factory> polar_factory = create_polar_factory_sw();
  report_fatal_error_if_not(polar_factory, "Invalid POLAR factory.");

  // Create sequence generators factories - PRG
  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  report_fatal_error_if_not(prg_factory, "Invalid PRG factory.");

  // Create modulation mapper factory.
  std::shared_ptr<channel_modulation_factory> mod_factory = create_channel_modulation_sw_factory();
  report_fatal_error_if_not(mod_factory, "Invalid modulation factory.");

  // Create channel processors encoder factories - PBCH
  std::shared_ptr<pbch_encoder_factory> pbch_enc_factory =
      create_pbch_encoder_factory_sw(crc_calc_factory, prg_factory, polar_factory);
  report_fatal_error_if_not(pbch_enc_factory, "Invalid PBCH encoder factory.");

  // Create channel processors encoder factories - PDCCH
  std::shared_ptr<pdcch_encoder_factory> pdcch_enc_factory =
      create_pdcch_encoder_factory_sw(crc_calc_factory, polar_factory);
  report_fatal_error_if_not(pdcch_enc_factory, "Invalid PDCCH encoder factory.");

  std::shared_ptr<pdsch_encoder_factory> pdsch_enc_factory = config.pdsch_enc_factory;
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
  report_fatal_error_if_not(ssb_proc_factory, "Invalid SSB processor factory.");

  // Create signal generators - NZP-CSI-RS
  std::shared_ptr<nzp_csi_rs_generator_factory> nzp_csi_rs_factory =
      create_nzp_csi_rs_generator_factory_sw(prg_factory);
  report_fatal_error_if_not(nzp_csi_rs_factory, "Invalid NZP-CSI-RS generator factory.");

  return std::make_shared<downlink_processor_single_executor_factory>(
      pdcch_proc_factory, pdsch_proc_factory, ssb_proc_factory, nzp_csi_rs_factory);
}

std::unique_ptr<uplink_processor_pool> srsran::create_uplink_processor_pool(uplink_processor_pool_config config)
{
  // Convert from pool config to pool_impl config.
  uplink_processor_pool_impl_config ul_processors;
  ul_processors.num_sectors = config.num_sectors;

  for (auto& proc : config.ul_processors) {
    ul_processors.procs.push_back({proc.sector, proc.scs, std::move(proc.procs)});
  }

  return std::make_unique<uplink_processor_pool_impl>(std::move(ul_processors));
}

std::unique_ptr<downlink_processor_pool> srsran::create_dl_processor_pool(downlink_processor_pool_config config)
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
srsran::create_upper_phy_factory(std::shared_ptr<downlink_processor_factory> downlink_proc_factory,
                                 std::shared_ptr<resource_grid_factory>      rg_factory)
{
  return std::make_unique<upper_phy_factory_impl>(downlink_proc_factory, rg_factory);
}
