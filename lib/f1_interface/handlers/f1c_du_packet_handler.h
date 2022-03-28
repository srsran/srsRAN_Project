#ifndef SRSGNB_F1AP_IMPL_H
#define SRSGNB_F1AP_IMPL_H

/// Notice that we are not using any implementation specific header files. We are decoupled.
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/f1_interface/f1ap_packet_procedures.h"
#include <cstdio>
#include <memory>

namespace srsgnb {

class f1c_du_packet_handler //: public f1ap_input_gateway
{
public:
  explicit f1c_du_packet_handler(std::unique_ptr<f1ap_packet_procedures> procedure_);

  void handle(const asn1::f1ap::f1_ap_pdu_c& pdu); // override;

private:
  srslog::basic_logger& logger;

  std::unique_ptr<f1ap_packet_procedures> procedure;
};

} // namespace srsgnb

#endif // SRSGNB_F1AP_IMPL_H
