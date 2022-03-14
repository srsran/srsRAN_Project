
#ifndef SRSGNB_RLC_IMPL_H
#define SRSGNB_RLC_IMPL_H

#include "srsgnb/rlc/rlc.h"
#include "srsgnb/srslog/srslog.h"

namespace srsgnb {

class rlc_ul_um_bearer : public rlc_pdu_handler
{
public:
  rlc_ul_um_bearer(du_ue_index_t du_index, lcid_t lcid, rlc_sdu_rx_notifier& notifier) :
    du_index(du_index), lcid(lcid), notifier(notifier)
  {}

  void handle_pdu(byte_buffer pdu) override { notifier.on_new_sdu(du_index, lcid, pdu); }

private:
  const du_ue_index_t  du_index;
  const lcid_t         lcid;
  rlc_sdu_rx_notifier& notifier;
};

} // namespace srsgnb

#endif // SRSGNB_RLC_IMPL_H
