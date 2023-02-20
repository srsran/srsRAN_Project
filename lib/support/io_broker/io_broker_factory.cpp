/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/support/io_broker/io_broker_factory.h"
#include "io_broker_epoll.h"

using namespace srsran;

std::unique_ptr<io_broker> srsran::create_io_broker(io_broker_type type)
{
  switch (type) {
    case io_broker_type::epoll:
      return std::make_unique<io_broker_epoll>(io_broker_config());
    default:
      srsgnb_terminate("IO broker type not supported");
  }
  return nullptr;
}
