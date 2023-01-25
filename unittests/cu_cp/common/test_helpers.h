/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "du_processor_test_messages.h"
#include "lib/cu_cp/adapters/du_processor_adapters.h"
#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_cp/ue_context.h"
#include "srsgnb/support/async/async_task_loop.h"
#include "srsgnb/support/test_utils.h"

namespace srsgnb {
namespace srs_cu_cp {

/// \brief Generate DU-to-CU RRC Container with CellGroupConfig.
byte_buffer generate_container_with_cell_group_config();

/// \brief Generate RRC Container with RRC Setup Complete message.
byte_buffer generate_rrc_setup_complete();

/// \brief Generate a random gnb_cu_cp_ue_e1ap_id
gnb_cu_cp_ue_e1ap_id_t generate_random_gnb_cu_cp_ue_e1ap_id();

/// \brief Generate a random gnb_cu_up_ue_e1ap_id
gnb_cu_up_ue_e1ap_id_t generate_random_gnb_cu_up_ue_e1ap_id();

/// \brief Generate a random gnb_cu_ue_f1ap_id
gnb_cu_ue_f1ap_id_t generate_random_gnb_cu_ue_f1ap_id();

/// \brief Generate a random gnb_du_ue_f1ap_id
gnb_du_ue_f1ap_id_t generate_random_gnb_du_ue_f1ap_id();

struct dummy_du_processor_to_cu_cp_task_scheduler : public du_processor_to_cu_cp_task_scheduler {
public:
  dummy_du_processor_to_cu_cp_task_scheduler(timer_manager& timers_) : timer_db(timers_) {}
  void schedule_async_task(du_index_t du_index, ue_index_t ue_index, async_task<void>&& task) override
  {
    ctrl_loop.schedule(std::move(task));
  }
  unique_timer   make_unique_timer() override { return timer_db.create_unique_timer(); }
  timer_manager& get_timer_manager() override { return timer_db; }

  void tick_timer() { timer_db.tick_all(); }

private:
  async_task_sequencer ctrl_loop{16};
  timer_manager&       timer_db;
};

struct dummy_du_processor_cu_cp_notifier : public du_processor_cu_cp_notifier {
public:
  explicit dummy_du_processor_cu_cp_notifier(cu_cp_du_handler* cu_cp_handler_ = nullptr) : cu_cp_handler(cu_cp_handler_)
  {
  }

  void attach_handler(cu_cp_du_handler* cu_cp_handler_) { cu_cp_handler = cu_cp_handler_; }

  void on_new_du_connection() override { logger.info("Received a new DU connection notification"); }

  void on_rrc_ue_created(du_index_t du_index, ue_index_t ue_index, rrc_ue_interface* rrc_ue) override
  {
    logger.info("Received a RRC UE creation notification");

    if (cu_cp_handler != nullptr) {
      cu_cp_handler->handle_rrc_ue_creation(du_index, ue_index, rrc_ue);
    }
  }

private:
  srslog::basic_logger& logger        = srslog::fetch_basic_logger("TEST");
  cu_cp_du_handler*     cu_cp_handler = nullptr;
};

struct dummy_du_processor_e1ap_control_notifier : public du_processor_e1ap_control_notifier {
public:
  dummy_du_processor_e1ap_control_notifier() {}

  void set_bearer_context_setup_outcome(bool outcome) { bearer_context_setup_outcome = outcome; }

  void set_bearer_context_modification_outcome(bool outcome) { bearer_context_modification_outcome = outcome; }

  async_task<e1ap_bearer_context_setup_response>
  on_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg) override
  {
    logger.info("Received a new bearer context setup request");

    return launch_async([res = e1ap_bearer_context_setup_response{}, msg, this](
                            coro_context<async_task<e1ap_bearer_context_setup_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      if (bearer_context_setup_outcome) {
        // generate random ids to make sure its not only working for hardcoded values
        gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = generate_random_gnb_cu_cp_ue_e1ap_id();
        gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = generate_random_gnb_cu_up_ue_e1ap_id();

        res = generate_e1ap_bearer_context_setup_response(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);
      } else {
        res.success = false;
      }

      CORO_RETURN(res);
    });
  }

  async_task<e1ap_bearer_context_modification_response>
  on_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg) override
  {
    logger.info("Received a new bearer context modification request");

    return launch_async([res = e1ap_bearer_context_modification_response{},
                         this](coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      if (bearer_context_modification_outcome) {
        // generate random ids to make sure its not only working for hardcoded values
        gnb_cu_cp_ue_e1ap_id_t cu_cp_ue_e1ap_id = generate_random_gnb_cu_cp_ue_e1ap_id();
        gnb_cu_up_ue_e1ap_id_t cu_up_ue_e1ap_id = generate_random_gnb_cu_up_ue_e1ap_id();

        res = generate_e1ap_bearer_context_modification_response(cu_cp_ue_e1ap_id, cu_up_ue_e1ap_id);
      } else {
        res.success = false;
      }

      CORO_RETURN(res);
    });
  }

private:
  srslog::basic_logger& logger                              = srslog::fetch_basic_logger("TEST");
  bool                  bearer_context_setup_outcome        = false;
  bool                  bearer_context_modification_outcome = false;
};

struct dummy_du_processor_f1ap_ue_context_notifier : public du_processor_f1ap_ue_context_notifier {
public:
  dummy_du_processor_f1ap_ue_context_notifier() {}

  void set_ue_context_setup_outcome(bool outcome) { ue_context_setup_outcome = outcome; }

  void set_ue_context_modification_outcome(bool outcome) { ue_context_modification_outcome = outcome; }

  async_task<f1ap_ue_context_setup_response>
  on_ue_context_setup_request(const f1ap_ue_context_setup_request& request) override
  {
    logger.info("Received a new UE context setup request");

    return launch_async([res = f1ap_ue_context_setup_response{},
                         this](coro_context<async_task<f1ap_ue_context_setup_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      res.success = ue_context_setup_outcome;

      CORO_RETURN(res);
    });
  }

  async_task<cu_cp_ue_context_modification_response>
  on_ue_context_modification_request(const cu_cp_ue_context_modification_request& request) override
  {
    logger.info("Received a new UE context modification request");

    return launch_async([res = cu_cp_ue_context_modification_response{},
                         this](coro_context<async_task<cu_cp_ue_context_modification_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      if (ue_context_modification_outcome) {
        // generate random ids to make sure its not only working for hardcoded values
        gnb_cu_ue_f1ap_id_t cu_ue_f1ap_id = generate_random_gnb_cu_ue_f1ap_id();
        gnb_du_ue_f1ap_id_t du_ue_f1ap_id = generate_random_gnb_du_ue_f1ap_id();

        res = generate_cu_cp_ue_context_modification_response(cu_ue_f1ap_id, du_ue_f1ap_id);
      } else {
        res.success = false;
      }

      CORO_RETURN(res);
    });
  }

  async_task<ue_index_t> on_ue_context_release_command(const f1ap_ue_context_release_command& msg) override
  {
    logger.info("Received a new UE context release command");

    return launch_async([msg](coro_context<async_task<ue_index_t>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(msg.ue_index);
    });
  }

private:
  srslog::basic_logger& logger                          = srslog::fetch_basic_logger("TEST");
  bool                  ue_context_setup_outcome        = false;
  bool                  ue_context_modification_outcome = false;
};

struct dummy_du_processor_rrc_ue_control_message_notifier : public du_processor_rrc_ue_control_message_notifier {
public:
  dummy_du_processor_rrc_ue_control_message_notifier() {}

  void set_rrc_reconfiguration_outcome(bool outcome) { rrc_reconfiguration_outcome = outcome; }

  void on_new_guami(const guami& msg) override { logger.info("Received a new GUAMI"); }

  async_task<bool> on_rrc_reconfiguration_request(const cu_cp_rrc_reconfiguration_procedure_request& msg) override
  {
    logger.info("Received a new RRC reconfiguration request");

    return launch_async([this](coro_context<async_task<bool>>& ctx) mutable {
      CORO_BEGIN(ctx);
      CORO_RETURN(rrc_reconfiguration_outcome);
    });
  }

private:
  srslog::basic_logger& logger                      = srslog::fetch_basic_logger("TEST");
  bool                  rrc_reconfiguration_outcome = false;
};

} // namespace srs_cu_cp
} // namespace srsgnb
