
#pragma once

#include "srsgnb/adt/byte_buffer.h"
#include "srsgnb/asn1/f1ap.h"
#include "srsgnb/ran/du_types.h"
#include "srsgnb/ran/lcid.h"
#include "srsgnb/ran/rnti.h"
#include "srsgnb/support/async/async_task.h"
#include <string>

namespace srsgnb {

struct ul_ccch_indication_message;

namespace srs_du {

struct du_ue_delete_message {
  du_ue_index_t ue_index;
};

/// Interface used to handle external events (e.g. UL CCCH).
class du_manager_ccch_handler
{
public:
  virtual ~du_manager_ccch_handler()                                            = default;
  virtual void handle_ul_ccch_indication(const ul_ccch_indication_message& msg) = 0;
};

/// Interface to schedule asynchronous tasks respective to a given UE.
class du_manager_ue_task_scheduler
{
public:
  virtual ~du_manager_ue_task_scheduler() = default;

  /// \brief Schedule asynchronous task that is specific to a UE.
  virtual void schedule_async_task(async_task<void>&& task) = 0;
};

class du_manager_task_scheduler
{
public:
  virtual ~du_manager_task_scheduler() = default;

  virtual du_manager_ue_task_scheduler& get_ue_task_scheduler(du_ue_index_t ue_index) = 0;
};

class du_manager_interface_query
{
public:
  virtual ~du_manager_interface_query() = default;
  virtual size_t nof_ues()              = 0;
};

class du_manager_controller
{
public:
  virtual ~du_manager_controller() = default;
  virtual void start()             = 0;
  virtual void stop()              = 0;
};

class du_manager_interface : public du_manager_interface_query,
                             public du_manager_ccch_handler,
                             public du_manager_controller,
                             public du_manager_task_scheduler
{
public:
  virtual ~du_manager_interface() = default;
};

} // namespace srs_du
} // namespace srsgnb
