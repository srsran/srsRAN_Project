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

#include "qos_flow_context.h"
#include "srsgnb/asn1/e1ap.h"
#include "srsgnb/pdcp/pdcp_entity.h"
#include "srsgnb/sdap/sdap_entity.h"
#include <map>

namespace srsgnb {
namespace srs_cu_up {

/// \brief DRB context with map to all QoS flows.
struct drb_context {
  drb_context(const asn1::e1ap::drb_to_setup_item_ng_ran_s& drb) : drb_id(drb.drb_id){};

  int16_t drb_id;

  std::unique_ptr<pdcp_entity> pdcp_bearer;

  uint8_t cell_group_id; /// This can/should be a list of cell groups.

  std::map<uint8_t, std::unique_ptr<qos_flow_context>> qos_flows; // key is qos_flow_id
};

} // namespace srs_cu_up
} // namespace srsgnb
