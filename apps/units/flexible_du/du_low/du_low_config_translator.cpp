/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
#include "du_low_config.h"
#include "srsran/du/du_cell_config.h"
#include "srsran/phy/upper/upper_phy_factories.h"
#include "srsran/ran/duplex_mode.h"
#include "srsran/ran/prach/prach_configuration.h"

using namespace srsran;

static void generate_du_low_config(du_low_config&             out_config,
                                   const du_low_unit_config&  du_low,
                                   span<const du_cell_config> du_cells,
                                   span<const unsigned>       max_puschs_per_slot,
                                   unsigned                   du_id)
{
  out_config.cells.reserve(du_cells.size());

  for (unsigned i = 0, e = du_cells.size(); i != e; ++i) {
    const du_cell_config& cell           = du_cells[i];
    upper_phy_config&     upper_phy_cell = out_config.cells.emplace_back().upper_phy_cfg;

    // Get band, frequency range and duplex mode from the band.
    nr_band               band       = cell.dl_carrier.band;
    const frequency_range freq_range = band_helper::get_freq_range(band);
    const duplex_mode     duplex     = band_helper::get_duplex_mode(band);

    // Get bandwidth in PRB.
    const unsigned bw_rb = band_helper::get_n_rbs_from_bw(
        MHz_to_bs_channel_bandwidth(cell.dl_carrier.carrier_bw_mhz), cell.scs_common, freq_range);
    // Deduce the number of slots per subframe.
    const unsigned nof_slots_per_subframe = get_nof_slots_per_subframe(cell.scs_common);
    // Deduce the number of slots per frame.
    unsigned nof_slots_per_frame = nof_slots_per_subframe * NOF_SUBFRAMES_PER_FRAME;
    // Number of slots per system frame.
    unsigned nof_slots_per_system_frame = NOF_SFNS * nof_slots_per_frame;
    // PUSCH HARQ process lifetime in slots. It assumes the maximum lifetime is 100ms.
    unsigned expire_pusch_harq_timeout_slots = 100 * nof_slots_per_subframe;

    // Calculate the number of UL slots in a frame and in a PUSCH HARQ process lifetime.
    unsigned nof_ul_slots_in_harq_lifetime = expire_pusch_harq_timeout_slots;
    unsigned nof_ul_slots_per_frame        = nof_slots_per_frame;
    if (duplex == duplex_mode::TDD && cell.tdd_ul_dl_cfg_common.has_value()) {
      const tdd_ul_dl_pattern& pattern1     = cell.tdd_ul_dl_cfg_common->pattern1;
      unsigned                 period_slots = pattern1.dl_ul_tx_period_nof_slots;
      unsigned                 nof_ul_slots = pattern1.nof_ul_slots + ((pattern1.nof_ul_symbols != 0) ? 1 : 0);
      if (cell.tdd_ul_dl_cfg_common->pattern2.has_value()) {
        const tdd_ul_dl_pattern& pattern2 = cell.tdd_ul_dl_cfg_common->pattern2.value();
        period_slots += pattern2.dl_ul_tx_period_nof_slots;
        nof_ul_slots += pattern2.nof_ul_slots + ((pattern2.nof_ul_symbols != 0) ? 1 : 0);
      }
      nof_ul_slots_per_frame        = divide_ceil(nof_slots_per_frame, period_slots) * nof_ul_slots;
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
    const unsigned nof_buffers = max_puschs_per_slot[i] * nof_ul_slots_in_harq_lifetime;

    // Calculate the maximum number of receive codeblocks. It is equal to the product of:
    // - the maximum number of codeblocks that can be scheduled in one slot; and
    // - the number of PUSCH occasions in a HARQ process lifetime.
    const unsigned max_rx_nof_codeblocks = nof_ul_slots_in_harq_lifetime * max_nof_pusch_cb_slot;

    // Determine processing pipelines depth. Make sure the number of slots per system frame is divisible by the pipeline
    // depths.
    unsigned dl_pipeline_depth = 4 * du_low.expert_phy_cfg.max_processing_delay_slots;
    while (nof_slots_per_system_frame % dl_pipeline_depth != 0) {
      ++dl_pipeline_depth;
    }
    unsigned ul_pipeline_depth = std::max(dl_pipeline_depth, 8U);

    static constexpr unsigned prach_pipeline_depth = 1;

    const prach_configuration prach_cfg = prach_configuration_get(
        freq_range, duplex, cell.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index);
    srsran_assert(prach_cfg.format != prach_format_type::invalid,
                  "Unsupported PRACH configuration index (i.e., {}) for the given frequency range (i.e., {}) and "
                  "duplex mode (i.e., {}).",
                  cell.ul_cfg_common.init_ul_bwp.rach_cfg_common->rach_cfg_generic.prach_config_index,
                  to_string(freq_range),
                  to_string(duplex));

    // Maximum number of concurrent PUSCH transmissions. It is the maximum number of PUSCH transmissions that can be
    // processed simultaneously. If there are no dedicated threads for PUSCH decoding, it sets the queue size to one.
    // Otherwise, it is set to the maximum number of PUSCH transmissions that can be scheduled in one frame.
    unsigned max_pusch_concurrency = 1;
    if (du_low.expert_execution_cfg.threads.nof_pusch_decoder_threads > 0) {
      max_pusch_concurrency = max_puschs_per_slot[i] * nof_ul_slots_per_frame;
    }

    upper_phy_cell.nof_slots_request_headroom = du_low.expert_phy_cfg.nof_slots_request_headroom;
    upper_phy_cell.log_level                  = du_low.loggers.phy_level;
    upper_phy_cell.enable_logging_broadcast   = du_low.loggers.broadcast_enabled;
    upper_phy_cell.rx_symbol_printer_filename = du_low.loggers.phy_rx_symbols_filename;
    upper_phy_cell.rx_symbol_printer_port     = du_low.loggers.phy_rx_symbols_port;
    upper_phy_cell.rx_symbol_printer_prach    = du_low.loggers.phy_rx_symbols_prach;
    upper_phy_cell.logger_max_hex_size        = du_low.loggers.hex_max_size;
    upper_phy_cell.sector_id                  = du_id + i;
    upper_phy_cell.nof_tx_ports               = cell.dl_carrier.nof_ant;
    upper_phy_cell.nof_rx_ports               = cell.ul_carrier.nof_ant;
    upper_phy_cell.ldpc_decoder_iterations    = du_low.expert_phy_cfg.pusch_decoder_max_iterations;
    upper_phy_cell.ldpc_decoder_early_stop    = du_low.expert_phy_cfg.pusch_decoder_early_stop;
    upper_phy_cell.nof_dl_rg                  = dl_pipeline_depth + 2;
    upper_phy_cell.dl_rg_expire_timeout_slots = dl_pipeline_depth;
    upper_phy_cell.nof_dl_processors          = dl_pipeline_depth;
    upper_phy_cell.nof_ul_rg                  = ul_pipeline_depth;
    upper_phy_cell.max_ul_thread_concurrency  = du_low.expert_execution_cfg.threads.nof_ul_threads + 1;
    upper_phy_cell.max_pusch_concurrency      = max_pusch_concurrency;
    upper_phy_cell.nof_pusch_decoder_threads  = du_low.expert_execution_cfg.threads.nof_pusch_decoder_threads +
                                               du_low.expert_execution_cfg.threads.nof_ul_threads;
    upper_phy_cell.nof_prach_buffer           = prach_pipeline_depth * nof_slots_per_subframe;
    upper_phy_cell.max_nof_td_prach_occasions = prach_cfg.nof_occasions_within_slot;
    upper_phy_cell.max_nof_fd_prach_occasions = 1;
    upper_phy_cell.is_prach_long_format       = is_long_preamble(prach_cfg.format);
    upper_phy_cell.pusch_sinr_calc_method =
        channel_state_information::sinr_type_from_string(du_low.expert_phy_cfg.pusch_sinr_calc_method);

    upper_phy_cell.active_scs                                       = {};
    upper_phy_cell.active_scs[to_numerology_value(cell.scs_common)] = true;

    upper_phy_cell.dl_bw_rb = bw_rb;
    upper_phy_cell.ul_bw_rb = bw_rb;

    upper_phy_cell.rx_buffer_config.nof_buffers          = nof_buffers;
    upper_phy_cell.rx_buffer_config.nof_codeblocks       = max_rx_nof_codeblocks;
    upper_phy_cell.rx_buffer_config.max_codeblock_size   = ldpc::MAX_CODEBLOCK_SIZE;
    upper_phy_cell.rx_buffer_config.expire_timeout_slots = expire_pusch_harq_timeout_slots;
    upper_phy_cell.rx_buffer_config.external_soft_bits   = false;
    upper_phy_cell.crc_calculator_type                   = "auto";
    upper_phy_cell.ldpc_rate_dematcher_type              = "auto";
    upper_phy_cell.ldpc_decoder_type                     = "auto";

    if (!is_valid_upper_phy_config(upper_phy_cell)) {
      report_error("Invalid upper PHY configuration.\n");
    }
  }
}

void srsran::generate_du_low_wrapper_config(du_low_wrapper_config&              out_config,
                                            const du_low_unit_config&           du_low_unit_cfg,
                                            std::vector<cell_prach_ports_entry> prach_ports,
                                            span<const du_cell_config>          du_cells,
                                            span<const unsigned>                max_puschs_per_slot,
                                            unsigned                            du_id)
{
  generate_du_low_config(out_config.du_low_cfg, du_low_unit_cfg, du_cells, max_puschs_per_slot, du_id);
  out_config.prach_ports = std::move(prach_ports);
}