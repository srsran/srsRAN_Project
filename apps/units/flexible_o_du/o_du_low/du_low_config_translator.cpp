/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "du_low_config_translator.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "du_low_config.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/phy/upper/upper_phy_factories.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/prach/prach_configuration.h"

using namespace srsran;

static srs_du::du_low_config generate_du_low_config(const du_low_unit_config&                       du_low,
                                                    span<const o_du_low_unit_config::du_low_config> cells)
{
  srs_du::du_low_config out_config;
  out_config.cells.reserve(cells.size());

  unsigned max_ul_bw_rb         = 0;
  unsigned pusch_max_nof_layers = 0;
  unsigned max_nof_rx_antennas  = 0;
  for (unsigned i = 0, e = cells.size(); i != e; ++i) {
    const o_du_low_unit_config::du_low_config& cell = cells[i];
    max_ul_bw_rb                                    = std::max(cell.bw_rb, max_ul_bw_rb);
    pusch_max_nof_layers                            = std::max(cell.pusch_max_nof_layers, pusch_max_nof_layers);
    max_nof_rx_antennas                             = std::max(cell.nof_rx_antennas, max_nof_rx_antennas);
  }

  upper_phy_factory_configuration& upper_phy_factory_config = out_config.upper_phy_common_config;
  upper_phy_factory_config.log_level                        = du_low.loggers.phy_level;
  upper_phy_factory_config.enable_logging_broadcast         = du_low.loggers.broadcast_enabled;
  upper_phy_factory_config.logger_max_hex_size              = du_low.loggers.hex_max_size;
  upper_phy_factory_config.enable_metrics                   = du_low.metrics_cfg.enable_du_low;
  upper_phy_factory_config.pusch_sinr_calc_method =
      channel_state_information::sinr_type_from_string(du_low.expert_phy_cfg.pusch_sinr_calc_method);
  upper_phy_factory_config.rx_symbol_printer_filename = du_low.loggers.phy_rx_symbols_filename;
  upper_phy_factory_config.rx_symbol_printer_port     = du_low.loggers.phy_rx_symbols_port;
  upper_phy_factory_config.rx_symbol_printer_prach    = du_low.loggers.phy_rx_symbols_prach;
  upper_phy_factory_config.ldpc_encoder_type          = "auto";
  upper_phy_factory_config.ldpc_decoder_type          = "auto";
  upper_phy_factory_config.ldpc_rate_dematcher_type   = "auto";
  upper_phy_factory_config.crc_calculator_type        = "auto";
  upper_phy_factory_config.pusch_channel_estimator_fd_strategy =
      du_low.expert_phy_cfg.pusch_channel_estimator_fd_strategy;
  upper_phy_factory_config.pusch_channel_estimator_td_strategy =
      du_low.expert_phy_cfg.pusch_channel_estimator_td_strategy;
  upper_phy_factory_config.pusch_channel_estimator_compensate_cfo =
      du_low.expert_phy_cfg.pusch_channel_estimator_cfo_compensation;
  upper_phy_factory_config.pusch_channel_equalizer_algorithm = du_low.expert_phy_cfg.pusch_channel_equalizer_algorithm;
  upper_phy_factory_config.ldpc_decoder_iterations           = du_low.expert_phy_cfg.pusch_decoder_max_iterations;
  upper_phy_factory_config.ldpc_decoder_early_stop           = du_low.expert_phy_cfg.pusch_decoder_early_stop;
  upper_phy_factory_config.ldpc_decoder_force_decoding       = du_low.expert_phy_cfg.pusch_decoder_force_decoding;
  upper_phy_factory_config.nof_rx_ports                      = max_nof_rx_antennas;
  upper_phy_factory_config.ul_bw_rb                          = max_ul_bw_rb;
  upper_phy_factory_config.pusch_max_nof_layers              = pusch_max_nof_layers;
  upper_phy_factory_config.enable_metrics                    = du_low.metrics_cfg.enable_du_low;
  upper_phy_factory_config.ldpc_decoder_type                 = "auto";
  upper_phy_factory_config.enable_phy_tap                    = du_low.expert_phy_cfg.enable_phy_tap;

  // The flexible PDSCH processor implementation will be used by default.
  const auto& upper_phy_threads_cfg = du_low.expert_execution_cfg.threads;
  if ((upper_phy_threads_cfg.pdsch_processor_type == "auto") ||
      (upper_phy_threads_cfg.pdsch_processor_type == "flexible")) {
    // Set the batch size for synchronous operation by default. Override it if the PDSCH codeblock processor operation
    // is asynchronous.
    unsigned cb_batch_length = pdsch_processor_flexible_configuration::synchronous_cb_batch_length;
    if (upper_phy_threads_cfg.pdsch_cb_batch_length != du_low_unit_expert_threads_config::synchronous_cb_batch_length) {
      cb_batch_length = upper_phy_threads_cfg.pdsch_cb_batch_length;
    }

    // Emplace configuration parameters.
    upper_phy_factory_config.pdsch_processor.emplace<pdsch_processor_flexible_configuration>(
        pdsch_processor_flexible_configuration{.cb_batch_length = cb_batch_length});
  } else if (upper_phy_threads_cfg.pdsch_processor_type == "generic") {
    upper_phy_factory_config.pdsch_processor.emplace<pdsch_processor_generic_configuration>();
  }

  for (unsigned i = 0, e = cells.size(); i != e; ++i) {
    const o_du_low_unit_config::du_low_config& cell           = cells[i];
    upper_phy_configuration&                   upper_phy_cell = out_config.cells.emplace_back().upper_phy_cfg;

    // Get bandwidth in PRB.
    const unsigned bw_rb = cell.bw_rb;
    // Deduce the number of slots per subframe.
    const unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(cell.scs_common);
    // Deduce the number of slots per frame.
    unsigned nof_slots_per_frame = nof_slots_per_subframe * NOF_SUBFRAMES_PER_FRAME;
    // Number of slots per hyper system frame.
    unsigned nof_slots_per_hyper_system_frame = NOF_SFNS * nof_slots_per_frame;
    // PUSCH HARQ process lifetime in slots. It assumes the maximum lifetime is 100ms.
    unsigned expire_pusch_harq_timeout_slots =
        100 * nof_slots_per_subframe + du_low.expert_phy_cfg.max_processing_delay_slots;

    // Calculate the number of UL slots in a frame and in a PUSCH HARQ process lifetime.
    unsigned nof_ul_slots_in_harq_lifetime = expire_pusch_harq_timeout_slots;
    if (cell.duplex == duplex_mode::TDD && cell.tdd_pattern.has_value()) {
      const tdd_ul_dl_pattern& pattern1     = cell.tdd_pattern->pattern1;
      unsigned                 period_slots = pattern1.dl_ul_tx_period_nof_slots;
      unsigned                 nof_ul_slots = pattern1.nof_ul_slots + ((pattern1.nof_ul_symbols != 0) ? 1 : 0);
      if (cell.tdd_pattern->pattern2) {
        const tdd_ul_dl_pattern& pattern2 = *cell.tdd_pattern->pattern2;
        period_slots += pattern2.dl_ul_tx_period_nof_slots;
        nof_ul_slots += pattern2.nof_ul_slots + ((pattern2.nof_ul_symbols != 0) ? 1 : 0);
      }
      nof_ul_slots_in_harq_lifetime = divide_ceil(expire_pusch_harq_timeout_slots, period_slots) * nof_ul_slots;
    }

    // Deduce the maximum number of codeblocks that can be scheduled for PUSCH in one slot assuming:
    // - The maximum number of resource elements used for data for each scheduled resource block;
    // - the cell bandwidth;
    // - the highest modulation order possible; and
    // - the maximum coding rate.
    const unsigned max_nof_pusch_cb_slot =
        divide_ceil(pusch_constants::MAX_NRE_PER_RB * bw_rb * get_bits_per_symbol(modulation_scheme::QAM256),
                    ldpc::MAX_MESSAGE_SIZE);

    // Calculate the maximum number of active PUSCH HARQ processes from:
    // - the maximum number of users per slot; and
    // - the number of PUSCH occasions in a HARQ process lifetime.
    const unsigned nof_buffers = cell.max_puschs_per_slot * nof_ul_slots_in_harq_lifetime;

    // Calculate the maximum number of receive codeblocks. It is equal to the product of:
    // - the maximum number of codeblocks that can be scheduled in one slot; and
    // - the number of PUSCH occasions in a HARQ process lifetime.
    const unsigned max_rx_nof_codeblocks = nof_ul_slots_in_harq_lifetime * max_nof_pusch_cb_slot;

    // Determine processing downlink pipeline depth. Make sure the number of slots per system frame is divisible by the
    // pipeline depth.
    unsigned dl_pipeline_depth = 4 * du_low.expert_phy_cfg.max_processing_delay_slots;
    while (nof_slots_per_hyper_system_frame % dl_pipeline_depth != 0) {
      ++dl_pipeline_depth;
    }

    // The uplink pipeline depth is set equal to the number of slots per frame for reusing uplink processors every
    // 10 ms.
    unsigned ul_pipeline_depth = nof_slots_per_frame;

    static constexpr unsigned prach_pipeline_depth = 1;

    const prach_configuration prach_cfg =
        prach_configuration_get(cell.freq_range, cell.duplex, cell.prach_config_index);
    srsran_assert(prach_cfg.format != prach_format_type::invalid,
                  "Unsupported PRACH configuration index (i.e., {}) for the given frequency range (i.e., {}) and "
                  "duplex mode (i.e., {}).",
                  cell.prach_config_index,
                  to_string(cell.freq_range),
                  to_string(cell.duplex));

    upper_phy_cell.nof_tx_ports                                     = cell.nof_tx_antennas;
    upper_phy_cell.nof_rx_ports                                     = cell.nof_rx_antennas;
    upper_phy_cell.nof_dl_rg                                        = dl_pipeline_depth + 2;
    upper_phy_cell.nof_ul_rg                                        = ul_pipeline_depth;
    upper_phy_cell.nof_prach_buffer                                 = prach_pipeline_depth * nof_slots_per_subframe;
    upper_phy_cell.max_nof_td_prach_occasions                       = prach_cfg.nof_occasions_within_slot;
    upper_phy_cell.max_nof_fd_prach_occasions                       = 1;
    upper_phy_cell.is_prach_long_format                             = is_long_preamble(prach_cfg.format);
    upper_phy_cell.nof_dl_processors                                = dl_pipeline_depth;
    upper_phy_cell.dl_bw_rb                                         = bw_rb;
    upper_phy_cell.ul_bw_rb                                         = bw_rb;
    upper_phy_cell.pusch_max_nof_layers                             = cell.pusch_max_nof_layers;
    upper_phy_cell.active_scs                                       = {};
    upper_phy_cell.active_scs[to_numerology_value(cell.scs_common)] = true;
    upper_phy_cell.rx_buffer_config.nof_buffers                     = nof_buffers;
    upper_phy_cell.rx_buffer_config.nof_codeblocks                  = max_rx_nof_codeblocks;
    upper_phy_cell.rx_buffer_config.max_codeblock_size              = ldpc::MAX_CODEBLOCK_SIZE;
    upper_phy_cell.rx_buffer_config.expire_timeout_slots            = expire_pusch_harq_timeout_slots;
    upper_phy_cell.rx_buffer_config.external_soft_bits              = false;

    if (!is_valid_upper_phy_config(upper_phy_cell)) {
      report_error("Invalid upper PHY configuration.\n");
    }
  }

  return out_config;
}

void srsran::generate_o_du_low_config(srs_du::o_du_low_config&                        out_config,
                                      const du_low_unit_config&                       du_low_unit_cfg,
                                      span<const o_du_low_unit_config::du_low_config> cells)
{
  out_config.du_low_cfg = generate_du_low_config(du_low_unit_cfg, cells);
}

void srsran::fill_du_low_worker_manager_config(worker_manager_config&    config,
                                               const du_low_unit_config& unit_cfg,
                                               unsigned                  is_blocking_mode_active,
                                               span<const unsigned>      nof_dl_antennas,
                                               span<const unsigned>      nof_ul_antennas)
{
  report_error_if_not(nof_dl_antennas.size() == nof_ul_antennas.size(),
                      "The length of nof_dl_antennas ({}) must be equal to the length of nof_ul_antennas ({}), which "
                      "should match the number of cells.",
                      nof_dl_antennas.size(),
                      nof_ul_antennas.size());

  auto& du_low_cfg = config.du_low_cfg.emplace();

  du_low_cfg.is_sequential_mode_active = is_blocking_mode_active;
  du_low_cfg.cell_nof_dl_antennas.assign(nof_dl_antennas.begin(), nof_dl_antennas.end());
  du_low_cfg.cell_nof_ul_antennas.assign(nof_ul_antennas.begin(), nof_ul_antennas.end());

  unsigned max_pdsch_concurrency         = unit_cfg.expert_execution_cfg.threads.max_pdsch_concurrency;
  unsigned max_pusch_and_srs_concurrency = unit_cfg.expert_execution_cfg.threads.max_pusch_and_srs_concurrency;

  // Override PDSCH and PUSCH maximum concurrency if hardware acceleration is present.
  if (unit_cfg.hal_config.has_value()) {
    const du_low_unit_hal_config& hal_config = *unit_cfg.hal_config;
    if (hal_config.bbdev_hwacc.has_value()) {
      const bbdev_appconfig& bbdev_hwacc = *hal_config.bbdev_hwacc;
      if (bbdev_hwacc.pdsch_enc.has_value()) {
        const hwacc_pdsch_appconfig& pdsch_enc = *bbdev_hwacc.pdsch_enc;
        if (max_pdsch_concurrency != pdsch_enc.nof_hwacc) {
          fmt::print("Warning: the configured maximum PDSCH concurrency ({}) is overridden by the number of PDSCH "
                     "encoder hardware accelerated functions ({})\n",
                     max_pdsch_concurrency,
                     pdsch_enc.nof_hwacc);
          max_pdsch_concurrency = pdsch_enc.nof_hwacc;
        }
      }
      if (bbdev_hwacc.pusch_dec.has_value()) {
        const hwacc_pusch_appconfig& pusch_dec = *bbdev_hwacc.pusch_dec;
        if (max_pusch_and_srs_concurrency != pusch_dec.nof_hwacc) {
          fmt::print("Warning: the configured maximum PUSCH and SRS concurrency ({}) is overridden by the number of "
                     "PUSCH decoder hardware accelerated functions ({})\n",
                     max_pusch_and_srs_concurrency,
                     pusch_dec.nof_hwacc);
          max_pusch_and_srs_concurrency = pusch_dec.nof_hwacc;
        }
      }
    }
  }

  du_low_cfg.max_pdsch_concurrency         = max_pdsch_concurrency;
  du_low_cfg.max_pucch_concurrency         = unit_cfg.expert_execution_cfg.threads.max_pucch_concurrency;
  du_low_cfg.max_pusch_and_srs_concurrency = max_pusch_and_srs_concurrency;

  if (unit_cfg.metrics_cfg.enable_du_low) {
    du_low_cfg.metrics_period.emplace(unit_cfg.metrics_cfg.du_report_period);
  }

  config.config_affinities.resize(nof_dl_antennas.size());
  for (unsigned i = 0, e = du_low_cfg.cell_nof_dl_antennas.size(); i != e; ++i) {
    config.config_affinities[i].push_back(unit_cfg.expert_execution_cfg.cell_affinities[i].l1_dl_cpu_cfg);
  }
}
