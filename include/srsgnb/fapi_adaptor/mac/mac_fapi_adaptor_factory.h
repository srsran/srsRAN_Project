/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_FAPI_ADAPTOR_MAC_MAC_FAPI_ADAPTOR_FACTORY_H
#define SRSGNB_FAPI_ADAPTOR_MAC_MAC_FAPI_ADAPTOR_FACTORY_H

#include "srsgnb/fapi_adaptor/mac/mac_fapi_adaptor.h"

namespace srsgnb {
namespace fapi_adaptor {

struct mac_fapi_adaptor_factory_config {
  unsigned sector_id;
};

/// Factory that creates mac_fapi adaptors.
class mac_fapi_adaptor_factory
{
public:
  virtual ~mac_fapi_adaptor_factory() = default;

  /// Creates and returns a mac_fapi_adaptor using the given configuration.
  std::unique_ptr<mac_fapi_adaptor> create(mac_fapi_adaptor_factory_config config) = 0;
};

} // namespace fapi_adaptor
} // namespace srsgnb

#endif // SRSGNB_FAPI_ADAPTOR_MAC_MAC_FAPI_ADAPTOR_FACTORY_H
