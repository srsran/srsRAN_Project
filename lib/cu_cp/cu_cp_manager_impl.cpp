/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "cu_cp_manager_impl.h"

namespace srsgnb {

cu_cp_manager_impl::cu_cp_manager_impl(const cu_cp_manager_config_t& cfg_) : cfg(cfg_), ue_mng(cfg), main_ctrl_loop(128)
{
  // nothing to start straigt away on the CU
  cu_cp_ctx = {}; // make it compile
}

void cu_cp_manager_impl::handle_ul_rrc_message_transfer(const ul_rrc_message_transfer_message& msg)
{
  // TODO: add handling and start procedure if needed
}

size_t cu_cp_manager_impl::nof_ues()
{
  // TODO: This is temporary code.
  static std::mutex              mutex;
  static std::condition_variable cvar;
  size_t                         result = MAX_NOF_CU_CP_UES;
  cfg.cu_cp_mng_exec->execute([this, &result]() {
    std::unique_lock<std::mutex> lock(mutex);
    result = ue_mng.get_ues().size();
    cvar.notify_one();
  });
  {
    std::unique_lock<std::mutex> lock(mutex);
    while (result == MAX_NOF_DU_UES) {
      cvar.wait(lock);
    }
  }
  return result;
}

} // namespace srsgnb
