/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/support/io/io_broker_factory.h"
#include "io_broker_epoll.h"

using namespace srsran;

std::unique_ptr<io_broker> srsran::create_io_broker(io_broker_type type, const io_broker_config& config)
{
  switch (type) {
    case io_broker_type::epoll:
      return std::make_unique<io_broker_epoll>(config);
    default:
      srsran_terminate("IO broker type not supported");
  }
  return nullptr;
}
