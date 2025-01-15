/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/gtpu/gtpu_echo_factory.h"
#include "gtpu_echo_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;

std::unique_ptr<gtpu_echo> srsran::create_gtpu_echo(gtpu_echo_creation_message& msg)
{
  return std::make_unique<gtpu_echo_impl>(*msg.gtpu_pcap, *msg.tx_upper);
}
