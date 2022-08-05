/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "upper_phy_ssb_example.h"
#include "srsgnb/phy/support/support_factories.h"
#include "srsgnb/phy/upper/channel_modulation/channel_modulation_factories.h"
#include "srsgnb/phy/upper/channel_processors/channel_processor_factories.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_encoder.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_modulator.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_processor.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_modulator.h"
#include "srsgnb/phy/upper/channel_processors/pdsch_processor.h"
#include "srsgnb/phy/upper/channel_processors/ssb_processor.h"
#include "srsgnb/phy/upper/signal_processors/pss_processor.h"
#include "srsgnb/phy/upper/signal_processors/signal_processor_factories.h"
#include "srsgnb/phy/upper/signal_processors/sss_processor.h"
#include <condition_variable>
#include <mutex>

using namespace srsgnb;

namespace {

class upper_phy_example_sw : public upper_phy_ssb_example
{
private:
  srslog::basic_logger&               logger;
  std::mutex                          mutex;
  std::condition_variable             cvar_tti_boundary;
  bool                                tti_boundary = false;
  bool                                quit         = false;
  std::unique_ptr<resource_grid_pool> dl_rg_pool;
  std::unique_ptr<ssb_processor>      ssb;
  upper_phy_rg_gateway*               gateway;
  ssb_configuration                   ssb_config;

public:
  /// Default constructor.
  upper_phy_example_sw(srslog::basic_logger&               logger_,
                       std::unique_ptr<resource_grid_pool> dl_rg_pool_,
                       std::unique_ptr<ssb_processor>      ssb_,
                       upper_phy_rg_gateway*               gateway_,
                       const ssb_configuration&            ssb_config_) :
    logger(logger_),
    dl_rg_pool(std::move(dl_rg_pool_)),
    ssb(std::move(ssb_)),
    gateway(gateway_),
    ssb_config(ssb_config_)
  {
    srsgnb_assert(dl_rg_pool, "Invalid RG pool.");
    srsgnb_assert(ssb, "Invalid SSB processor.");
    srsgnb_assert(gateway, "Invalid RG gateway.");
    srsgnb_assert(ssb_config.period_ms, "SSB period cannot be 0 ms.");
    srsgnb_assert(ssb_config.period_ms % 5 == 0, "SSB period ({}) must be multiple of 5 ms.", ssb_config.period_ms);
  }

  void handle_tti_boundary(const upper_phy_timing_context& context) override
  {
    std::unique_lock<std::mutex> lock(mutex);
    srsgnb_assert(gateway, "Upper PHY is not connected to a gateway.");

    // Set logger context.
    logger.set_context(context.slot.system_slot());
    logger.debug("New TTI boundary.");

    // Wait for TTI boundary to be cleared.
    while (tti_boundary && !quit) {
      cvar_tti_boundary.wait_for(lock, std::chrono::milliseconds(1));
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

        ssb->process(pdu, rg);
        logger.info("SSB: phys_cell_id={}; ssb_idx={};", pdu.phys_cell_id, pdu.ssb_idx);
      }
    }
    gateway->send(rg_context, rg);

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
    while (!tti_boundary && !quit) {
      cvar_tti_boundary.wait_for(lock, std::chrono::milliseconds(1));
    }

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

namespace srsgnb {

struct pdcch_processor_config_t {
  std::unique_ptr<pdcch_encoder>        encoder;
  std::unique_ptr<pdcch_modulator>      modulator;
  std::unique_ptr<dmrs_pdcch_processor> dmrs;
};
std::unique_ptr<pdcch_processor> create_pdcch_processor(pdcch_processor_config_t& config);

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
  } while (false)

std::unique_ptr<upper_phy_ssb_example> srsgnb::upper_phy_ssb_example::create(const configuration& config)
{
  srslog::basic_logger& logger = srslog::fetch_basic_logger("UpperPHY", false);
  logger.set_level(srslog::str_to_basic_level(config.log_level));

  std::shared_ptr<channel_modulation_factory> modulator_factory = create_channel_modulation_sw_factory();
  ASSERT_FACTORY(modulator_factory);

  std::shared_ptr<pseudo_random_generator_factory> prg_factory = create_pseudo_random_generator_sw_factory();
  ASSERT_FACTORY(prg_factory);

  std::shared_ptr<pbch_modulator_factory> pbch_modulator_factory =
      create_pbch_modulator_factory_sw(modulator_factory, prg_factory);
  ASSERT_FACTORY(pbch_modulator_factory);

  // Create SSB processor.
  std::unique_ptr<ssb_processor> ssb = nullptr;
  {
    ssb_processor_config ssb_config;
    ssb_config.encoder   = create_pbch_encoder();
    ssb_config.modulator = pbch_modulator_factory->create();
    ssb_config.dmrs      = create_dmrs_pbch_processor();
    ssb_config.pss       = create_pss_processor();
    ssb_config.sss       = create_sss_processor();
    ssb                  = create_ssb_processor(ssb_config);
    ASSERT_FACTORY(ssb);
  }

  // Create DL resource grid pool configuration.
  std::unique_ptr<resource_grid_pool> dl_rg_pool = nullptr;
  {
    unsigned                                    nof_sectors = 1;
    unsigned                                    nof_slots   = config.rg_pool_size;
    std::vector<std::unique_ptr<resource_grid>> grids;
    grids.reserve(nof_sectors * nof_slots);

    for (unsigned sector_id = 0; sector_id != nof_sectors; ++sector_id) {
      for (unsigned slot_id = 0; slot_id != nof_slots; ++slot_id) {
        grids.push_back(create_resource_grid(config.max_nof_ports, MAX_NSYMB_PER_SLOT, config.max_nof_prb * NRE));
        ASSERT_FACTORY(grids.back());
      }
    }

    // Create UL resource grid pool.
    dl_rg_pool = create_resource_grid_pool(nof_sectors, nof_slots, std::move(grids));
    ASSERT_FACTORY(dl_rg_pool);
  }

  return std::make_unique<upper_phy_example_sw>(
      logger, std::move(dl_rg_pool), std::move(ssb), config.gateway, config.ssb_config);
}
