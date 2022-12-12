/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "f1u_connector_impl.h"
#include "srsgnb/f1u/common/f1u_connector_factory.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsgnb;

std::unique_ptr<f1u_connector> srsgnb::create_f1u_connector(const f1u_connector_creation_message& msg)
{
  return std::make_unique<f1u_connector_impl>();
}
