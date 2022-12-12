/*
 *
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/f1u/common/f1u_connector.h"
#include <memory>

namespace srsgnb {

struct f1u_connector_creation_message {
  // TODO
};

/// \brief Creates an F1-U connector for the DU and CU-UP.
std::unique_ptr<f1u_connector> create_f1u_connector(const f1u_connector_creation_message& msg);

} // namespace srsgnb
