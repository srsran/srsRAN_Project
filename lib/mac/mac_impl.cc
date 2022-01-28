
#include "mac_impl.h"

namespace srsgnb {

mac_impl::mac_impl(mac_config_notifier& cfg_notifier_, mac_northbound_notifier& ul_sdu_notifier_) :
  cfg_notifier(cfg_notifier_), ul_sdu_notifier(ul_sdu_notifier_), logger(srslog::fetch_basic_logger("MAC"))
{}

void mac_impl::push_ul_pdu(rnti_t rnti, du_cell_index_t cell_index, byte_buffer pdu)
{
  logger.info("UL\t0x%x\tcc=%d\tCCCH", rnti, cell_index);
  ul_sdu_notifier.push_sdu(rnti, 0, pdu);
}

void mac_impl::ue_create_request(const mac_ue_create_request_message& cfg)
{
  logger.info("Created UE ID=%d, rnti=0x%x", cfg.ue_index, cfg.crnti);

  mac_ue_create_request_response_message resp{};
  resp.ue_index   = cfg.ue_index;
  resp.cell_index = cfg.cell_index;
  resp.result     = true;
  cfg_notifier.ue_create_request_complete(resp);
}

} // namespace srsgnb