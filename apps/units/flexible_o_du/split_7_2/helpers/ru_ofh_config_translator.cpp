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

#include "ru_ofh_config_translator.h"
#include "apps/services/worker_manager/worker_manager_config.h"
#include "ru_ofh_config.h"

using namespace srsran;

static bool parse_mac_address(const std::string& mac_str, span<uint8_t> mac)
{
  std::array<unsigned, 6> data       = {};
  int                     bytes_read = std::sscanf(
      mac_str.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x", &data[0], &data[1], &data[2], &data[3], &data[4], &data[5]);
  if (bytes_read != ether::ETH_ADDR_LEN) {
    fmt::print("Invalid MAC address provided: {}\n", mac_str);
    return false;
  }

  std::copy(data.begin(), data.end(), mac.begin());

  return true;
}

/// Converts reception window timing parameters from microseconds to number of symbols given the symbol duration.
static ofh::rx_window_timing_parameters
rx_timing_window_params_us_to_symbols(std::chrono::microseconds                Ta4_max,
                                      std::chrono::microseconds                Ta4_min,
                                      std::chrono::duration<double, std::nano> symbol_duration)
{
  ofh::rx_window_timing_parameters rx_window_timing_params;
  rx_window_timing_params.sym_start = std::floor(Ta4_min / symbol_duration);
  rx_window_timing_params.sym_end   = std::ceil(Ta4_max / symbol_duration);

  return rx_window_timing_params;
}

/// Converts transmission window timing parameters from microseconds to number of symbols given the symbol duration.
static ofh::tx_window_timing_parameters
tx_timing_window_params_us_to_symbols(std::chrono::microseconds                T1a_max_cp_dl,
                                      std::chrono::microseconds                T1a_min_cp_dl,
                                      std::chrono::microseconds                T1a_max_cp_ul,
                                      std::chrono::microseconds                T1a_min_cp_ul,
                                      std::chrono::microseconds                T1a_max_up,
                                      std::chrono::microseconds                T1a_min_up,
                                      std::chrono::duration<double, std::nano> symbol_duration)
{
  ofh::tx_window_timing_parameters tx_window_timing_params;
  tx_window_timing_params.sym_cp_dl_start = std::floor(T1a_max_cp_dl / symbol_duration);
  tx_window_timing_params.sym_cp_dl_end   = std::ceil(T1a_min_cp_dl / symbol_duration);
  tx_window_timing_params.sym_cp_ul_start = std::floor(T1a_max_cp_ul / symbol_duration);
  tx_window_timing_params.sym_cp_ul_end   = std::ceil(T1a_min_cp_ul / symbol_duration);
  tx_window_timing_params.sym_up_dl_start = std::floor(T1a_max_up / symbol_duration);
  tx_window_timing_params.sym_up_dl_end   = std::ceil(T1a_min_up / symbol_duration);

  return tx_window_timing_params;
}

static void generate_config(ru_ofh_configuration&                            out_cfg,
                            const ru_ofh_unit_config&                        ru_cfg,
                            span<const flexible_o_du_ru_config::cell_config> cells,
                            unsigned                                         max_processing_delay_slots)
{
  out_cfg.gps_Alpha = ru_cfg.gps_Alpha;
  out_cfg.gps_Beta  = ru_cfg.gps_Beta;

  // Add one cell.
  for (unsigned i = 0, e = ru_cfg.cells.size(); i != e; ++i) {
    const ru_ofh_unit_cell_config&              ofh_cell_cfg = ru_cfg.cells[i];
    const flexible_o_du_ru_config::cell_config& cell         = cells[i];
    ofh::sector_configuration&                  sector_cfg   = out_cfg.sector_configs.emplace_back();

    sector_cfg.sector_id                    = i;
    sector_cfg.max_processing_delay_slots   = max_processing_delay_slots;
    sector_cfg.dl_processing_time           = std::chrono::microseconds(ru_cfg.dl_processing_time);
    sector_cfg.ul_processing_time           = std::chrono::microseconds(ru_cfg.ul_processing_time);
    sector_cfg.uses_dpdk                    = ru_cfg.hal_config.has_value();
    sector_cfg.interface                    = ofh_cell_cfg.network_interface;
    sector_cfg.is_promiscuous_mode_enabled  = ofh_cell_cfg.enable_promiscuous_mode;
    sector_cfg.is_link_status_check_enabled = ofh_cell_cfg.check_link_status;
    sector_cfg.are_metrics_enabled          = ru_cfg.metrics_cfg.enable_ru_metrics;
    sector_cfg.mtu_size                     = ofh_cell_cfg.mtu_size;
    if (!parse_mac_address(ofh_cell_cfg.du_mac_address, sector_cfg.mac_src_address)) {
      srsran_terminate("Invalid Distributed Unit MAC address");
    }

    if (!parse_mac_address(ofh_cell_cfg.ru_mac_address, sector_cfg.mac_dst_address)) {
      srsran_terminate("Invalid Radio Unit MAC address");
    }

    std::chrono::duration<double, std::nano> symbol_duration(
        (1e6 / (get_nsymb_per_slot(cyclic_prefix::NORMAL) * get_nof_slots_per_subframe(cell.scs))));

    sector_cfg.cp              = cell.cp;
    sector_cfg.scs             = cell.scs;
    sector_cfg.bw              = cell.bw;
    sector_cfg.nof_antennas_ul = cell.nof_rx_antennas;
    sector_cfg.ru_operating_bw =
        ofh_cell_cfg.cell.ru_operating_bw ? ofh_cell_cfg.cell.ru_operating_bw.value() : sector_cfg.bw;
    sector_cfg.is_uplink_static_compr_hdr_enabled   = ofh_cell_cfg.cell.is_uplink_static_comp_hdr_enabled;
    sector_cfg.is_downlink_static_compr_hdr_enabled = ofh_cell_cfg.cell.is_downlink_static_comp_hdr_enabled;
    sector_cfg.tx_window_timing_params = tx_timing_window_params_us_to_symbols(ofh_cell_cfg.cell.T1a_max_cp_dl,
                                                                               ofh_cell_cfg.cell.T1a_min_cp_dl,
                                                                               ofh_cell_cfg.cell.T1a_max_cp_ul,
                                                                               ofh_cell_cfg.cell.T1a_min_cp_ul,
                                                                               ofh_cell_cfg.cell.T1a_max_up,
                                                                               ofh_cell_cfg.cell.T1a_min_up,
                                                                               symbol_duration);
    sector_cfg.rx_window_timing_params =
        rx_timing_window_params_us_to_symbols(ofh_cell_cfg.cell.Ta4_max, ofh_cell_cfg.cell.Ta4_min, symbol_duration);
    sector_cfg.is_prach_control_plane_enabled  = ofh_cell_cfg.cell.is_prach_control_plane_enabled;
    sector_cfg.ignore_prach_start_symbol       = ofh_cell_cfg.cell.ignore_prach_start_symbol;
    sector_cfg.ignore_ecpri_payload_size_field = ofh_cell_cfg.cell.ignore_ecpri_payload_size_field;
    sector_cfg.ignore_ecpri_seq_id_field       = ofh_cell_cfg.cell.ignore_ecpri_seq_id_field;
    sector_cfg.enable_log_warnings_for_lates   = ofh_cell_cfg.cell.enable_log_warnings_for_lates;
    sector_cfg.log_unreceived_ru_frames        = ofh_cell_cfg.cell.log_unreceived_ru_frames;
    sector_cfg.ul_compression_params           = {ofh::to_compression_type(ofh_cell_cfg.cell.compression_method_ul),
                                                  ofh_cell_cfg.cell.compression_bitwidth_ul};
    sector_cfg.dl_compression_params           = {ofh::to_compression_type(ofh_cell_cfg.cell.compression_method_dl),
                                                  ofh_cell_cfg.cell.compression_bitwidth_dl};
    sector_cfg.prach_compression_params        = {ofh::to_compression_type(ofh_cell_cfg.cell.compression_method_prach),
                                                  ofh_cell_cfg.cell.compression_bitwidth_prach};

    sector_cfg.tci_cp = ofh_cell_cfg.vlan_tag_cp;
    sector_cfg.tci_up = ofh_cell_cfg.vlan_tag_up;
    sector_cfg.prach_eaxc.assign(ofh_cell_cfg.ru_prach_port_id.begin(), ofh_cell_cfg.ru_prach_port_id.end());
    sector_cfg.dl_eaxc.assign(ofh_cell_cfg.ru_dl_port_id.begin(), ofh_cell_cfg.ru_dl_port_id.end());
    sector_cfg.ul_eaxc.assign(ofh_cell_cfg.ru_ul_port_id.begin(), ofh_cell_cfg.ru_ul_port_id.end());

    // TDD UL DL config.
    sector_cfg.tdd_config = cell.tdd_config;

    // IQ scaling config.
    if (const auto* legacy_scaling_config =
            std::get_if<ru_ofh_legacy_scaling_config>(&ofh_cell_cfg.cell.iq_scaling_config)) {
      // Set the IQ scaling with bandwidth normalization.
      unsigned nof_prbs =
          get_max_Nprb(bs_channel_bandwidth_to_MHz(sector_cfg.bw), sector_cfg.scs, frequency_range::FR1);
      sector_cfg.iq_scaling =
          legacy_scaling_config->iq_scaling / std::sqrt(static_cast<float>(nof_prbs * NOF_SUBCARRIERS_PER_RB));
    } else if (const auto* scaling_config = std::get_if<ru_ofh_scaling_config>(&ofh_cell_cfg.cell.iq_scaling_config)) {
      // Take the RU reference level, apply the configured subcarrier back-off and convert the result into a linear
      // scaling factor. If no subcarrier back-off is configured, apply a bandwidth power normalization factor.
      unsigned nof_prbs =
          get_max_Nprb(bs_channel_bandwidth_to_MHz(sector_cfg.bw), sector_cfg.scs, frequency_range::FR1);
      float subcarrier_rms_backoff_dB = convert_power_to_dB(nof_prbs * NOF_SUBCARRIERS_PER_RB);
      // If no value of subcarrier backoff is defined, the default bandwidth normalization is used.
      if (scaling_config->subcarrier_rms_backoff_dB) {
        subcarrier_rms_backoff_dB = *scaling_config->subcarrier_rms_backoff_dB;
      }
      sector_cfg.iq_scaling =
          convert_dB_to_amplitude(scaling_config->ru_reference_level_dBFS - subcarrier_rms_backoff_dB);
    } else {
      report_fatal_error(
          "Either the iq_scaling or the RU reference level and subarrier RMS backoff must be configured.");
    }

    // C-Plane PRACH FFT config.
    sector_cfg.c_plane_prach_fft_len = ofh_cell_cfg.cell.c_plane_prach_fft_len;
  }

  if (!is_valid_ru_ofh_config(out_cfg)) {
    report_error("Invalid Open Fronthaul Radio Unit configuration detected.\n");
  }
}

ru_ofh_configuration srsran::generate_ru_ofh_config(const ru_ofh_unit_config&                        ru_cfg,
                                                    span<const flexible_o_du_ru_config::cell_config> cells,
                                                    unsigned max_processing_delay_slots)
{
  ru_ofh_configuration out_cfg;
  generate_config(out_cfg, ru_cfg, cells, max_processing_delay_slots);

  return out_cfg;
}

void srsran::fill_ofh_worker_manager_config(worker_manager_config& config, const ru_ofh_unit_config& ru_cfg)
{
  auto& ofh_cfg           = config.ru_ofh_cfg.emplace();
  ofh_cfg.nof_cells       = ru_cfg.cells.size();
  ofh_cfg.ru_timing_cpu   = ru_cfg.expert_execution_cfg.ru_timing_cpu;
  ofh_cfg.txrx_affinities = ru_cfg.expert_execution_cfg.txrx_affinities;

  // If ru_txrx_cpus parameters are not specified, use the affinities of ru_cpus parameters of the cells.
  if (ofh_cfg.txrx_affinities.empty()) {
    for (unsigned i = 0, e = ru_cfg.cells.size(); i != e; ++i) {
      auto affinity_cfg = ru_cfg.expert_execution_cfg.cell_affinities[i].ru_cpu_cfg;
      ofh_cfg.txrx_affinities.emplace_back(affinity_cfg.mask);
    }
  }

  srsran_assert(config.config_affinities.size() == ru_cfg.expert_execution_cfg.cell_affinities.size(),
                "Invalid number of cell affinities");

  for (unsigned i = 0, e = ru_cfg.expert_execution_cfg.cell_affinities.size(); i != e; ++i) {
    config.config_affinities[i].push_back(ru_cfg.expert_execution_cfg.cell_affinities[i].ru_cpu_cfg);
  }
}
