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

ru_emu_socket_receiver::ru_emu_socket_receiver(srslog::basic_logger&     logger_,
                                               srsran::task_executor&    executor_,
                                               const transmitter_config& config)
{
  receiver = create_receiver({config.interface, config.is_promiscuous_mode_enabled, false}, executor_, logger_);
  srsran_assert(receiver, "RU emulator failed to initialize Ethernet receiver");
}

ru_emu_socket_transmitter::ru_emu_socket_transmitter(srslog::basic_logger& logger_, const transmitter_config& config)
{
  transmitter = create_transmitter(config, logger_);
  srsran_assert(transmitter, "RU emulator failed to initialize Ethernet transmitter");
}

void ru_emu_socket_receiver::start(ether::frame_notifier& notifier_)
{
  receiver->get_operation_controller().start(notifier_);
}

void ru_emu_socket_receiver::stop()
{
  receiver->get_operation_controller().stop();
}

void ru_emu_socket_transmitter::send(span<span<const uint8_t>> frames)
{
  transmitter->send(frames);
}

std::unique_ptr<ru_emulator_transceiver> srsran::ru_emu_create_socket_transceiver(srslog::basic_logger&     logger,
                                                                                  task_executor&            executor,
                                                                                  const transmitter_config& config)
{
  return std::make_unique<ru_emulator_transceiver>(std::make_unique<ru_emu_socket_receiver>(logger, executor, config),
                                                   std::make_unique<ru_emu_socket_transmitter>(logger, config));
}
