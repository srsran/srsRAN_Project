
#include "du_manager_impl.h"

namespace srsgnb {

du_manager_impl::du_manager_impl(const du_manager_config_t& cfg_) : cfg(cfg_), ue_mng(cfg) {}

void du_manager_impl::ue_create(const du_ue_create_message& msg)
{
  // Switch DU Manager exec context
  cfg.du_mng_exec->execute([this, msg]() {
    // Start UE create procedure
    ue_mng.handle_ue_create_request(msg);
  });
}

std::string du_manager_impl::get_ues()
{
  fmt::memory_buffer fmtbuf;
  //  fmt::format_to(fmtbuf, "[");
  //  const char* sep = ", ";
  //  for (auto& u : ue_db) {
  //    fmt::format_to(fmtbuf, "{}{{id={}, rnti=0x{:x}, cc={}}}", sep, u.ctxt.ue_index, u.ctxt.rnti,
  //    u.ctxt.pcell_index); sep = "";
  //  }
  //  fmt::format_to(fmtbuf, "]");
  fmt::format_to(fmtbuf, "{}", ue_mng.get_ues().size());
  return fmt::to_string(fmtbuf);
}

} // namespace srsgnb
