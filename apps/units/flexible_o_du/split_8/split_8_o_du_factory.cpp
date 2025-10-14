/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "split_8_o_du_factory.h"
#include "helpers/ru_sdr_factories.h"

using namespace srsran;

/// Calculates the dBFS calibration value as sqrt(sampling rate / subcarrier spacing). This factor is the magnitude of a
/// single subcarrier in normalized PHY linear units equivalent to a constant signal with a power of 0 dBFS.
static float calculate_dBFS_calibration_value(float sampling_rate_MHz, subcarrier_spacing scs)
{
  return std::sqrt(sampling_rate_MHz * 1e3 / scs_to_khz(scs));
}

split8_du_factory::split8_du_factory(const split_8_o_du_unit_config& config_) :
  flexible_o_du_factory(
      {config_.odu_high_cfg,
       config_.du_low_cfg,
       {config_.ru_cfg.metrics_cfg.metrics_cfg,
        config_.ru_cfg.metrics_cfg.enable_ru_metrics,
        calculate_dBFS_calibration_value(config_.ru_cfg.srate_MHz,
                                         config_.odu_high_cfg.du_high_cfg.config.cells_cfg.front().cell.common_scs)}}),
  unit_config(config_)
{
}

std::unique_ptr<radio_unit> split8_du_factory::create_radio_unit(const flexible_o_du_ru_config&       ru_config,
                                                                 const flexible_o_du_ru_dependencies& ru_dependencies)
{
  return create_sdr_radio_unit(unit_config.ru_cfg, ru_config, ru_dependencies);
}
