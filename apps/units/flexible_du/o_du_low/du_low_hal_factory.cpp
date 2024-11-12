/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_low_hal_factory.h"
#include "srsran/ran/sch/sch_constants.h"
#ifdef DPDK_FOUND
#include "srsran/hal/dpdk/bbdev/bbdev_acc_factory.h"
#include "srsran/hal/phy/upper/channel_processors/hw_accelerator_factories.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository_factory.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/hw_accelerator_factories.h"
#endif // DPDK_FOUND

using namespace srsran;

#ifdef DPDK_FOUND
std::shared_ptr<dpdk::bbdev_acc>
srsran::init_bbdev_hwacc(const bbdev_appconfig& bbdev_app_cfg, srslog::basic_logger& logger, unsigned nof_hwacc_dus)
{
  // Initialize the bbdev-based hardware accelerator, if required.
  static std::shared_ptr<dpdk::bbdev_acc> bbdev_accelerator = [bbdev_app_cfg, &logger, nof_hwacc_dus]() {
    // Intefacing to the bbdev-based hardware-accelerator.
    dpdk::bbdev_acc_configuration bbdev_config;
    bbdev_config.id = bbdev_app_cfg.id;
    if (bbdev_app_cfg.pdsch_enc && bbdev_app_cfg.pdsch_enc->nof_hwacc > 0) {
      bbdev_config.nof_ldpc_enc_lcores = nof_hwacc_dus * bbdev_app_cfg.pdsch_enc->nof_hwacc;
    }
    if (bbdev_app_cfg.pusch_dec && bbdev_app_cfg.pusch_dec->nof_hwacc > 0) {
      bbdev_config.nof_ldpc_dec_lcores = nof_hwacc_dus * bbdev_app_cfg.pusch_dec->nof_hwacc;
    }
    // If no msg_mbuf size is defined, a worst-case value will be used.
    bbdev_config.msg_mbuf_size = bbdev_app_cfg.msg_mbuf_size.value_or(RTE_BBDEV_LDPC_E_MAX_MBUF);
    // If no rm_mbuf size is defined, a worst-case value will be used.
    bbdev_config.rm_mbuf_size = bbdev_app_cfg.rm_mbuf_size.value_or(RTE_BBDEV_LDPC_E_MAX_MBUF);
    // If no number of mbufs is defined, a worst-case value will be used.
    bbdev_config.nof_mbuf = bbdev_app_cfg.nof_mbuf.value_or(static_cast<unsigned>(pow2(log2_ceil(MAX_NOF_SEGMENTS))));
    std::shared_ptr<dpdk::bbdev_acc> bbdev_acc = create_bbdev_acc(bbdev_config, logger);
    report_error_if_not(bbdev_acc, "Unable to open the {} hardware-accelerator.", bbdev_app_cfg.hwacc_type);
    return bbdev_acc;
  }();

  return bbdev_accelerator;
}
#endif // DPDK_FOUND

o_du_low_hal_dependencies srsran::make_du_low_hal_dependencies(const du_low_unit_config& du_low_unit_cfg,
                                                               unsigned                  nof_cells)
{
  o_du_low_hal_dependencies hal_dependencies;

  // Initialize hardware-accelerator (only once and if needed).
#ifdef DPDK_FOUND
  if (du_low_unit_cfg.hal_config && du_low_unit_cfg.hal_config->bbdev_hwacc &&
      !du_low_unit_cfg.hal_config->bbdev_hwacc->hwacc_type.empty()) {
    const bbdev_appconfig& bbdev_app_cfg = du_low_unit_cfg.hal_config->bbdev_hwacc.value();
    srslog::basic_logger&  hwacc_logger  = srslog::fetch_basic_logger("HWACC", false);
    hwacc_logger.set_level(du_low_unit_cfg.loggers.hal_level);

    // Create a hardware-accelerated PDSCH encoder factory (only if needed).
    if (bbdev_app_cfg.pdsch_enc && bbdev_app_cfg.pdsch_enc->nof_hwacc > 0) {
      hal::bbdev_hwacc_pdsch_enc_factory_configuration hwacc_pdsch_enc_cfg;
      hwacc_pdsch_enc_cfg.acc_type          = bbdev_app_cfg.hwacc_type;
      hwacc_pdsch_enc_cfg.bbdev_accelerator = init_bbdev_hwacc(bbdev_app_cfg, hwacc_logger, nof_cells);
      hwacc_pdsch_enc_cfg.cb_mode           = bbdev_app_cfg.pdsch_enc->cb_mode;
      // If no maximum buffer size is defined, a worst-case value will be used.
      hwacc_pdsch_enc_cfg.max_tb_size = bbdev_app_cfg.pdsch_enc->max_buffer_size.value_or(RTE_BBDEV_LDPC_E_MAX_MBUF);
      hwacc_pdsch_enc_cfg.dedicated_queue = bbdev_app_cfg.pdsch_enc->dedicated_queue;
      hal_dependencies.hw_encoder_factory = hal::create_bbdev_pdsch_enc_acc_factory(hwacc_pdsch_enc_cfg);
    }

    // // Create a hardware-accelerated PUSCH decoder factory (only if needed).
    if (bbdev_app_cfg.pusch_dec && bbdev_app_cfg.pusch_dec->nof_hwacc > 0) {
      hal::bbdev_hwacc_pusch_dec_factory_configuration         hwacc_pusch_dec_cfg;
      std::shared_ptr<hal::ext_harq_buffer_context_repository> harq_buffer_context = nullptr;
      hwacc_pusch_dec_cfg.acc_type                                                 = bbdev_app_cfg.hwacc_type;
      hwacc_pusch_dec_cfg.bbdev_accelerator = init_bbdev_hwacc(bbdev_app_cfg, hwacc_logger, nof_cells);
      hwacc_pusch_dec_cfg.ext_softbuffer    = bbdev_app_cfg.pusch_dec->ext_softbuffer;
      if (hwacc_pusch_dec_cfg.ext_softbuffer) {
        // Set up an external HARQ buffer context repository.
        unsigned nof_cbs            = bbdev_app_cfg.pusch_dec->harq_context_size.value_or(MAX_NOF_SEGMENTS);
        uint64_t ext_harq_buff_size = hwacc_pusch_dec_cfg.bbdev_accelerator->get_harq_buff_size_bytes();
        harq_buffer_context = hal::create_ext_harq_buffer_context_repository(nof_cbs, ext_harq_buff_size, false);
        report_error_if_not(harq_buffer_context,
                            "Unable to create the external HARQ buffer context for the {} hardware-accelerator.",
                            bbdev_app_cfg.hwacc_type);
        hwacc_pusch_dec_cfg.harq_buffer_context = harq_buffer_context;
      }
      hwacc_pusch_dec_cfg.dedicated_queue = bbdev_app_cfg.pusch_dec->dedicated_queue;
      hal_dependencies.hw_decoder_factory = hal::create_bbdev_pusch_dec_acc_factory(hwacc_pusch_dec_cfg);
    }
  }
#endif // DPDK_FOUND

  return hal_dependencies;
}