
#ifndef SRSGNB_MAC_CTRL_TEST_DUMMIES_H
#define SRSGNB_MAC_CTRL_TEST_DUMMIES_H

#include "../../lib/mac/mac_config_interfaces.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/support/async/manual_event.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {

class mac_ctrl_dummy_configurer final : public mac_ctrl_configurer
{
public:
  bool add_ue(du_ue_index_t ue_index, rnti_t rnti, du_cell_index_t pcell_index) override { return true; }
  void remove_ue(du_ue_index_t ue_index) override {}
};

/// Dummy class to emulate MAC UL procedures to add/remove/configure UEs
class mac_ul_dummy_configurer final : public mac_ul_configurer
{
public:
  bool                                             expected_result = true;
  manual_event_flag                                ue_created_ev;
  optional<mac_ue_create_request_message>          last_ue_create_request;
  optional<mac_ue_delete_request_message>          last_ue_delete_request;
  optional<mac_ue_reconfiguration_request_message> last_ue_reconfiguration_request;

  async_task<bool> add_ue(const mac_ue_create_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<bool> >& ctx) {
      CORO_BEGIN(ctx);
      last_ue_create_request = msg;
      CORO_AWAIT(ue_created_ev);
      CORO_RETURN(expected_result);
    });
  }

  async_task<void> remove_ue(const mac_ue_delete_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<void> >& ctx) {
      CORO_BEGIN(ctx);
      last_ue_delete_request = msg;
      CORO_RETURN();
    });
  }

  async_task<bool> reconfigure_ue(const mac_ue_reconfiguration_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<bool> >& ctx) {
      CORO_BEGIN(ctx);
      last_ue_reconfiguration_request = msg;
      CORO_RETURN(true);
    });
  }
};

class mac_dl_dummy_configurer final : public mac_dl_configurer
{
public:
  bool                                             expected_result = true;
  manual_event_flag                                ue_created_ev;
  optional<mac_ue_create_request_message>          last_ue_create_request;
  optional<mac_ue_delete_request_message>          last_ue_delete_request;
  optional<mac_ue_reconfiguration_request_message> last_ue_reconfiguration_request;

  async_task<bool> add_ue(const mac_ue_create_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<bool> >& ctx) {
      CORO_BEGIN(ctx);
      last_ue_create_request = msg;
      CORO_AWAIT(ue_created_ev);
      CORO_RETURN(expected_result);
    });
  }
  async_task<void> remove_ue(const mac_ue_delete_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<void> >& ctx) {
      CORO_BEGIN(ctx);
      last_ue_delete_request = msg;
      CORO_RETURN();
    });
  }

  async_task<bool> reconfigure_ue(const mac_ue_reconfiguration_request_message& msg) override
  {
    return launch_async([this, msg](coro_context<async_task<bool> >& ctx) {
      CORO_BEGIN(ctx);
      last_ue_reconfiguration_request = msg;
      CORO_RETURN(true);
    });
  }
};

class dummy_ul_executor_mapper : public du_l2_ul_executor_mapper
{
public:
  dummy_ul_executor_mapper(task_executor& exec_) : exec(exec_) {}

  task_executor& rebind_executor(rnti_t rnti, du_cell_index_t pcell_index) override { return executor(rnti); }
  task_executor& executor(rnti_t rnti) override { return exec; }

  task_executor& exec;
};

class dummy_mac_event_indicator : public mac_event_notifier
{
public:
  optional<ul_ccch_indication_message> last_ccch_ind;

  void on_ul_ccch_msg_received(const ul_ccch_indication_message& msg) override { last_ccch_ind = msg; }
};

} // namespace srsgnb

#endif // SRSGNB_MAC_CTRL_TEST_DUMMIES_H
