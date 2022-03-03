
#include "du_ue_manager.h"
#include "ue_creation_procedure.h"

using namespace srsgnb;

du_ue_manager::du_ue_manager(du_manager_config_t& cfg_) : cfg(cfg_), logger(cfg.logger)
{
  const size_t number_of_pending_procedures = 16;
  for (size_t i = 0; i < MAX_NOF_UES; ++i) {
    ue_ctrl_loop.emplace(i, number_of_pending_procedures);
  }
}

void du_ue_manager::handle_ue_create_request(const du_ue_create_message& msg)
{
  // Enqueue UE creation procedure
  ue_ctrl_loop[msg.ue_index].schedule<ue_creation_procedure>(msg, cfg, *this);
}

du_ue_context* du_ue_manager::find_ue(du_ue_index_t ue_index)
{
  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_index);
  return ue_db.contains(ue_index) ? &ue_db[ue_index] : nullptr;
}

du_ue_context* du_ue_manager::add_ue(du_ue_context ue_ctx)
{
  srsran_assert(ue_ctx.rnti != INVALID_RNTI, "Invalid RNTI");
  srsran_assert(ue_ctx.ue_index < MAX_NOF_UES, "Invalid ue_index=%d", ue_ctx.ue_index);

  if (ue_db.contains(ue_ctx.ue_index)) {
    // UE already existed with same ue_index
    return nullptr;
  }

  // Create UE object
  ue_db.emplace(ue_ctx.ue_index, std::move(ue_ctx));
  return &ue_db[ue_ctx.ue_index];
}

void du_ue_manager::remove_ue(du_ue_index_t ue_index)
{
  // Note: The caller of this function can be a UE procedure. Thus, we have to wait for the procedure to finish
  // before safely removing the UE. This achieved via a scheduled async task

  srsran_assert(ue_index < MAX_NOF_UES, "Invalid ueId={}", ue_index);
  if (not ue_db.contains(ue_index)) {
    logger.warning("Failed to find ueId={}", ue_index);
    return;
  }
  logger.debug("Scheduling ueId={} deletion", ue_index);

  // Schedule UE removal task
  ue_ctrl_loop[ue_index].schedule([this, ue_index](coro_context<async_task<void> >& ctx) {
    CORO_BEGIN(ctx);
    ue_db.erase(ue_index);
    logger.info("Removed ueId={}", ue_index);
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
