/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "du_manager_impl.h"
#include "procedures/initial_du_setup_procedure.h"

using namespace srsgnb;
using namespace srs_du;

du_manager_impl::du_manager_impl(const du_manager_params& params) :
  cfg{srslog::fetch_basic_logger("DU-MNG"),
      {},
      {params.ran.cells.begin(), params.ran.cells.end()},
      params.services,
      params.f1ap,
      params.rlc,
      params.mac},
  cell_mng(cfg),
  ue_mng(cfg),
  main_ctrl_loop(128)
{
}

void du_manager_impl::start()
{
  // start F1 setup procedure.
  main_ctrl_loop.schedule<initial_du_setup_procedure>(cfg, cell_mng);
}

void du_manager_impl::stop()
{
  // TODO.
}

void du_manager_impl::handle_ul_ccch_indication(const ul_ccch_indication_message& msg)
{
  // Switch DU Manager exec context
  cfg.services.du_mng_exec.execute([this, msg = std::move(msg)]() {
    // Start UE create procedure
    ue_mng.handle_ue_create_request(msg);
  });
}

async_task<f1ap_ue_config_update_response>
du_manager_impl::handle_ue_config_update(const f1ap_ue_config_update_request& request)
{
  return ue_mng.handle_ue_config_request(request);
}

async_task<void> du_manager_impl::handle_ue_delete_request(const f1ap_ue_delete_request& request)
{
  return ue_mng.handle_ue_delete_request(request);
}

size_t du_manager_impl::nof_ues()
{
  // TODO: This is temporary code.
  static std::mutex              mutex;
  static std::condition_variable cvar;
  size_t                         result = MAX_NOF_DU_UES;
  cfg.services.du_mng_exec.execute([this, &result]() {
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
