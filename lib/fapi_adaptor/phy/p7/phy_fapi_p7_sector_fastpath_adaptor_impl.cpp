/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "phy_fapi_p7_sector_fastpath_adaptor_impl.h"
#include "srsran/fapi/slot_last_message_notifier.h"

using namespace srsran;
using namespace fapi_adaptor;

/// Generates and returns a FAPI-to-PHY translator configuration from the given PHY adaptor configuration.
static fapi_to_phy_fastpath_translator_config
generate_fapi_to_phy_translator_config(const phy_fapi_p7_sector_fastpath_adaptor_config& config)
{
  return {.sector_id                     = config.sector_id,
          .nof_slots_request_headroom    = config.nof_slots_request_headroom,
          .allow_request_on_empty_ul_tti = config.allow_request_on_empty_ul_tti,
          .scs                           = config.scs,
          .scs_common                    = config.scs_common,
          .prach_cfg                     = config.prach_cfg,
          .carrier_cfg                   = config.carrier_cfg,
          .prach_ports                   = config.prach_ports};
}

/// Generates and returns a FAPI-to-PHY translator dependencies from the given PHY adaptor dependencies.
static fapi_to_phy_fastpath_translator_dependencies
generate_fapi_to_phy_translator_dependencies(phy_fapi_p7_sector_fastpath_adaptor_dependencies dependencies)
{
  return {.logger               = dependencies.logger,
          .dl_processor_pool    = dependencies.dl_processor_pool,
          .dl_rg_pool           = dependencies.dl_rg_pool,
          .dl_pdu_validator     = dependencies.dl_pdu_validator,
          .ul_request_processor = dependencies.ul_request_processor,
          .ul_pdu_repository    = dependencies.ul_pdu_repository,
          .ul_pdu_validator     = dependencies.ul_pdu_validator,
          .pm_repo              = std::move(dependencies.pm_repo),
          .part2_repo           = std::move(dependencies.part2_repo)};
}

phy_fapi_p7_sector_fastpath_adaptor_impl::phy_fapi_p7_sector_fastpath_adaptor_impl(
    const phy_fapi_p7_sector_fastpath_adaptor_config& config,
    phy_fapi_p7_sector_fastpath_adaptor_dependencies  dependencies) :
  results_translator(config.sector_id, config.dBFS_calibration_value, dependencies.logger),
  fapi_translator(generate_fapi_to_phy_translator_config(config),
                  generate_fapi_to_phy_translator_dependencies(std::move(dependencies))),
  time_translator(fapi_translator)
{
}

upper_phy_timing_notifier& phy_fapi_p7_sector_fastpath_adaptor_impl::get_timing_notifier()
{
  return time_translator;
}

void phy_fapi_p7_sector_fastpath_adaptor_impl::set_slot_time_message_notifier(
    fapi::slot_time_message_notifier& fapi_time_slot_notifier)
{
  time_translator.set_slot_time_message_notifier(fapi_time_slot_notifier);
}

void phy_fapi_p7_sector_fastpath_adaptor_impl::set_error_message_notifier(
    fapi::error_message_notifier& fapi_error_notifier)
{
  fapi_translator.set_error_message_notifier(fapi_error_notifier);
  error_translator.set_error_message_notifier(fapi_error_notifier);
}

void phy_fapi_p7_sector_fastpath_adaptor_impl::set_slot_data_message_notifier(
    fapi::slot_data_message_notifier& fapi_data_notifier)
{
  results_translator.set_slot_data_message_notifier(fapi_data_notifier);
}

fapi::slot_last_message_notifier& phy_fapi_p7_sector_fastpath_adaptor_impl::get_slot_last_message_notifier()
{
  return fapi_translator;
}

fapi::slot_message_gateway& phy_fapi_p7_sector_fastpath_adaptor_impl::get_slot_message_gateway()
{
  return fapi_translator;
}

upper_phy_rx_results_notifier& phy_fapi_p7_sector_fastpath_adaptor_impl::get_rx_results_notifier()
{
  return results_translator;
}

upper_phy_error_notifier& phy_fapi_p7_sector_fastpath_adaptor_impl::get_error_notifier()
{
  return error_translator;
}
