/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
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

std::unique_ptr<mac_fapi_adaptor>
mac_fapi_adaptor_factory_impl::create(const mac_fapi_adaptor_factory_config&  config,
                                      mac_fapi_adaptor_factory_dependencies&& dependencies)
{
  return std::make_unique<mac_fapi_adaptor_impl>(dependencies.gateway.get(),
                                                 dependencies.last_msg_notifier.get(),
                                                 std::move(dependencies.pm_mapper),
                                                 config.sector_id,
                                                 config.cell_nof_prbs,
                                                 config.scs);
}

std::unique_ptr<mac_fapi_adaptor_factory> srsran::fapi_adaptor::create_mac_fapi_adaptor_factory()
{
  return std::make_unique<mac_fapi_adaptor_factory_impl>();
}
