#ifndef SRSGNB_FAPI_CONFIG_MESSAGE_GATEWAY_H
#define SRSGNB_FAPI_CONFIG_MESSAGE_GATEWAY_H

namespace srsgnb {
namespace fapi {

class config_message_gateway
{
public:
  virtual void param_request()  = 0;
  virtual void config_request() = 0;
  virtual void start_request()  = 0;
  virtual void stop_request()   = 0;
};

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_FAPI_CONFIG_MESSAGE_GATEWAY_H
