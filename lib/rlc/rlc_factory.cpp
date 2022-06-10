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
#include "rlc_um_bearer.h"

namespace srsgnb {

std::unique_ptr<rlc_common_bearer> create_rlc_bearer(du_ue_index_t                        ue_index,
                                                     lcid_t                               lcid,
                                                     rlc_rx_upper_layer_data_notifier&    upper_dn,
                                                     rlc_tx_upper_layer_control_notifier& upper_cn)
{
  std::unique_ptr<rlc_common_bearer> instance = std::make_unique<rlc_um_bearer>(ue_index, lcid, upper_dn, upper_cn);
  return instance;
}

} // namespace srsgnb
