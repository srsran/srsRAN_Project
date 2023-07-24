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

#include "upper_phy_ssb_example.h"
#include "srsran/phy/support/prach_buffer.h"
#include "srsran/phy/support/prach_buffer_context.h"
#include "srsran/phy/support/resource_grid_mapper.h"
#include "srsran/phy/support/resource_grid_reader.h"
#include "srsran/phy/support/resource_grid_writer.h"
#include "srsran/phy/support/support_factories.h"
#include "srsran/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsran/phy/upper/channel_processors/ssb_processor.h"
#include "srsran/ran/precoding/precoding_codebooks.h"
#include "srsran/srsvec/bit.h"
#include <condition_variable>
#include <mutex>
#include <random>

using namespace srsran;

namespace {

class upper_phy_example_sw : public upper_phy_ssb_example
{
private:
  srslog::basic_logger&                 logger;
  std::mutex                            mutex;
  std::condition_variable               cvar_tti_boundary;
  bool                                  tti_boundary = false;
  bool                                  quit         = false;
  std::unique_ptr<resource_grid_pool>   dl_rg_pool;
  std::unique_ptr<resource_grid_pool>   ul_rg_pool;
  std::unique_ptr<prach_buffer>         prach_buf;
  std::unique_ptr<ssb_processor>        ssb;
  std::unique_ptr<modulation_mapper>    data_modulator;
  upper_phy_rg_gateway*                 gateway;
  upper_phy_rx_symbol_request_notifier* rx_symb_req_notifier;
  ssb_configuration                     ssb_config;
  bool                                  enable_random_data;
  bool                                  enable_ul_processing;
  bool                                  enable_prach_processing;
  modulation_scheme                     data_modulation;
  unsigned                              nof_subcs;
  unsigned                              nof_ports;
  std::mt19937                          rgen;

  // Pseudo-random data and symbol buffers.
  static constexpr unsigned MAX_NRE_PER_SLOT = MAX_NSYMB_PER_SLOT * MAX_RB * NRE;
  static_bit_buffer<MAX_NRE_PER_SLOT * MODULATION_MAX_BITS_PER_SYMBOL> data;
  static_re_buffer<1, MAX_NRE_PER_SLOT>                                data_symbols;

public:
  upper_phy_example_sw(srslog::basic_logger&                 logger_,
                       std::unique_ptr<resource_grid_pool>   dl_rg_pool_,
                       std::unique_ptr<resource_grid_pool>   ul_rg_pool_,
                       std::unique_ptr<ssb_processor>        ssb_,
                       std::unique_ptr<modulation_mapper>    data_modulator_,
                       upper_phy_rg_gateway*                 gateway_,
                       upper_phy_rx_symbol_request_notifier* rx_symb_req_notifier_,
                       const ssb_configuration&              ssb_config_,
                       bool                                  enable_random_data_,
                       bool                                  enable_ul_processing_,
                       bool                                  enable_prach_processing_,
                       modulation_scheme                     data_modulation_,
                       unsigned                              nof_subcs_,
                       unsigned                              nof_ports_) :
    logger(logger_),
    dl_rg_pool(std::move(dl_rg_pool_)),
    ul_rg_pool(std::move(ul_rg_pool_)),
    prach_buf(create_prach_buffer_short(1,
                                        prach_constants::MAX_NOF_PRACH_TD_OCCASIONS,
                                        prach_constants::MAX_NOF_PRACH_FD_OCCASIONS)),
    ssb(std::move(ssb_)),
    data_modulator(std::move(data_modulator_)),
    gateway(gateway_),
    rx_symb_req_notifier(rx_symb_req_notifier_),
    ssb_config(ssb_config_),
    enable_random_data(enable_random_data_),
    enable_ul_processing(enable_ul_processing_),
    enable_prach_processing(enable_prach_processing_),
    data_modulation(data_modulation_),
    nof_subcs(nof_subcs_),
    nof_ports(nof_ports_),
    rgen(0)
  {
    srsran_assert(dl_rg_pool, "Invalid DL RG pool.");
    srsran_assert(ul_rg_pool, "Invalid UL RG pool.");
    srsran_assert(ssb, "Invalid SSB processor.");
    srsran_assert(data_modulator, "Invalid modulation mapper.");
    srsran_assert(gateway, "Invalid RG gateway.");
    srsran_assert(rx_symb_req_notifier, "Invalid receive symbol request notifier.");
    srsran_assert(ssb_config.period_ms, "SSB period cannot be 0 ms.");
    srsran_assert(ssb_config.period_ms % 5 == 0, "SSB period ({}) must be multiple of 5 ms.", ssb_config.period_ms);
    srsran_assert(nof_subcs != 0, "Number of OFDM subcarriers cannot be zero.");
    srsran_assert(nof_ports_ != 0, "Number of antenna ports cannot be zero.");
  }

  void handle_tti_boundary(const upper_phy_timing_context& context) override
  {
    std::unique_lock<std::mutex> lock(mutex);
    srsran_assert(gateway, "Upper PHY is not connected to a gateway.");

    // Set logger context.
    logger.set_context(context.slot.sfn(), context.slot.slot_index());
    logger.debug("New TTI boundary.");

    // Wait for TTI boundary to be cleared.
    cvar_tti_boundary.wait(lock, [this]() { return ((!tti_boundary) || quit); });

    // Request RX symbol if UL processing is enabled.
    if (enable_ul_processing) {
      resource_grid_context rx_symb_context;
      rx_symb_context.sector = 0;
      rx_symb_context.slot   = context.slot;
      resource_grid& rg      = ul_rg_pool->get_resource_grid(rx_symb_context);
      rx_symb_req_notifier->on_uplink_slot_request(rx_symb_context, rg);
    }

    // Request PRACH capture if PRACH processing is enabled.
    if (enable_prach_processing && (context.slot.subframe_index() == 0)) {
      prach_buffer_context prach_context;
      prach_context.sector                = 0;
      prach_context.slot                  = context.slot;
      prach_context.port                  = 0;
      prach_context.start_symbol          = 0;
      prach_context.format                = prach_format_type::A1;
      prach_context.rb_offset             = 0;
      prach_context.nof_td_occasions      = 6;
      prach_context.nof_fd_occasions      = 1;
      prach_context.nof_prb_ul_grid       = nof_subcs / NRE;
      prach_context.pusch_scs             = to_subcarrier_spacing(context.slot.numerology());
      prach_context.root_sequence_index   = 0;
      prach_context.restricted_set        = restricted_set_config::UNRESTRICTED;
      prach_context.zero_correlation_zone = 0;
      prach_context.start_preamble_index  = 0;
      prach_context.nof_preamble_indices  = 64;
      rx_symb_req_notifier->on_prach_capture_request(prach_context, *prach_buf);
    }

    // Prepare resource grid context.
    resource_grid_context rg_context;
    rg_context.sector = 0;
    rg_context.slot   = context.slot;

    // Get a resource grid from the pool.
    resource_grid& rg = dl_rg_pool->get_resource_grid(rg_context);

    // Set all the RE to zero.
    rg.set_all_zero();

    // Calculate SSB period in half-radio frame.
    unsigned ssb_period_hrf = ssb_config.period_ms / 5;
    unsigned hrf_index      = context.slot.slot_index() / context.slot.nof_hrf_slots();
    unsigned nsymb_per_slot = get_nsymb_per_slot(ssb_config.cp);

    // Check if a SS/PBCH burst is active.
    if (hrf_index % ssb_period_hrf == 0) {
      // For each SSB candidate to transmit.
      for (unsigned ssb_idx : ssb_config.ssb_idx) {
        unsigned l_start = ssb_get_l_first(ssb_config.pattern_case, ssb_idx);
        if (l_start / nsymb_per_slot != context.slot.hrf_slot_index()) {
          continue;
        }

        ssb_processor::pdu_t pdu;
        pdu.slot              = context.slot;
        pdu.phys_cell_id      = ssb_config.phys_cell_id;
        pdu.beta_pss          = ssb_config.beta_pss_dB;
        pdu.ssb_idx           = ssb_idx;
        pdu.L_max             = ssb_config.L_max;
        pdu.common_scs        = to_subcarrier_spacing(ssb_config.pattern_case);
        pdu.subcarrier_offset = ssb_config.subcarrier_offset;
        pdu.offset_to_pointA  = ssb_config.offset_pointA;
        pdu.pattern_case      = ssb_config.pattern_case;
        pdu.bch_payload       = {};
        pdu.ports             = {0};

        ssb->process(rg.get_writer(), pdu);
        logger.info("SSB: phys_cell_id={}; ssb_idx={};", pdu.phys_cell_id, pdu.ssb_idx);
      }
    }

    // Fill the empty resource grids with pseudo-random data.
    if (rg.get_reader().is_empty(0) && enable_random_data) {
      unsigned mod_order = get_bits_per_symbol(data_modulation);

      unsigned nbits = MAX_NSYMB_PER_SLOT * nof_subcs * mod_order;
      data_symbols.resize(1, MAX_NSYMB_PER_SLOT * nof_subcs);

      // Generate the pseudo-random data bits.
      data.resize(nbits);
      for (unsigned i_byte = 0, i_byte_end = nbits / 8; i_byte != i_byte_end; ++i_byte) {
        data.set_byte(rgen() & mask_lsb_ones<unsigned>(8), i_byte);
      }
      data.insert(rgen() & mask_lsb_ones<unsigned>(nbits % 8), (nbits / 8) * 8, nbits % 8);

      // Modulate the data into symbols.
      data_modulator->modulate(data_symbols.get_slice(0), data, data_modulation);

      // Precoding configuration for data. It transmits the data REs over all ports.
      precoding_configuration precoding_config =
          precoding_configuration::precoding_configuration::make_wideband(make_one_layer_all_ports(nof_ports));

      // Contiguous allocation pattern in time and frequency.
      re_pattern grid_allocation(0, nof_subcs / NRE, 1, ~re_prb_mask(), ~symbol_slot_mask());

      // Map the data symbols to the grid.
      resource_grid_mapper& mapper = rg.get_mapper();
      mapper.map(data_symbols, grid_allocation, precoding_config);
    }

    gateway->send(rg_context, rg.get_reader());

    // Raise TTI boundary and notify.
    tti_boundary = true;
    cvar_tti_boundary.notify_all();
  }

  void handle_ul_half_slot_boundary(const upper_phy_timing_context& context) override
  {
    logger.debug("UL half slot boundary.");
  }

  void handle_ul_full_slot_boundary(const upper_phy_timing_context& context) override
  {
    logger.debug("UL full slot boundary.");
  }

  void wait_tti_boundary() override
  {
    std::unique_lock<std::mutex> lock(mutex);

    // Wait for TTI boundary to be raised.
    cvar_tti_boundary.wait(lock, [this]() { return (tti_boundary || quit); });

    // Clear TTI boundary and notify
    tti_boundary = false;
    cvar_tti_boundary.notify_all();
  }

  void stop() override
  {
    std::unique_lock<std::mutex> lock(mutex);
    quit = true;
    cvar_tti_boundary.notify_all();
  }
};

} // namespace

#define ASSERT_FACTORY(FACTORY_PTR)                                                                                    \
  do {                                                                                                                 \
    if (!FACTORY_PTR) {                                                                                                \
      logger.error("{}:{}: {}: Error creating {}.", __FILE__, __LINE__, __PRETTY_FUNCTION__, #FACTORY_PTR);            \
      return nullptr;                                                                                                  \
    }                                                                                                                  \
  } while (false)

std::unique_ptr<upper_phy_ssb_example> srsran::upper_phy_ssb_example::create(const configuration& config)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("UpperPHY", false);
  logger.set_level(srslog::str_to_basic_level(config.log_level));

  std::shared_ptr<crc_calculator_factory> crc_calc_factory = create_crc_calculator_factory_sw("lut");
  ASSERT_FACTORY(crc_calc_factory);

  std::shared_ptr<channel_modulation_factory> modulator_factory = create_channel_modulation_sw_factory();
  ASSERT_FACTORY(modulator_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  ASSERT_FACTORY(prg_factory);

  std::shared_ptr<polar_factory> polar_factory = create_polar_factory_sw();
  ASSERT_FACTORY(polar_factory);

  std::shared_ptr<pbch_encoder_factory> pbch_enc_factory =
      create_pbch_encoder_factory_sw(crc_calc_factory, prg_factory, polar_factory);
  ASSERT_FACTORY(polar_factory);

  std::shared_ptr<pbch_modulator_factory> pbch_mod_factory =
      create_pbch_modulator_factory_sw(modulator_factory, prg_factory);
  ASSERT_FACTORY(pbch_mod_factory);

  std::shared_ptr<dmrs_pbch_processor_factory> pbch_dmrs_factory = create_dmrs_pbch_processor_factory_sw(prg_factory);
  ASSERT_FACTORY(pbch_dmrs_factory);

  std::shared_ptr<pss_processor_factory> pss_factory = create_pss_processor_factory_sw();
  ASSERT_FACTORY(pss_factory);

  std::shared_ptr<sss_processor_factory> sss_factory = create_sss_processor_factory_sw();
  ASSERT_FACTORY(sss_factory);

  std::shared_ptr<channel_precoder_factory> precoder_factory = create_channel_precoder_factory("auto");
  ASSERT_FACTORY(precoder_factory);

  std::shared_ptr<resource_grid_factory> rg_factory = create_resource_grid_factory(precoder_factory);
  ASSERT_FACTORY(rg_factory);

  ssb_processor_factory_sw_configuration ssb_factory_config;
  ssb_factory_config.encoder_factory                 = pbch_enc_factory;
  ssb_factory_config.modulator_factory               = pbch_mod_factory;
  ssb_factory_config.dmrs_factory                    = pbch_dmrs_factory;
  ssb_factory_config.pss_factory                     = pss_factory;
  ssb_factory_config.sss_factory                     = sss_factory;
  std::shared_ptr<ssb_processor_factory> ssb_factory = create_ssb_processor_factory_sw(ssb_factory_config);
  ASSERT_FACTORY(ssb_factory);

  // Create SSB processor.
  std::unique_ptr<ssb_processor> ssb = ssb_factory->create();
  ASSERT_FACTORY(ssb);

  // Determine the number of subcarriers of the resource grid.
  unsigned nof_subcs = config.max_nof_prb * NRE;

  // Create DL and UL resource grid pool configuration.
  std::unique_ptr<resource_grid_pool> dl_rg_pool = nullptr;
  std::unique_ptr<resource_grid_pool> ul_rg_pool = nullptr;
  {
    unsigned                                    nof_sectors = 1;
    unsigned                                    nof_slots   = config.rg_pool_size;
    std::vector<std::unique_ptr<resource_grid>> dl_grids;
    std::vector<std::unique_ptr<resource_grid>> ul_grids;
    dl_grids.reserve(nof_sectors * nof_slots);
    ul_grids.reserve(nof_sectors * nof_slots);

    for (unsigned sector_id = 0; sector_id != nof_sectors; ++sector_id) {
      for (unsigned slot_id = 0; slot_id != nof_slots; ++slot_id) {
        dl_grids.push_back(rg_factory->create(config.max_nof_ports, MAX_NSYMB_PER_SLOT, nof_subcs));
        ASSERT_FACTORY(dl_grids.back());
        ul_grids.push_back(rg_factory->create(config.max_nof_ports, MAX_NSYMB_PER_SLOT, nof_subcs));
        ASSERT_FACTORY(ul_grids.back());
      }
    }

    // Create DL resource grid pool.
    dl_rg_pool = create_resource_grid_pool(nof_sectors, nof_slots, std::move(dl_grids));
    ASSERT_FACTORY(dl_rg_pool);

    // Create UL resource grid pool.
    ul_rg_pool = create_resource_grid_pool(nof_sectors, nof_slots, std::move(ul_grids));
    ASSERT_FACTORY(ul_rg_pool);
  }

  // Create modulation mapper for random data.
  std::shared_ptr<channel_modulation_factory> data_modulator_factory = create_channel_modulation_sw_factory();
  std::unique_ptr<modulation_mapper>          data_modulator = data_modulator_factory->create_modulation_mapper();

  return std::make_unique<upper_phy_example_sw>(logger,
                                                std::move(dl_rg_pool),
                                                std::move(ul_rg_pool),
                                                std::move(ssb),
                                                std::move(data_modulator),
                                                config.gateway,
                                                config.rx_symb_req_notifier,
                                                config.ssb_config,
                                                config.enable_random_data,
                                                config.enable_ul_processing,
                                                config.enable_prach_processing,
                                                config.data_modulation,
                                                nof_subcs,
                                                config.max_nof_ports);
}
