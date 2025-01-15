/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/pdcp/pdcp_factory.h"
#include "pdcp_entity_impl.h"

/// Notice this would be the only place were we include concrete class implementation files.

using namespace srsran;

std::unique_ptr<pdcp_entity> srsran::create_pdcp_entity(pdcp_entity_creation_message& msg)
{
  return std::make_unique<pdcp_entity_impl>(msg.ue_index,
                                            msg.rb_id,
                                            msg.config,
                                            *msg.tx_lower,
                                            *msg.tx_upper_cn,
                                            *msg.rx_upper_dn,
                                            *msg.rx_upper_cn,
                                            msg.ue_dl_timer_factory,
                                            msg.ue_ul_timer_factory,
                                            msg.ue_ctrl_timer_factory,
                                            *msg.ue_dl_executor,
                                            *msg.ue_ul_executor,
                                            *msg.crypto_executor,
                                            msg.max_nof_crypto_workers);
}
