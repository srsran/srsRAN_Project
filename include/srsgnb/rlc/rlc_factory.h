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
#include <memory>

namespace srsgnb {

/// Creates an instance of a RLC UL bearer
std::unique_ptr<rlc_rx_pdu_handler>
create_rlc_ul_bearer(du_ue_index_t ue_index, lcid_t lcid, rlc_rx_upper_layer_data_plane& upper_dp);

/// Creates an instance of a RLC UL bearer
std::unique_ptr<rlc_tx_sdu_handler>
create_rlc_dl_bearer(du_ue_index_t ue_index, lcid_t lcid, rlc_tx_upper_layer_control_plane& upper_cp);

} // namespace srsgnb

#endif // SRSGNB_RLC_MANAGER_FACTORY_H
