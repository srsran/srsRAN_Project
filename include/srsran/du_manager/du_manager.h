
#pragma once

#include "du_configurator.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/ran/du_types.h"
#include "srsran/ran/lcid.h"
#include "srsran/ran/rnti.h"
#include "srsran/support/async/async_task.h"
#include <string>

namespace srsran {

struct ul_ccch_indication_message;

namespace srs_du {

/// Interface used to handle external events (e.g. UL CCCH).
class du_manager_mac_event_handler
{
public:
  virtual ~du_manager_mac_event_handler() = default;

  /// \brief Handle UL CCCH message arrival.
  virtual void handle_ul_ccch_indication(const ul_ccch_indication_message& msg) = 0;
};

/// This class handles updates in cell and UE configurations. TODO: Better naming needed.
class du_manager_configurator
{
public:
  virtual ~du_manager_configurator() = default;

  /// \brief Schedule asynchronous task in a DU-wide scope.
  virtual void schedule_async_task(async_task<void>&& task) = 0;

  /// \brief Schedule asynchronous task that is specific to a UE.
  virtual void schedule_async_task(du_ue_index_t ue_index, async_task<void>&& task) = 0;

  /// \brief Request a currently unused and unallocated UE index from the DU.
  virtual du_ue_index_t find_unused_du_ue_index() = 0;

  /// \brief Create a new UE context in the DU with an assigned UE index.
  virtual async_task<f1ap_ue_context_creation_response>
  handle_ue_context_creation(const f1ap_ue_context_creation_request& request) = 0;

  /// \brief Update the UE configuration in the DU, namely its SRBs and DRBs.
  virtual async_task<f1ap_ue_context_update_response>
  handle_ue_context_update(const f1ap_ue_context_update_request& request) = 0;

  /// \brief Remove UE context from the DU.
  virtual async_task<void> handle_ue_delete_request(const f1ap_ue_delete_request& request) = 0;

  /// \brief Handle the transfer of resources of old UE to new Reestablishing UE and deletion of the old UE context.
  virtual void handle_ue_reestablishment(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index) = 0;
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

  /// \brief Stop the DU manager. This call is blocking and only returns once all tasks in the DU manager are completed.
  virtual void stop() = 0;
};

class du_manager_interface : public du_manager_interface_query,
                             public du_manager_controller,
                             public du_manager_configurator,
                             public du_manager_mac_event_handler,
                             public du_configurator
{
public:
  virtual ~du_manager_interface() = default;
};

} // namespace srs_du
} // namespace srsran
