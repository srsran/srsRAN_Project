/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_F1_TEST_HELPERS_H
#define SRSGNB_LIB_F1_TEST_HELPERS_H

#include "srsgnb/cu_cp/cu_cp_manager.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/f1_interface/common/f1c_common.h"
#include "srsgnb/f1_interface/cu/f1ap_cu.h"

namespace srsgnb {

class dummy_f1c_du_processor_message_notifier : public srs_cu_cp::f1c_du_processor_message_notifier
{
public:
  dummy_f1c_du_processor_message_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}
  srs_cu_cp::f1_setup_request_message last_f1_setup_request_message;
  void on_f1_setup_request_received(const srs_cu_cp::f1_setup_request_message& msg) override
  {
    logger.info("Received F1SetupRequest message.");
    last_f1_setup_request_message = msg;
  }

  srs_cu_cp::du_cell_index_t find_cell(uint64_t packed_nr_cell_id) override { return srs_cu_cp::MIN_DU_CELL_INDEX; }

private:
  srslog::basic_logger& logger;
};

class dummy_f1c_ue_manager_message_notifier : public srs_cu_cp::f1c_ue_manager_message_notifier
{
public:
  dummy_f1c_ue_manager_message_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}

  srs_cu_cp::ue_index_t
  on_initial_ul_rrc_message_transfer_received(const srs_cu_cp::du_cell_index_t          pcell_index,
                                              const srs_cu_cp::f1ap_initial_ul_rrc_msg& msg) override
  {
    logger.info("Received Initial UL RRC Message transfer message.");
    return srs_cu_cp::MIN_UE_INDEX;
  }

  void on_ul_rrc_message_transfer_received(const srs_cu_cp::ue_index_t       ue_index,
                                           const srs_cu_cp::f1ap_ul_rrc_msg& msg) override
  {
    logger.info("Received UL RRC Message transfer message.");
  }

private:
  srslog::basic_logger& logger;
};

/// Reusable class implementing the notifier interface.
class f1c_null_notifier : public f1c_message_notifier
{
public:
  f1c_msg last_f1c_msg;

  void on_new_message(const f1c_msg& msg) override
  {
    srslog::basic_logger& test_logger = srslog::fetch_basic_logger("TEST");
    test_logger.info("Received F1 AP PDU");
    last_f1c_msg = msg;
  }
};

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_f1c_pdu_notifier : public f1c_message_notifier
{
public:
  dummy_f1c_pdu_notifier(f1c_message_handler* handler_) :
    logger(srslog::fetch_basic_logger("TEST")), handler(handler_){};

  void attach_handler(f1c_message_handler* handler_) { handler = handler_; };
  void on_new_message(const f1c_msg& msg) override
  {
    logger.info("Received a F1AP PDU of type {}", msg.pdu.type().to_string());
    last_f1c_msg = msg; // store msg

    if (handler != nullptr) {
      logger.info("Forwarding F1AP PDU");
      handler->handle_message(msg);
    }
  }
  f1c_msg last_f1c_msg;

private:
  srslog::basic_logger& logger;
  f1c_message_handler*  handler = nullptr;
};

/// Reusable notifier class that a) stores the received PDU for test inspection and b)
/// calls the registered PDU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_cu_cp_f1c_pdu_notifier : public f1c_message_notifier
{
public:
  dummy_cu_cp_f1c_pdu_notifier(srs_cu_cp::cu_cp_manager_interface* manager_, f1c_message_handler* handler_) :
    logger(srslog::fetch_basic_logger("TEST")), manager(manager_), handler(handler_){};

  void attach_handler(srs_cu_cp::cu_cp_manager_interface* manager_, f1c_message_handler* handler_)
  {
    manager = manager_;
    handler = handler_;
    manager->on_new_connection();
  };
  void on_new_message(const f1c_msg& msg) override
  {
    logger.info("Received a F1AP PDU of type {}", msg.pdu.type().to_string());
    last_f1c_msg = msg; // store msg

    if (handler != nullptr) {
      logger.info("Forwarding F1AP PDU");
      handler->handle_message(msg);
    }
  }
  f1c_msg last_f1c_msg;

private:
  srslog::basic_logger&               logger;
  srs_cu_cp::cu_cp_manager_interface* manager = nullptr;
  f1c_message_handler*                handler = nullptr;
};

/// Reusable notifier class that a) stores the received du_index for test inspection and b)
/// calls the registered DU handler (if any). The handler can be added upon construction
/// or later via the attach_handler() method.
class dummy_f1c_du_management_notifier : public srs_cu_cp::f1c_du_management_notifier
{
public:
  dummy_f1c_du_management_notifier(srs_cu_cp::cu_cp_manager_du_handler* handler_) :
    logger(srslog::fetch_basic_logger("TEST")), handler(handler_){};

  void attach_handler(srs_cu_cp::cu_cp_manager_du_handler* handler_) { handler = handler_; };
  void on_du_remove_request_received(const srs_cu_cp::du_index_t idx) override
  {
    logger.info("Received a du remove request for du {}", idx);
    last_du_idx = idx; // store idx

    if (handler != nullptr) {
      logger.info("Forwarding remove request");
      handler->handle_du_remove_request(idx);
    }
  }
  srs_cu_cp::du_index_t last_du_idx;

private:
  srslog::basic_logger&                logger;
  srs_cu_cp::cu_cp_manager_du_handler* handler = nullptr;
};

/// Dummy handler just printing the received PDU.
class dummy_f1c_message_handler : public f1c_message_handler
{
public:
  dummy_f1c_message_handler() : logger(srslog::fetch_basic_logger("TEST")){};
  void handle_message(const f1c_msg& msg) override
  {
    logger.info("Received a F1AP PDU of type %s\n", msg.pdu.type().to_string());
  }

private:
  srslog::basic_logger& logger;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_F1_TEST_HELPERS_H
