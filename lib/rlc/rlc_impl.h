
#ifndef SRSGNB_RLC_IMPL_H
#define SRSGNB_RLC_IMPL_H

#include "srsgnb/rlc/rlc.h"
#include "srsran/srslog/srslog.h"

namespace srsgnb {

class rlc_ul_um_bearer : public rlc_ul_bearer
{
public:
  explicit rlc_ul_um_bearer(du_ue_index_t du_index_, lcid_t lcid_, rlc_ul_sdu_notifier& notifier_) :
    du_index(du_index_), lcid(lcid_), notifier(notifier_)
  {}

  void push_pdu(const byte_buffer& pdu) override { notifier.on_ul_sdu(du_index, lcid, pdu); }

private:
  const du_ue_index_t  du_index;
  const lcid_t         lcid;
  rlc_ul_sdu_notifier& notifier;
};

} // namespace srsgnb

#endif // SRSGNB_RLC_IMPL_H
