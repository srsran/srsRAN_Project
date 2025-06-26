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

#include "split6_flexible_o_du_low_factory.h"
#include "apps/services/metrics/metrics_config.h"
#include "apps/units/flexible_o_du/o_du_low/o_du_low_unit_factory.h"
#include "apps/units/flexible_o_du/split_7_2/helpers/ru_ofh_factories.h"
#include "apps/units/flexible_o_du/split_8/helpers/ru_sdr_factories.h"
#include "apps/units/flexible_o_du/split_helpers/flexible_o_du_configs.h"
#include "split6_constants.h"
#include "split6_flexible_o_du_low_impl.h"
#include "split6_slot_configurator_plugin_dummy.h"
#include "srsran/du/du_low/o_du_low_config.h"
#include "srsran/fapi/cell_operation_request_notifier.h"
#include "srsran/ran/slot_pdu_capacity_constants.h"

using namespace srsran;

namespace {

/// Split 6 flexible O-DU low factory dummy implementation.
class split6_flexible_o_du_low_factory_dummy : public split6_flexible_o_du_low_factory
{
private:
  // See interface for documentation.
  std::unique_ptr<split6_slot_configurator_plugin> create_plugin() override
  {
    return std::make_unique<split6_slot_configurator_plugin_dummy>();
  }
};

} // namespace

std::unique_ptr<split6_flexible_o_du_low_impl>
split6_flexible_o_du_low_factory::create_split6_flexible_o_du_low_impl(const fapi::fapi_cell_config& config)
{
  auto odu = std::make_unique<split6_flexible_o_du_low_impl>();

  // Create Radio Unit.
  auto ru = create_ru(*odu, config);

  // Create O-DU low.
  auto odu_low =
      create_odu_low(config, {odu->get_upper_ru_dl_rg_adapter(), odu->get_upper_ru_ul_request_adapter(), workers});

  // Create split 6 slot messages plugin.
  auto plugin = create_plugin();

  odu->set_dependencies(std::move(plugin), std::move(odu_low), std::move(ru));

  return odu;
}

/// Return the TDD period in slots for the given period and slots per subframe.
static unsigned get_period_in_slots(unsigned period, unsigned nof_slots_per_subframe)
{
  switch (period) {
    // 0.5 ms.
    case 0:
      srsran_assert(nof_slots_per_subframe > 1 && nof_slots_per_subframe % 2 == 0,
                    "Invalid number of slots per subframe");
      return nof_slots_per_subframe / 2;
      // 1 ms.
    case 2:
      return nof_slots_per_subframe;
      // 5 ms.
    case 6:
      return nof_slots_per_subframe * 5;
      // 10 ms.
    case 7:
      return nof_slots_per_subframe * 10;
      // Ignore the rest of the cases for simplicity.
      // 0.625 ms.
    case 1:
      // 1.25 ms.
    case 3:
      // 2 ms.
    case 4:
      // 2.5 ms.
    case 5:
      srsran_assert(0, "TDD period '{}' not supported");
  }
}

/// Generates the TDD pattern from the given FAPI structure.
static tdd_ul_dl_config_common generate_tdd_pattern(subcarrier_spacing scs, const fapi::tdd_phy_config& pattern)
{
  tdd_ul_dl_config_common out;
  out.ref_scs = scs;

  // [Implementation defined] For now only fill one pattern.
  auto& pattern1 = out.pattern1;

  unsigned nof_slots_per_subframe    = slot_point(scs, 0).nof_slots_per_subframe();
  pattern1.dl_ul_tx_period_nof_slots = get_period_in_slots(pattern.tdd_period, nof_slots_per_subframe);

  for (const auto& slot : pattern.slot_config) {
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

      if (auto I = std::find_if(
              slot.cbegin(),
              slot.cend(),
              [](const fapi::tdd_slot_symbol_type& value) { return value == fapi::tdd_slot_symbol_type::ul_symbol; });
          I != slot.cend()) {
        pattern1.nof_ul_symbols = std::distance(slot.cbegin(), I);
      }
    }
  }

  return out;
}

std::unique_ptr<srs_du::o_du_low>
split6_flexible_o_du_low_factory::create_o_du_low(const fapi::fapi_cell_config& config,
                                                  o_du_low_unit_dependencies&&  odu_low_dependencies)
{
  srs_du::cell_prach_ports_entry prach_ports = {split6_du_low::PRACH_PORT};

  o_du_low_unit_config odu_low_cfg = {config.du_low_cfg, {}, {}};

  auto& fapi_sector = odu_low_cfg.fapi_cfg.sectors.emplace_back();

  fapi_sector.carrier_cfg                   = config.carrier_cfg;
  fapi_sector.prach_cfg                     = config.prach_cfg;
  fapi_sector.allow_request_on_empty_ul_tti = unit_config.du_low_cfg.expert_phy_cfg.allow_request_on_empty_uplink_slot;
  fapi_sector.nof_slots_request_headroom    = unit_config.du_low_cfg.expert_phy_cfg.nof_slots_request_headroom;
  fapi_sector.prach_ports                   = prach_ports;
  fapi_sector.scs                           = config.phy_cfg.scs;
  fapi_sector.scs_common                    = config.phy_cfg.scs;
  // :TODO: add a parse option for the sector, so it will be easier to debug problems when running more than one
  // instance.
  fapi_sector.sector_id = 0;

  auto&   du_low_cell = odu_low_cfg.cells.emplace_back();
  nr_band band        = band_helper::get_band_from_dl_arfcn(band_helper::freq_to_nr_arfcn(config.carrier_cfg.dl_freq));
  du_low_cell.duplex  = band_helper::get_duplex_mode(band);
  du_low_cell.freq_range           = band_helper::get_freq_range(band);
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

  o_du_low_unit_factory odu_low_factory(du_lo.hal_config, split6_du_low::NOF_CELLS_SUPPORTED);

  return odu_low_factory.create(odu_low_cfg, std::move(odu_low_dependencies));
}

static flexible_o_du_ru_config generate_o_du_ru_config(const fapi::fapi_cell_config& config, unsigned max_prox_delay)
{
  flexible_o_du_ru_config out_cfg;
  out_cfg.prach_nof_ports      = split6_du_low::PRACH_NOF_PORTS;
  out_cfg.max_processing_delay = max_prox_delay;

  // Add one cell.
  auto& out_cell           = out_cfg.cells.emplace_back();
  out_cell.nof_rx_antennas = config.carrier_cfg.num_rx_ant;
  out_cell.nof_tx_antennas = config.carrier_cfg.num_tx_ant;
  out_cell.scs             = config.phy_cfg.scs;
  out_cell.dl_arfcn        = band_helper::freq_to_nr_arfcn(config.carrier_cfg.dl_freq);
  out_cell.ul_arfcn        = band_helper::freq_to_nr_arfcn(config.carrier_cfg.ul_freq);
  out_cell.bw              = MHz_to_bs_channel_bandwidth(config.carrier_cfg.dl_bandwidth);
  out_cell.band = band_helper::get_band_from_dl_arfcn(band_helper::freq_to_nr_arfcn(config.carrier_cfg.dl_freq));
  out_cell.cp   = config.phy_cfg.cp;

  // TDD pattern.
  if (config.cell_cfg.frame_duplex_type == 1) {
    // [Implementation defined] Start with one pattern for now.
    out_cell.tdd_config.emplace(generate_tdd_pattern(config.phy_cfg.scs, config.tdd_cfg));
  }

  return out_cfg;
}

std::unique_ptr<radio_unit> split6_flexible_o_du_low_factory::create_radio_unit(split6_flexible_o_du_low_impl& odu_low,
                                                                                const fapi::fapi_cell_config&  config)
{
  auto ru_config = generate_o_du_ru_config(config, unit_config.du_low_cfg.expert_phy_cfg.max_processing_delay_slots);
  const auto& ru_cfg = unit_config.ru_cfg;

  flexible_o_du_ru_dependencies ru_dependencies{workers,
                                                odu_low.get_upper_ru_ul_adapter(),
                                                odu_low.get_upper_ru_timing_adapter(),
                                                odu_low.get_upper_ru_error_adapter()};

  if (const auto* cfg = std::get_if<ru_ofh_unit_parsed_config>(&ru_cfg)) {
    return create_ofh_radio_unit(cfg->config, ru_config, ru_dependencies);
  }

  if (const auto* cfg = std::get_if<ru_sdr_unit_config>(&ru_cfg)) {
    return create_sdr_radio_unit(*cfg, ru_config, ru_dependencies);
  }

  srsran::report_error("Could not detect valid Radio Unit configuration");
}

std::unique_ptr<split6_flexible_o_du_low_factory> srsran::create_split6_flexible_o_du_low_factory()
{
  return std::make_unique<split6_flexible_o_du_low_factory_dummy>();
}
