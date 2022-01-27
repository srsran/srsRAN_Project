#ifndef SRSGNB_F1AP_IMPL_H
#define SRSGNB_F1AP_IMPL_H

/// Notice that we are not using any implementation specific header files. We are decoupled.
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/f1ap/f1ap_du.h"
#include "srsgnb/f1ap/f1ap_packet_procedures.h"
#include <cstdio>
#include <memory>

namespace srsgnb {

//class f1ap_du_packet_handler : public f1ap_input_gateway
//{
//  /// In reality, as the interfaces grow, this would become a reference or ptr and ownership would be managed from a
//  /// different class. For simplicity, we keep it this way for now.
//  /// This is important as the dependency with f1ap_packet_procedures interface would eventually disappear, although it
//  /// is an interface and not a concrete class the less dependencies the better.
//  std::unique_ptr<f1ap_packet_procedures> procedure;
//
//public:
//  explicit f1ap_du_packet_handler(std::unique_ptr<f1ap_packet_procedures> procedure) : procedure(std::move(procedure)) {}
//
//  void handle(const byte_buffer& pdu) override;
//};

} // namespace srsgnb

#endif // SRSGNB_F1AP_IMPL_H
