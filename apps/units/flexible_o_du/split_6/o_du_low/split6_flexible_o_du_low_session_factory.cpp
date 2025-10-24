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

#include "split6_flexible_o_du_low_session_factory.h"
#include "apps/services/metrics/metrics_config.h"
#include "apps/services/worker_manager/worker_manager.h"
#include "apps/units/flexible_o_du/o_du_low/du_low_config_validator.h"
#include "apps/units/flexible_o_du/o_du_low/o_du_low_unit_factory.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_config_validator.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_factories.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_config_validator.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_factories.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_helpers.h"
#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_configs.h"
#include "external/fmt/include/fmt/chrono.h"
#include "split6_constants.h"
#include "split6_flexible_o_du_low_session.h"
#include "srsran/du/du_low/o_du_low_config.h"
#include "srsran/fapi/cell_config.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_sector_adaptor.h"
#include "srsran/ran/prach/prach_configuration.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"

using namespace srsran;

std::optional<std::chrono::system_clock::time_point>
split6_flexible_o_du_low_session_factory::start_time_calculator::calculate_start_time() const
{
  if (jitter.count() == 0) {
    return std::nullopt;
  }

  auto                      current_time = std::chrono::system_clock::now();
  std::chrono::milliseconds ms_since_epoch =
      std::chrono::duration_cast<std::chrono::milliseconds>(current_time.time_since_epoch());

  std::chrono::milliseconds offset_in_window = ms_since_epoch % window_size;

  if (offset_in_window < jitter || offset_in_window > jitter_upper_border) {
    logger.warning("Current start time is too close to the start window border, synchronization may fail");
  }

  auto start_time = std::chrono::floor<std::chrono::seconds>(current_time + (2 * window_size - offset_in_window));
  logger.debug("Start.request received at '{:%Y-%m-%d %H:%M:%S}' jitter_ms={} window_size={}. Calculated O-DU low "
               "start at '{:%Y-%m-%d %H:%M:%S}'",
               current_time,
               jitter,
               window_size,
               start_time);

  // Return next window start.
  return {start_time};
}

std::unique_ptr<split6_flexible_o_du_low_session>
split6_flexible_o_du_low_session_factory::create_o_du_low_session(const fapi::fapi_cell_config& config)
{
  auto odu = std::make_unique<split6_flexible_o_du_low_session>(
      notifier, std::chrono::milliseconds(unit_config.du_report_period));

  // Create Radio Unit.
  auto ru = create_radio_unit(*odu, config);
  if (!ru) {
    return nullptr;
  }

  // Create O-DU low.
  auto odu_low = create_o_du_low(config,
                                 {odu->get_upper_ru_dl_rg_adapter(),
                                  odu->get_upper_ru_ul_request_adapter(),
                                  workers.get_du_low_executor_mapper()});

  if (!odu_low.o_du_lo) {
    return nullptr;
  }

  // Get the FAPI sector adaptor to get the dependencies.
  auto& fapi_sector_adaptor = odu_low.o_du_lo->get_phy_fapi_adaptor().get_sector_adaptor(split6_du_low::CELL_ID);

  // Create FAPI slot messages adaptor.
  auto adaptor =
      slot_messages_adaptor_factory->create_slot_messages_adaptor(config,
                                                                  fapi_sector_adaptor.get_slot_message_gateway(),
                                                                  fapi_sector_adaptor.get_slot_last_message_notifier(),
                                                                  ru->get_controller());

  if (!adaptor) {
    return nullptr;
  }

  odu->set_dependencies(std::move(adaptor),
                        std::move(odu_low.o_du_lo),
                        std::move(ru),
                        timers.create_unique_timer(workers.get_metrics_executor()));

  return odu;
}

/// Return the TDD period in slots for the given period and slots per subframe.
static unsigned get_period_in_slots(unsigned period, unsigned nof_slots_per_subframe)
{
  switch (period) {
    // 0.5 ms.
    case 0:
      srsran_assert(nof_slots_per_subframe % 2 == 0, "Invalid number of slots per subframe to get periodicity 0.5ms");
      return nof_slots_per_subframe / 2;
      // 0.625 ms.
    case 1:
      srsran_assert((nof_slots_per_subframe * 5) % 8 == 0,
                    "Invalid number of slots per subframe to get periodicity 0.625ms");
      return nof_slots_per_subframe * 5 / 8;
      // 1 ms.
    case 2:
      return nof_slots_per_subframe;
      // 1.25 ms.
    case 3:
      srsran_assert((nof_slots_per_subframe * 5) % 4 == 0,
                    "Invalid number of slots per subframe to get 1.25ms periodicity");
      return nof_slots_per_subframe * 5 / 4;
      // 2 ms.
    case 4:
      return 2 * nof_slots_per_subframe;
      // 2.5 ms.
    case 5:
      srsran_assert((nof_slots_per_subframe * 5) % 2 == 0,
                    "Invalid number of slots per subframe to get periodicity 2.5ms");
      return nof_slots_per_subframe * 5 / 2;
      // 5 ms.
    case 6:
      return nof_slots_per_subframe * 5;
      // 10 ms.
    case 7:
      return nof_slots_per_subframe * 10;
    default:
      srsran_assert(0, "TDD period '{}' not supported", period);
  }

  return nof_slots_per_subframe;
}

/// Generates the TDD pattern from the given FAPI structure.
static tdd_ul_dl_config_common generate_tdd_pattern(subcarrier_spacing scs, const fapi::tdd_phy_config& pattern)
{
  // Note: initializing the struct is important to set all values to 0.
  tdd_ul_dl_config_common out = {};
  out.ref_scs                 = scs;

  // [Implementation defined] For now only fill one pattern.
  auto& pattern1 = out.pattern1;

  unsigned nof_slots_per_subframe    = slot_point(scs, 0).nof_slots_per_subframe();
  pattern1.dl_ul_tx_period_nof_slots = get_period_in_slots(pattern.tdd_period, nof_slots_per_subframe);

  for (unsigned i = 0; i != pattern1.dl_ul_tx_period_nof_slots; ++i) {
    const auto& slot = pattern.slot_config[i];
    if (std::all_of(slot.cbegin(), slot.cend(), [](const fapi::tdd_slot_symbol_type& value) {
          return value == fapi::tdd_slot_symbol_type::dl_symbol;
        })) {
      ++pattern1.nof_dl_slots;
    } else if (std::all_of(slot.cbegin(), slot.cend(), [](const fapi::tdd_slot_symbol_type& value) {
                 return value == fapi::tdd_slot_symbol_type::ul_symbol;
               })) {
      ++pattern1.nof_ul_slots;
    } else {
      if (auto I = std::find_if(
              slot.crbegin(),
              slot.crend(),
              [](const fapi::tdd_slot_symbol_type& value) { return value == fapi::tdd_slot_symbol_type::dl_symbol; });
          I != slot.crend()) {
        pattern1.nof_dl_symbols = std::distance(I, slot.crend());
      }

      if (const auto* I = std::find_if(
              slot.cbegin(),
              slot.cend(),
              [](const fapi::tdd_slot_symbol_type& value) { return value == fapi::tdd_slot_symbol_type::ul_symbol; });
          I != slot.cend()) {
        pattern1.nof_ul_symbols = std::distance(I, slot.cend());
      }
    }
  }

  return out;
}

static std::vector<du_low_prach_validation_config>
get_du_low_validation_dependencies(const fapi::fapi_cell_config& config)
{
  std::vector<du_low_prach_validation_config> out_cfg(split6_du_low::NOF_CELLS_SUPPORTED);

  du_low_prach_validation_config& out_cell = out_cfg.front();

  // Get PRACH info.
  subcarrier_spacing common_scs = config.phy_cfg.scs;

  prach_configuration prach_info = prach_configuration_get(split6_du_low::freq_range,
                                                           static_cast<duplex_mode>(config.cell_cfg.frame_duplex_type),
                                                           config.prach_cfg.prach_config_index);
  // PRACH format type.
  out_cell.format = prach_info.format;

  // Get preamble info.
  prach_preamble_information preamble_info =
      is_long_preamble(prach_info.format)
          ? get_prach_preamble_long_info(prach_info.format)
          : get_prach_preamble_short_info(prach_info.format, to_ra_subcarrier_spacing(common_scs), false);

  out_cell.prach_scs             = preamble_info.scs;
  out_cell.zero_correlation_zone = config.prach_cfg.fd_occasions.front().prach_zero_corr_conf;
  out_cell.nof_prach_ports       = split6_du_low::PRACH_NOF_PORTS;
  out_cell.nof_antennas_ul       = config.carrier_cfg.num_rx_ant;

  return out_cfg;
}

o_du_low_unit
split6_flexible_o_du_low_session_factory::create_o_du_low(const fapi::fapi_cell_config& config,
                                                          o_du_low_unit_dependencies&&  odu_low_dependencies)
{
  srs_du::cell_prach_ports_entry prach_ports = {split6_du_low::PRACH_PORT};

  auto du_low_dependencies = get_du_low_validation_dependencies(config);
  if (!validate_du_low_config(unit_config.du_low_cfg, du_low_dependencies)) {
    return {};
  }

  o_du_low_unit_config odu_low_cfg = {unit_config.du_low_cfg, {}, {}};

  auto& fapi_sector = odu_low_cfg.fapi_cfg.sectors.emplace_back();

  fapi_sector.carrier_cfg                   = config.carrier_cfg;
  fapi_sector.prach_cfg                     = config.prach_cfg;
  fapi_sector.allow_request_on_empty_ul_tti = unit_config.du_low_cfg.expert_phy_cfg.allow_request_on_empty_uplink_slot;
  fapi_sector.nof_slots_request_headroom    = unit_config.du_low_cfg.expert_phy_cfg.nof_slots_request_headroom;
  fapi_sector.prach_ports                   = prach_ports;
  fapi_sector.scs                           = config.phy_cfg.scs;
  fapi_sector.scs_common                    = config.phy_cfg.scs;
  fapi_sector.dBFS_calibration_value        = 1.F;
  // When the sampling rate is provided, calculate the dBFS calibration value as sqrt(sampling rate / subcarrier
  // spacing). This factor is the magnitude of a single subcarrier in normalized PHY linear units equivalent to
  // a constant signal with a power of 0 dBFS.
  if (sampling_rate_MHz) {
    fapi_sector.dBFS_calibration_value = calculate_dBFS_calibration_value(*sampling_rate_MHz, config.phy_cfg.scs);
  }

  // :TODO: add a parse option for the sector, so it will be easier to debug problems when running more than one
  // instance.
  fapi_sector.sector_id = 0;

  auto& du_low_cell = odu_low_cfg.cells.emplace_back();

  du_low_cell.duplex               = static_cast<duplex_mode>(config.cell_cfg.frame_duplex_type);
  du_low_cell.freq_range           = split6_du_low::freq_range;
  du_low_cell.bw_rb                = config.carrier_cfg.dl_grid_size[to_numerology_value(config.phy_cfg.scs)];
  du_low_cell.nof_rx_antennas      = config.carrier_cfg.num_rx_ant;
  du_low_cell.nof_tx_antennas      = config.carrier_cfg.num_tx_ant;
  du_low_cell.prach_ports          = prach_ports;
  du_low_cell.scs_common           = config.phy_cfg.scs;
  du_low_cell.prach_config_index   = config.prach_cfg.prach_config_index;
  du_low_cell.max_puschs_per_slot  = MAX_PUSCH_PDUS_PER_SLOT;
  du_low_cell.pusch_max_nof_layers = split6_du_low::PUSCH_MAX_NOF_LAYERS;

  // TDD pattern.
  if (config.cell_cfg.frame_duplex_type == 1) {
    // [Implementation defined] Start with one pattern for now.
    du_low_cell.tdd_pattern.emplace(generate_tdd_pattern(config.phy_cfg.scs, config.tdd_cfg));
  }

  o_du_low_unit_factory odu_low_factory(unit_config.du_low_cfg.hal_config);

  return odu_low_factory.create(odu_low_cfg, odu_low_dependencies);
}

static flexible_o_du_ru_config
generate_o_du_ru_config(const fapi::fapi_cell_config& config, unsigned expected_max_proc_delay, bool uses_ofh)
{
  flexible_o_du_ru_config out_cfg;
  out_cfg.prach_nof_ports = split6_du_low::PRACH_NOF_PORTS;

  // Open Fronthaul notifies OTA + max_proc_delay.
  if (uses_ofh) {
    out_cfg.max_processing_delay = expected_max_proc_delay;
  } else {
    // Split 8 notifies OTA + max_proc_delay + 1ms.
    out_cfg.max_processing_delay = expected_max_proc_delay - get_nof_slots_per_subframe(config.phy_cfg.scs);
  }

  // Add one cell.
  auto& out_cell           = out_cfg.cells.emplace_back();
  out_cell.nof_rx_antennas = config.carrier_cfg.num_rx_ant;
  out_cell.nof_tx_antennas = config.carrier_cfg.num_tx_ant;
  out_cell.scs             = config.phy_cfg.scs;
  out_cell.dl_arfcn        = config.carrier_cfg.dl_f_ref_arfcn;
  out_cell.ul_arfcn        = config.carrier_cfg.ul_f_ref_arfcn;
  out_cell.bw              = MHz_to_bs_channel_bandwidth(config.carrier_cfg.dl_bandwidth);
  out_cell.cp              = config.phy_cfg.cp;
  out_cell.freq_range      = split6_du_low::freq_range;

  // TDD pattern.
  if (config.cell_cfg.frame_duplex_type == 1) {
    // [Implementation defined] Start with one pattern for now.
    out_cell.tdd_config.emplace(generate_tdd_pattern(config.phy_cfg.scs, config.tdd_cfg));
  }

  return out_cfg;
}

static std::vector<ru_ofh_cell_validation_config>
get_ru_ofh_validation_dependencies(const fapi::fapi_cell_config& config)
{
  std::vector<ru_ofh_cell_validation_config> out_cfg(split6_du_low::NOF_CELLS_SUPPORTED);

  ru_ofh_cell_validation_config& out_cell = out_cfg.front();

  // Validates the sampling rate is compatible with the PRACH sequence.
  out_cell.scs             = config.phy_cfg.scs;
  out_cell.nof_prach_ports = split6_du_low::PRACH_NOF_PORTS;
  out_cell.nof_antennas_dl = config.carrier_cfg.num_tx_ant;
  out_cell.nof_antennas_ul = config.carrier_cfg.num_rx_ant;

  return out_cfg;
}

static std::vector<ru_sdr_cell_validation_config>
get_ru_sdr_validation_dependencies(const fapi::fapi_cell_config& config)
{
  std::vector<ru_sdr_cell_validation_config> out_cfg(split6_du_low::NOF_CELLS_SUPPORTED);

  ru_sdr_cell_validation_config& out_cell = out_cfg.front();

  // Validates the sampling rate is compatible with the PRACH sequence.
  out_cell.common_scs     = config.phy_cfg.scs;
  out_cell.channel_bw_mhz = MHz_to_bs_channel_bandwidth(config.carrier_cfg.dl_bandwidth);
  out_cell.dplx_mode      = static_cast<duplex_mode>(config.cell_cfg.frame_duplex_type);

  prach_configuration prach_info =
      prach_configuration_get(frequency_range::FR1, out_cell.dplx_mode, config.prach_cfg.prach_config_index);

  out_cell.prach_format = prach_info.format;

  out_cell.preamble_info =
      is_long_preamble(prach_info.format)
          ? get_prach_preamble_long_info(prach_info.format)
          : get_prach_preamble_short_info(prach_info.format, to_ra_subcarrier_spacing(config.phy_cfg.scs), false);

  return out_cfg;
}

/// Derives the sampling rate from the frequency.
static double derive_srate_MHz_from_bandwith(unsigned bandwidth_MHz)
{
  if (bandwidth_MHz <= 15) {
    return 23.04;
  } else if (bandwidth_MHz <= 30) {
    return 46.08;
  } else if (bandwidth_MHz <= 60) {
    return 92.16;
  }

  return 184.32;
}

std::unique_ptr<radio_unit>
split6_flexible_o_du_low_session_factory::create_radio_unit(split6_flexible_o_du_low_session& odu_low,
                                                            const fapi::fapi_cell_config&     config)
{
  auto ru_config = generate_o_du_ru_config(config,
                                           unit_config.du_low_cfg.expert_phy_cfg.max_processing_delay_slots,
                                           std::holds_alternative<ru_ofh_unit_parsed_config>(unit_config.ru_cfg));

  const auto& ru_cfg = unit_config.ru_cfg;

  flexible_o_du_ru_dependencies ru_dependencies{workers,
                                                odu_low.get_upper_ru_ul_adapter(),
                                                odu_low.get_upper_ru_timing_adapter(),
                                                odu_low.get_upper_ru_error_adapter()};

  if (const auto* cfg = std::get_if<ru_ofh_unit_parsed_config>(&ru_cfg)) {
    auto ru_ofh_dependencies = get_ru_ofh_validation_dependencies(config);
    if (!validate_ru_ofh_config(cfg->config, ru_ofh_dependencies)) {
      return nullptr;
    }

    return create_ofh_radio_unit(cfg->config, ru_config, ru_dependencies);
  }

  if (const auto* cfg = std::get_if<ru_sdr_unit_config>(&ru_cfg)) {
    ru_sdr_unit_config updated_srate_config = *cfg;
    updated_srate_config.srate_MHz          = derive_srate_MHz_from_bandwith(config.carrier_cfg.dl_bandwidth);
    // Update the samping rate.
    sampling_rate_MHz.emplace(updated_srate_config.srate_MHz);

    auto ru_sdr_dependencies = get_ru_sdr_validation_dependencies(config);
    if (!validate_ru_sdr_config(updated_srate_config, ru_sdr_dependencies)) {
      return nullptr;
    }

    return create_sdr_radio_unit(
        updated_srate_config, ru_config, ru_dependencies, start_time_calc.calculate_start_time());
  }

  report_error("Could not detect valid Radio Unit configuration");
}
