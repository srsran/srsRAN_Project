/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "mac_fapi_adaptor_factory_impl.h"
#include "mac_fapi_adaptor_impl.h"

using namespace srsran;
using namespace fapi_adaptor;

std::unique_ptr<mac_fapi_adaptor> mac_fapi_adaptor_factory_impl::create(const mac_fapi_adaptor_config&  config,
                                                                        mac_fapi_adaptor_dependencies&& dependencies)
{
  srsran_assert(dependencies.gateway, "Invalid FAPI slot message gateway");
  srsran_assert(dependencies.last_msg_notifier, "Invalid FAPI last message notifier");

  return std::make_unique<mac_fapi_adaptor_impl>(config, std::move(dependencies));
}

std::unique_ptr<mac_fapi_adaptor_factory> srsran::fapi_adaptor::create_mac_fapi_adaptor_factory()
{
  return std::make_unique<mac_fapi_adaptor_factory_impl>();
}
