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

#include "../adapters/du_bearer_adapter.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/rlc/rlc_config.h"
#include "srsgnb/rlc/rlc_entity.h"

namespace srsgnb {
namespace srs_du {

/// \brief Configuration of a DU bearer.
struct du_bearer {
  lcid_t             lcid;
  optional<drb_id_t> drbid; // Valid if DRB.
  rlc_config         rlc_cfg;

  std::unique_ptr<rlc_entity> rlc_bearer;
  du_bearer_adapter           bearer_connector;
};

} // namespace srs_du
} // namespace srsgnb