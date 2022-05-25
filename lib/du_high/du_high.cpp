/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_high.h"
#include "adapters.h"
#include "du_high_executor_strategies.h"
#include "srsgnb/du_manager/du_manager_factory.h"
#include "srsgnb/f1_interface/f1ap_du_factory.h"
#include "srsgnb/mac/mac_factory.h"

namespace srsgnb {

void assert_du_high_configuration_valid(const du_high_configuration& cfg)
{
  srsran_assert(cfg.du_mng_executor != nullptr, "Invalid DU manager executor");
  srsran_assert(cfg.dl_executors != nullptr, "Invalid DL executor mapper");
  srsran_assert(cfg.ul_executors != nullptr, "Invalid UL executor mapper");
}

du_high::du_high(const du_high_configuration& config_) : cfg(config_)
{
  assert_du_high_configuration_valid(cfg);

  // Create layers
  mac  = create_mac(mac_ev_notifier, *cfg.ul_executors, *cfg.dl_executors, *cfg.du_mng_executor, *cfg.phy_adapter);
  f1ap = create_f1ap_du(*cfg.f1c_msg_hdl);
  du_manager = create_du_manager(
      mac->get_ue_configurator(), mac->get_cell_manager(), *f1ap, *f1ap, rlc_sdu_notifier, *cfg.du_mng_executor);

  // Connect Layer->DU manager notifiers.
  mac_ev_notifier.connect(*du_manager);
}

du_high::~du_high()
{
  stop();
}

void du_high::start() {}

void du_high::stop() {}

f1c_message_handler& du_high::get_f1c_message_handler()
{
  return *f1ap;
}

mac_pdu_handler& du_high::get_pdu_handler(du_cell_index_t cell_idx)
{
  return mac->get_pdu_handler(cell_idx);
}

mac_cell_slot_handler& du_high::get_slot_handler(du_cell_index_t cell_idx)
{
  return mac->get_slot_handler(cell_idx);
}

} // namespace srsgnb
