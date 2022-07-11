/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/fapi/slot_message_gateway.h"
#include "srsgnb/fapi_adaptor/mac/mac_fapi_adaptor.h"
#include "srsgnb/ran/subcarrier_spacing.h"
#include <memory>

namespace srsgnb {
namespace fapi_adaptor {

struct mac_fapi_adaptor_factory_config {
  mac_fapi_adaptor_factory_config(unsigned sector_id, subcarrier_spacing scs, fapi::slot_message_gateway& gateway) :
    sector_id(sector_id), gateway(gateway), scs(scs)
  {
  }

  unsigned                                           sector_id;
  std::reference_wrapper<fapi::slot_message_gateway> gateway;
  subcarrier_spacing                                 scs;
};

/// Factory that creates mac_fapi adaptors.
class mac_fapi_adaptor_factory
{
public:
  virtual ~mac_fapi_adaptor_factory() = default;

  /// Creates and returns a mac_fapi_adaptor using the given configuration.
  virtual std::unique_ptr<mac_fapi_adaptor> create(mac_fapi_adaptor_factory_config config) = 0;
};

/// Creates and returns a mac_fapi_adaptor_factory.
std::unique_ptr<mac_fapi_adaptor_factory> create_mac_fapi_adaptor_factory();

} // namespace fapi_adaptor
} // namespace srsgnb
