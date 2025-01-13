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

#include "ru_emulator_transceiver.h"
#include "srsran/ofh/ethernet/ethernet_factories.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace ether;

socket_transceiver::socket_transceiver(srslog::basic_logger&   logger_,
                                       srsran::task_executor&  executor_,
                                       const ether::gw_config& config)
{
  transmitter = create_gateway(config, logger_);
  srsran_assert(transmitter, "RU emulator failed to initialize Ethernet transmitter");

  receiver = create_receiver(config.interface, config.is_promiscuous_mode_enabled, executor_, logger_);
  srsran_assert(receiver, "RU emulator failed to initialize Ethernet receiver");
}

void socket_transceiver::start(ether::frame_notifier& notifier_)
{
  receiver->start(notifier_);
}

void socket_transceiver::stop()
{
  receiver->stop();
}

void socket_transceiver::send(span<span<const uint8_t>> frames)
{
  transmitter->send(frames);
}
