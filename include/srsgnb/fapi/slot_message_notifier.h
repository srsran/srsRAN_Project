#ifndef SRSGNB_FAPI_SLOT_MESSAGE_NOTIFIER_H
#define SRSGNB_FAPI_SLOT_MESSAGE_NOTIFIER_H

namespace srsgnb {
namespace fapi {

class slot_message_notifier
{
public:
  virtual void on_dl_tti_response()    = 0;
  virtual void on_slot_indication()    = 0;
  virtual void on_rx_data_indication() = 0;
  virtual void on_crc_indication()     = 0;
  virtual void on_uci_indication()     = 0;
  virtual void on_srs_indication()     = 0;
  virtual void on_rach_indication()    = 0;
};

} // namespace fapi
} // namespace srsgnb

#endif // SRSGNB_FAPI_SLOT_MESSAGE_NOTIFIER_H
