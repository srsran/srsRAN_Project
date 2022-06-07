/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/rlc/rlc_factory.h"
#include "rlc_rx_um_bearer.h"

using namespace srsgnb;

std::unique_ptr<rlc_rx_pdu_handler>
srsgnb::create_rlc_ul_bearer(du_ue_index_t ue_index, lcid_t lcid, rlc_rx_upper_layer_data_plane& upper_dp)
{
  std::unique_ptr<rlc_rx_pdu_handler> instance = std::make_unique<rlc_rx_um_bearer>(ue_index, lcid, upper_dp);
  return instance;
}
