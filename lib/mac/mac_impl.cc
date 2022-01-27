
#include "mac_impl.h"

namespace srsgnb {

mac_impl::mac_impl(mac_config_notifier& listener, mac_northbound_notifier& ul_sdu_notifier_) :
  notifier(listener), ul_sdu_notifier(ul_sdu_notifier_)
{}

void mac_impl::push_ul_ccch_msg(rnti_t rnti, lcid_t lcid, byte_buffer pdu)
{
  ul_sdu_notifier.push_sdu(rnti, lcid, pdu);
}

} // namespace srsgnb