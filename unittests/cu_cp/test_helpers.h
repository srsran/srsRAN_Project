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

#include "../../lib/cu_cp/adapters/du_processor_adapters.h"
#include "srsgnb/cu_cp/cu_cp.h"
#include "srsgnb/cu_cp/cu_cp_types.h"
#include "srsgnb/cu_cp/ue_context.h"
#include "srsgnb/support/async/async_task_loop.h"

namespace srsgnb {
namespace srs_cu_cp {

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
  dummy_du_processor_cu_cp_notifier(cu_cp_du_handler* cu_cp_handler_) :
    logger(srslog::fetch_basic_logger("TEST")), cu_cp_handler(cu_cp_handler_)
  {
  }

  void attach_handler(cu_cp_du_handler* cu_cp_handler_) { cu_cp_handler = cu_cp_handler_; }

  void on_rrc_ue_created(du_index_t du_index, ue_index_t ue_index, rrc_ue_interface* rrc_ue) override
  {
    logger.info("Received a RRC UE creation notification");

    if (cu_cp_handler != nullptr) {
      cu_cp_handler->handle_rrc_ue_creation(du_index, ue_index, rrc_ue);
    }
  }

private:
  srslog::basic_logger& logger;
  cu_cp_du_handler*     cu_cp_handler = nullptr;
};

struct dummy_du_processor_e1ap_control_notifier : public du_processor_e1ap_control_notifier {
public:
  dummy_du_processor_e1ap_control_notifier() : logger(srslog::fetch_basic_logger("TEST")) {}

  async_task<e1ap_bearer_context_setup_response>
  on_bearer_context_setup_request(const e1ap_bearer_context_setup_request& msg) override
  {
    logger.info("Received a new bearer context setup request");

    return launch_async([res = e1ap_bearer_context_setup_response{},
                         msg](coro_context<async_task<e1ap_bearer_context_setup_response>>& ctx) mutable {
      CORO_BEGIN(ctx);
      if (msg.ue_dl_aggregate_maximum_bit_rate > 0) {
        res.success = true;
      }
      CORO_RETURN(res);
    });
  }

  async_task<e1ap_bearer_context_modification_response>
  on_bearer_context_modification_request(const e1ap_bearer_context_modification_request& msg) override
  {
    logger.info("Received a new bearer context modification request");

    return launch_async([res = e1ap_bearer_context_modification_response{}](
                            coro_context<async_task<e1ap_bearer_context_modification_response>>& ctx) mutable {
      CORO_BEGIN(ctx);

      CORO_RETURN(res);
    });
  }

private:
  srslog::basic_logger& logger;
};

/// \brief Generate DU-to-CU RRC Container with CellGroupConfig.
byte_buffer generate_container_with_cell_group_config();

/// \brief Generate RRC Container with RRC Setup Complete message.
byte_buffer generate_rrc_setup_complete();

} // namespace srs_cu_cp
} // namespace srsgnb
