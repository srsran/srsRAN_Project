/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_F1AP_ASN1_PACKER_H
#define SRSGNB_F1AP_ASN1_PACKER_H

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/f1_interface/f1ap_du.h"
#include "srsgnb/gateways/network_gateway.h"
#include "srsgnb/srslog/srslog.h"
#include <cstdio>

namespace srsgnb {

class f1ap_asn1_packer : public f1c_pdu_handler
{
public:
  explicit f1ap_asn1_packer(network_gateway_data_handler& gw, f1c_pdu_handler& f1c);

  void handle_packed_pdu(const byte_buffer& pdu);

  void handle_unpacked_pdu(const asn1::f1ap::f1_ap_pdu_c& pdu) override;

private:
  srslog::basic_logger& logger;
  network_gateway_data_handler& gw;
  f1c_pdu_handler&              f1c;
};

} // namespace srsgnb

#endif // SRSGNB_F1AP_ASN1_PACKER_H
