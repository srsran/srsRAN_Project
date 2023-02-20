/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/srslog/srslog.h"

namespace srsgnb {
namespace fapi {

struct crc_indication_message;
struct dl_tti_request_message;
struct rach_indication_message;
struct rx_data_indication_message;
struct tx_data_request_message;
struct slot_indication_message;
struct uci_indication_message;
struct ul_dci_request_message;
struct ul_tti_request_message;

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
} // namespace srsgnb
