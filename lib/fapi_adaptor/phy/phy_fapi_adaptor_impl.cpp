/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "phy_fapi_adaptor_impl.h"

using namespace srsran;
using namespace fapi_adaptor;

/// Generates and returns a FAPI-to-PHY translator configuration from the given PHY adaptor configuration.
static fapi_to_phy_translator_config generate_fapi_to_phy_translator_config(const phy_fapi_adaptor_impl_config& config)
{
  fapi_to_phy_translator_config fapi_config;

  fapi_config.sector_id            = config.sector_id;
  fapi_config.scs                  = config.scs;
  fapi_config.ul_pdu_repository    = config.ul_pdu_repository;
  fapi_config.ul_rg_pool           = config.ul_rg_pool;
  fapi_config.dl_pdu_validator     = config.dl_pdu_validator;
  fapi_config.ul_pdu_validator     = config.ul_pdu_validator;
  fapi_config.ul_request_processor = config.ul_request_processor;
  fapi_config.dl_processor_pool    = config.dl_processor_pool;
  fapi_config.dl_rg_pool           = config.dl_rg_pool;
  fapi_config.carrier_cfg          = config.carrier_cfg;
  fapi_config.prach_cfg            = config.prach_cfg;
  fapi_config.scs_common           = config.scs_common;

  return fapi_config;
}

phy_fapi_adaptor_impl::phy_fapi_adaptor_impl(const phy_fapi_adaptor_impl_config& config) :
  fapi_translator(generate_fapi_to_phy_translator_config(config), srslog::fetch_basic_logger("FAPI")),
  time_translator(fapi_translator),
  results_translator(srslog::fetch_basic_logger("FAPI"))
{
}

upper_phy_timing_notifier& phy_fapi_adaptor_impl::get_timing_notifier()
{
  return time_translator;
}

void phy_fapi_adaptor_impl::set_slot_time_message_notifier(fapi::slot_time_message_notifier& fapi_time_slot_notifier)
{
  time_translator.set_slot_time_message_notifier(fapi_time_slot_notifier);
}

void phy_fapi_adaptor_impl::set_slot_data_message_notifier(fapi::slot_data_message_notifier& fapi_data_notifier)
{
  results_translator.set_slot_data_message_notifier(fapi_data_notifier);
}

fapi::slot_message_gateway& phy_fapi_adaptor_impl::get_slot_message_gateway()
{
  return fapi_translator;
}

upper_phy_rx_results_notifier& phy_fapi_adaptor_impl::get_rx_results_notifier()
{
  return results_translator;
}
