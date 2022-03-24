#ifndef SRSGNB_F1AP_DU_FACTORY_H
#define SRSGNB_F1AP_DU_FACTORY_H

#include "srsgnb/f1_interface/f1ap_du.h"
#include <memory>

namespace srsgnb {

/// Creates an instance of an F1AP interface, notifying outgoing packets on the specified listener object.
std::unique_ptr<f1ap_du_interface> create_f1ap_du(f1ap_du_pdu_notifier& pdu_listener, f1_gateway& f1_gw);

} // namespace srsgnb

#endif // SRSGNB_F1AP_DU_FACTORY_H
