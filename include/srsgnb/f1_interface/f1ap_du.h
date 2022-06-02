
#ifndef SRSGNB_F1AP_DU_H
#define SRSGNB_F1AP_DU_H

#include "f1c_common.h"
#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/adt/expected.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {

struct f1ap_du_ue_create_request {
  du_ue_index_t ue_index;
};

struct f1ap_du_ue_create_response {
  bool result;
};

struct f1_rx_pdu {
  du_ue_index_t ue_index;
  lcid_t        lcid;
  byte_buffer   pdu;
};

struct ul_rrc_message_delivery_status {
  du_cell_index_t cell_index;
  du_ue_index_t   ue_index;
  lcid_t          lcid;
  bool            rrc_delivery_status;
};

/// TODO: Rename/change type of Rx PDU
class f1ap_du_ul_interface
{
public:
  virtual ~f1ap_du_ul_interface()                                                           = default;
  virtual void ul_rrc_message_delivery_report(const ul_rrc_message_delivery_status& report) = 0;
  //: TODO: remove this in favour of f1c_message_handler
  virtual void handle_pdu(f1_rx_pdu pdu) = 0;
};

struct du_setup_params {
  // mandatory
  uint64_t gnb_du_id;
  uint8_t  rrc_version;

  // optional
  std::string gnb_du_name;

  int max_setup_retries = 5;
};

struct f1_setup_request_message {
  du_setup_params setup_params;
};

struct f1_du_setup_response_message {
  asn1::f1ap::f1_setup_resp_s msg;
  bool                        success;
};

class f1ap_du_configurator
{
public:
  virtual ~f1ap_du_configurator() = default;

  /// \brief Initiates the F1 Setup procedure.
  /// \param[in] request The F1SetupRequest message to transmit.
  /// \return Returns a f1_du_setup_response_message struct with the success member set to 'true' in case of a
  /// successful outcome, 'false' otherwise. \remark The DU transmits the F1SetupRequest as per TS 38.473 section 8.2.3
  /// and awaits the response. If a F1SetupFailure is received the F1AP will handle the failure.
  virtual async_task<f1_du_setup_response_message>
  handle_f1ap_setup_request(const f1_setup_request_message& request) = 0;

  /// Initiates creation of UE context in F1.
  virtual async_task<f1ap_du_ue_create_response> handle_ue_creation_request(const f1ap_du_ue_create_request& msg) = 0;
};

/// Combined entry point for F1C/U handling.
class f1_du_interface : public f1c_message_handler,
                        public f1c_event_handler,
                        public f1ap_du_ul_interface,
                        public f1ap_du_configurator
{
public:
  virtual ~f1_du_interface() = default;
};

} // namespace srsgnb

#endif // SRSGNB_F1AP_DU_H
