#ifndef SRSGNB_FAPI_SLOT_MESSAGE_GATEWAY_H
#define SRSGNB_FAPI_SLOT_MESSAGE_GATEWAY_H

namespace srsgnb {
namespace fapi {

class slot_message_gateway
{
public:
  virtual void dl_tti_request()  = 0;
  virtual void ul_tti_request()  = 0;
  virtual void ul_dci_request()  = 0;
  virtual void tx_data_request() = 0;
};

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_FAPI_SLOT_MESSAGE_GATEWAY_H
