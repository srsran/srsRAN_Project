/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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
