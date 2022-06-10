/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_RLC_MANAGER_FACTORY_H
#define SRSGNB_RLC_MANAGER_FACTORY_H

#include "rlc.h"
#include "srsgnb/rlc/rlc_common_bearer.h"
#include <memory>

namespace srsgnb {

/// Creates an instance of a RLC bearer
std::unique_ptr<rlc_common_bearer> create_rlc_bearer(du_ue_index_t                     ue_index,
                                                     lcid_t                            lcid,
                                                     rlc_rx_upper_layer_data_plane&    upper_dp,
                                                     rlc_tx_upper_layer_control_plane& upper_cp);
} // namespace srsgnb

#endif // SRSGNB_RLC_MANAGER_FACTORY_H
