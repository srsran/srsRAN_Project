/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "ru_emulator_transceiver.h"
#include "srsran/ofh/ethernet/ethernet_factories.h"
#include "srsran/support/error_handling.h"

using namespace srsran;
using namespace ether;

ru_emu_socket_receiver::ru_emu_socket_receiver(srslog::basic_logger&  logger_,
                                               srsran::task_executor& executor_,
                                               const gw_config&       config)
{
  receiver = create_receiver({config.interface, config.is_promiscuous_mode_enabled, false}, executor_, logger_);
  srsran_assert(receiver, "RU emulator failed to initialize Ethernet receiver");
}

ru_emu_socket_transmitter::ru_emu_socket_transmitter(srslog::basic_logger& logger_, const gw_config& config)
{
  transmitter = create_gateway(config, logger_);
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

std::unique_ptr<ru_emulator_transceiver>
srsran::ru_emu_create_socket_transceiver(srslog::basic_logger& logger, task_executor& executor, const gw_config& config)
{
  return std::make_unique<ru_emulator_transceiver>(std::make_unique<ru_emu_socket_receiver>(logger, executor, config),
                                                   std::make_unique<ru_emu_socket_transmitter>(logger, config));
}
