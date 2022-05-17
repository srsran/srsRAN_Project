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
#include "rlc_impl.h"

using namespace srsgnb;

std::unique_ptr<rlc_pdu_handler>
srsgnb::create_rlc_ul_bearer(du_ue_index_t ue_index, lcid_t lcid, rlc_sdu_rx_notifier& notifier)
{
  std::unique_ptr<rlc_pdu_handler> instance = std::make_unique<rlc_ul_um_bearer>(ue_index, lcid, notifier);
  return instance;
}
