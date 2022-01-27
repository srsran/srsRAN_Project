
#include "du.h"
#include "adapters.h"
#include "srsgnb/du_manager/du_manager_factory.h"
#include "srsgnb/f1ap/f1ap_du_factory.h"
#include "srsgnb/mac/mac_factory.h"
#include "srsgnb/rlc/rlc_factory.h"

namespace srsgnb {

du::du()
{
  // Create RLC
  rlc = create_rlc(rlc_cfg_notifier);

  // Create MAC
  mac = create_mac(mac_cfg_notifier, mac_ul_sdu_notifier);

  // Create DU manager
  du_manager = create_du_manager(*mac, *rlc, f1ap_cfg_notifier);

  // Create F1AP
  f1ap = create_f1ap_du(f1ap_pdu_adapter, *du_manager);

  // Connect DU blocks
  mac_cfg_notifier.connect(*du_manager);
  mac_ul_sdu_notifier.connect(*f1ap);
  rlc_cfg_notifier.connect(*du_manager);
  f1ap_cfg_notifier.connect(*f1ap);
}

void du::push_pusch(rnti_t rnti, lcid_t lcid, byte_buffer pdu)
{
  mac->push_ul_ccch_msg(rnti, lcid, std::move(pdu));
}

} // namespace srsgnb
