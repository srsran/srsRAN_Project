/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_fapi_adaptor_factory_impl.h"
#include "mac_fapi_adaptor_impl.h"

using namespace srsgnb;
using namespace fapi_adaptor;

std::unique_ptr<mac_fapi_adaptor> mac_fapi_adaptor_factory_impl::create(const mac_fapi_adaptor_factory_config& config)
{
  return std::make_unique<mac_fapi_adaptor_impl>(
      config.gateway.get(), config.last_msg_notifier.get(), config.sector_id, config.scs);
}

std::unique_ptr<mac_fapi_adaptor_factory> srsgnb::fapi_adaptor::create_mac_fapi_adaptor_factory()
{
  return std::make_unique<mac_fapi_adaptor_factory_impl>();
}
