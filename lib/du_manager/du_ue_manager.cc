
#include "du_ue_manager.h"
#include "ue_creation_procedure.h"

using namespace srsgnb;

du_ue_manager::du_ue_manager(du_manager_config_t& cfg_) : cfg(cfg_), logger(cfg.logger) {}

void du_ue_manager::create_ue(const du_ue_create_message& msg)
{
  ue_impl* ue = add_ue(msg.ue_index, msg.crnti, msg.cell_index);
  if (ue == nullptr) {
    logger.error("Creating already existing ueId={}", msg.ue_index);
    // Send response back to F1AP
    du_ue_create_response_message f1ap_resp{};
    f1ap_resp.ue_index = msg.ue_index;
    f1ap_resp.result   = false;
    cfg.f1ap_cfg_notifier->on_du_ue_create_response(f1ap_resp);
    return;
  }

  // Enqueue UE creation procedure
  ue->ctrl_loop.schedule<ue_creation_procedure>(msg, cfg, ue->ctxt, *this);
}

du_ue_manager::ue_impl* du_ue_manager::add_ue(du_ue_index_t ue_index, rnti_t crnti, du_cell_index_t pcell_index)
{
  srsran_assert(crnti != INVALID_RNTI, "Invalid RNTI");
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_index);

  if (ue_db.contains(ue_index)) {
    // UE already existed with same ue_index
    return nullptr;
  }

  // Create UE object
  ue_db.emplace(ue_index);
  ue_impl& u         = ue_db[ue_index];
  u.ctxt.ue_index    = ue_index;
  u.ctxt.rnti        = crnti;
  u.ctxt.pcell_index = pcell_index;
  return &u;
}

void du_ue_manager::remove_ue(du_ue_index_t ue_index)
{
  // Note: The caller of this function can be a UE procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the UE. This achieved via an async task in the MAC main control loop

  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ueId={}", ue_index);
  if (not ue_db.contains(ue_index)) {
    logger.warning("Failed to find ueId={}", ue_index);
    return;
  }
  logger.debug("Scheduling ueId={} deletion", ue_index);

  // Schedule task removal
  main_ctrl_loop.schedule([this, ue_index](coro_context<async_task<void> >& ctx) {
    CORO_BEGIN(ctx);
    srsran_sanity_check(ue_db.contains(ue_index), "ueId={} was unexpectedly removed", ue_index);

    CORO_AWAIT(ue_db[ue_index].ctrl_loop.request_stop());

    logger.info("Removing ueId={}", ue_index);
    ue_db.erase(ue_index);

    CORO_RETURN();
  });
}

std::string du_ue_manager::get_ues()
{
  fmt::memory_buffer fmtbuf;
  //  fmt::format_to(fmtbuf, "[");
  //  const char* sep = ", ";
  //  for (auto& u : ue_db) {
  //    fmt::format_to(fmtbuf, "{}{{id={}, rnti=0x{:x}, cc={}}}", sep, u.ctxt.ue_index, u.ctxt.rnti,
  //    u.ctxt.pcell_index); sep = "";
  //  }
  //  fmt::format_to(fmtbuf, "]");
  fmt::format_to(fmtbuf, "{}", ue_db.size());
  return fmt::to_string(fmtbuf);
}
