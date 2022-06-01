
#ifndef SRSGNB_F1C_COMMON_H
#define SRSGNB_F1C_COMMON_H

#include "srsgnb/asn1/f1ap.h"

namespace srsgnb {

/// This interface represents the F1-C interface used to transmit F1AP messages.
class f1c_message_handler
{
public:
  virtual ~f1c_message_handler() = default;

  /// Handle the incoming F1AP message.
  virtual void handle_message(const asn1::f1ap::f1_ap_pdu_c& msg) = 0;
};

/// Interface used by F1 to inform about events.
class f1c_event_handler
{
public:
  virtual ~f1c_event_handler()          = default;
  virtual void handle_connection_loss() = 0;
};

/// This interface notifies the reception of new F1AP messages over the F1-C interface.
class f1c_message_notifier
{
public:
  virtual ~f1c_message_notifier() = default;

  /// This callback is invoked on each received F1AP message.
  virtual void on_new_message(const asn1::f1ap::f1_ap_pdu_c& msg) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_F1C_COMMON_H
