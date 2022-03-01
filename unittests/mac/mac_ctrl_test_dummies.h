
#ifndef SRSGNB_MAC_CTRL_TEST_DUMMIES_H
#define SRSGNB_MAC_CTRL_TEST_DUMMIES_H

#include "../../lib/mac/mac_config_interfaces.h"
#include "srsgnb/adt/optional.h"
#include "srsgnb/support/task_executor.h"

namespace srsgnb {

/// Just runs enqueued tasks inline
class inline_executor : public task_executor
{
public:
  void execute(unique_task task) final { task(); }
};

/// Records MAC completed configurations (instead of notifying DU manager)
class mac_config_notification_recorder : public mac_config_notifier
{
public:
  optional<mac_ue_create_request_response_message>  last_ue_created;
  optional<mac_ue_delete_response_message>          last_ue_deleted;
  optional<mac_ue_reconfiguration_response_message> last_ue_reconfigured;

  void on_ue_create_request_complete(const mac_ue_create_request_response_message& resp) override
  {
    last_ue_created = resp;
  }
  void on_ue_reconfiguration_complete(const mac_ue_reconfiguration_response_message& resp) override
  {
    last_ue_reconfigured = resp;
  }
  void on_ue_delete_complete(const mac_ue_delete_response_message& resp) override { last_ue_deleted = resp; }
};

class mac_ctrl_dummy_configurer final : public mac_ctrl_configurer
{
public:
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

} // namespace srsgnb

#endif // SRSGNB_MAC_CTRL_TEST_DUMMIES_H
