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

#pragma once

#include "srsran/srslog/srslog.h"

namespace srsran {
namespace fapi {

struct crc_indication_message;
struct dl_tti_request_message;
struct error_indication_message;
struct rach_indication_message;
struct rx_data_indication_message;
struct tx_data_request_message;
struct slot_indication_message;
struct uci_indication_message;
struct ul_dci_request_message;
struct ul_tti_request_message;

/// Logs the given Error.indication message in the given logger.
void log_error_indication(const error_indication_message& msg, srslog::basic_logger& logger);

/// Logs the given CRC.indication message in the given logger.
void log_crc_indication(const crc_indication_message& msg, srslog::basic_logger& logger);

/// Logs the given DL_TTI.request message in the given logger.
void log_dl_tti_request(const dl_tti_request_message& msg, srslog::basic_logger& logger);

/// Logs the given RACH.indication message in the given logger.
void log_rach_indication(const rach_indication_message& msg, srslog::basic_logger& logger);

/// Logs the given Rx_Data.indication message in the given logger.
void log_rx_data_indication(const rx_data_indication_message& msg, srslog::basic_logger& logger);

/// Logs the given SLOT.indication message in the given logger.
void log_slot_indication(const slot_indication_message& msg, srslog::basic_logger& logger);

/// Logs the given Tx_Data.request message in the given logger.
void log_tx_data_request(const tx_data_request_message& msg, srslog::basic_logger& logger);

/// Logs the given UCI.indication message in the given logger.
void log_uci_indication(const uci_indication_message& msg, srslog::basic_logger& logger);

/// Logs the given UL_DCI.request message in the given logger.
void log_ul_dci_request(const ul_dci_request_message& msg, srslog::basic_logger& logger);

/// Logs the given UL_TTI.request message in the given logger.
void log_ul_tti_request(const ul_tti_request_message& msg, srslog::basic_logger& logger);

} // namespace fapi
} // namespace srsran
