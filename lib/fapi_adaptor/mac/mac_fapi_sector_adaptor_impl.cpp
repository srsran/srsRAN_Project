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

#include "mac_fapi_sector_adaptor_impl.h"
#include "srsran/srslog/srslog.h"

using namespace srsran;
using namespace fapi_adaptor;

static mac_to_fapi_translator_config generate_translator_config(const mac_fapi_sector_adaptor_config& config)
{
  mac_to_fapi_translator_config out_config;
  out_config.cell_nof_prbs = config.cell_nof_prbs;
  out_config.sector_id     = config.sector_id;

  return out_config;
}

static mac_to_fapi_translator_dependencies
generate_translator_dependencies(mac_fapi_sector_adaptor_dependencies&& dependencies)
{
  return {srslog::fetch_basic_logger("FAPI"),
          *dependencies.gateway,
          *dependencies.last_msg_notifier,
          std::move(dependencies.pm_mapper),
          std::move(dependencies.part2_mapper)};
}

mac_fapi_sector_adaptor_impl::mac_fapi_sector_adaptor_impl(const mac_fapi_sector_adaptor_config&  config,
                                                           mac_fapi_sector_adaptor_dependencies&& dependencies) :
  mac_translator(generate_translator_config(config), generate_translator_dependencies(std::move(dependencies))),
  fapi_data_translator(config.scs, config.sector_id),
  fapi_time_translator(config.scs),
  fapi_error_translator(config.scs)
{
}

fapi::slot_time_message_notifier& mac_fapi_sector_adaptor_impl::get_slot_time_message_notifier()
{
  return fapi_time_translator;
}

fapi::error_message_notifier& mac_fapi_sector_adaptor_impl::get_error_message_notifier()
{
  return fapi_error_translator;
}

fapi::slot_data_message_notifier& mac_fapi_sector_adaptor_impl::get_slot_data_message_notifier()
{
  return fapi_data_translator;
}

mac_cell_result_notifier& mac_fapi_sector_adaptor_impl::get_cell_result_notifier()
{
  return mac_translator;
}

void mac_fapi_sector_adaptor_impl::set_cell_slot_handler(mac_cell_slot_handler& mac_slot_handler)
{
  fapi_time_translator.set_cell_slot_handler(mac_slot_handler);
  fapi_error_translator.set_cell_slot_handler(mac_slot_handler);
  mac_translator.set_cell_slot_handler(mac_slot_handler);
}

void mac_fapi_sector_adaptor_impl::set_cell_rach_handler(mac_cell_rach_handler& mac_rach_handler)
{
  fapi_data_translator.set_cell_rach_handler(mac_rach_handler);
}

void mac_fapi_sector_adaptor_impl::set_cell_pdu_handler(mac_pdu_handler& handler)
{
  fapi_data_translator.set_cell_pdu_handler(handler);
}

void mac_fapi_sector_adaptor_impl::set_cell_crc_handler(mac_cell_control_information_handler& handler)
{
  fapi_data_translator.set_cell_crc_handler(handler);
}
