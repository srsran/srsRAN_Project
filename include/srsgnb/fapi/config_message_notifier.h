#ifndef SRSGNB_FAPI_CONFIG_MESSAGE_NOTIFIER_H
#define SRSGNB_FAPI_CONFIG_MESSAGE_NOTIFIER_H

namespace srsgnb {
namespace fapi {

class config_message_notifier
{
public:
  virtual void on_param_response()  = 0;
  virtual void on_config_response() = 0;
  virtual void on_stop_indication() = 0;
};

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_FAPI_CONFIG_MESSAGE_NOTIFIER_H
