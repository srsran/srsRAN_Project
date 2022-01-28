
#ifndef SRSGNB_MAC_IMPL_H
#define SRSGNB_MAC_IMPL_H

#include "srsgnb/mac/mac.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

class mac_impl : public mac_interface
{
public:
  explicit mac_impl(mac_config_notifier& listener, mac_northbound_notifier& ul_sdu_notifier);

  void ue_create_request(const mac_ue_create_request_message& cfg) override;
  void ue_reconfiguration_request(const mac_ue_reconfiguration_request& cfg) override {}
  void ue_delete_request(const mac_ue_delete_request& cfg) override {}

  void push_ul_pdu(rnti_t rnti, du_cell_index_t cell_index, byte_buffer pdu) override;

private:
  mac_config_notifier&     cfg_notifier;
  mac_northbound_notifier& ul_sdu_notifier;
  srslog::basic_logger&    logger;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_IMPL_H
