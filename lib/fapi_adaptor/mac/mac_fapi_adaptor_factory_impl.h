/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_FAPI_ADAPTOR_MAC_MAC_FAPI_ADAPTOR_FACTORY_IMPL_H
#define SRSGNB_LIB_FAPI_ADAPTOR_MAC_MAC_FAPI_ADAPTOR_FACTORY_IMPL_H

#include "srsgnb/fapi_adaptor/mac/mac_fapi_adaptor_factory.h"

namespace srsgnb {
namespace fapi_adaptor {

class mac_fapi_adaptor_factory_impl : public mac_fapi_adaptor_factory
{
public:
  // See interface for documentation.
  std::unique_ptr<mac_fapi_adaptor> create(mac_fapi_adaptor_factory_config config) override;
};

} // namespace fapi_adaptor
} // namespace srsgnb

#endif // SRSGNB_LIB_FAPI_ADAPTOR_MAC_MAC_FAPI_ADAPTOR_FACTORY_IMPL_H
