
#pragma once

#include "srsran/du/du_high/du_manager/du_configurator.h"
#include "srsran/f1ap/du/f1ap_du.h"
#include "srsran/f1ap/du/f1ap_du_positioning_handler.h"
#include "srsran/mac/mac_cell_manager.h"
#include "srsran/ran/du_types.h"
#include "srsran/support/async/async_task.h"

namespace srsran {

struct ul_ccch_indication_message;
struct mac_metric_report;
struct scheduler_cell_metrics;

namespace srs_du {

/// Interface used to handle external events (e.g. UL CCCH).
class du_manager_mac_event_handler
{
public:
  virtual ~du_manager_mac_event_handler() = default;

  /// \brief Handle UL CCCH message arrival.
  virtual void handle_ul_ccch_indication(const ul_ccch_indication_message& msg) = 0;
};

/// Interface used to handle metric reports from the DU-high remaining layers.
class du_manager_mac_metric_aggregator
{
public:
  virtual ~du_manager_mac_metric_aggregator() = default;

  /// \brief Handle the metrics report from the MAC.
  virtual void aggregate_mac_metrics_report(const mac_metric_report& report) = 0;
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

  /// Handle the F1-C connection loss event.
  virtual void handle_f1c_connection_loss() = 0;

  /// \brief Request a currently unused and unallocated UE index from the DU.
  virtual du_ue_index_t find_unused_du_ue_index() = 0;

  /// \brief Handle reset of UE contexts as per request from the F1AP.
  virtual async_task<void> handle_f1_reset_request(const std::vector<du_ue_index_t>& ues_to_reset) = 0;

  /// \brief Handle request to update context by the CU via F1AP.
  virtual async_task<gnbcu_config_update_response>
  handle_cu_context_update_request(const gnbcu_config_update_request& request) = 0;

  /// \brief Create a new UE context in the DU with an assigned UE index.
  virtual async_task<f1ap_ue_context_creation_response>
  handle_ue_context_creation(const f1ap_ue_context_creation_request& request) = 0;

  /// \brief Update the UE configuration in the DU, namely its SRBs and DRBs.
  virtual async_task<f1ap_ue_context_update_response>
  handle_ue_context_update(const f1ap_ue_context_update_request& request) = 0;

  /// \brief Remove UE context from the DU.
  virtual async_task<void> handle_ue_delete_request(const f1ap_ue_delete_request& request) = 0;

  /// \brief Deactivate DRB activity for a given UE.
  virtual async_task<void> handle_ue_deactivation_request(du_ue_index_t ue_index) = 0;

  /// \brief Handle the transfer of resources of old UE to new Reestablishing UE and deletion of the old UE context.
  virtual void handle_ue_reestablishment(du_ue_index_t new_ue_index, du_ue_index_t old_ue_index) = 0;

  /// Handle the confirmation that the UE has applied the last received RRC config.
  virtual void handle_ue_config_applied(du_ue_index_t ue_index) = 0;

  /// Retrieve handler of UE positioning.
  virtual f1ap_du_positioning_handler& get_positioning_handler() = 0;
};

class du_manager_interface_query
{
public:
  virtual ~du_manager_interface_query() = default;
  virtual size_t nof_ues()              = 0;
};

/// Interface to access the DU MAC time-slot mapper.
class du_manager_time_mapper_accessor
{
public:
  virtual ~du_manager_time_mapper_accessor() = default;
  /// Get MAC cell time-slot mapper.
  virtual mac_cell_time_mapper& get_time_mapper() = 0;
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
                             public du_manager_time_mapper_accessor,
                             public du_manager_controller,
                             public du_manager_configurator,
                             public du_manager_mac_event_handler,
                             public du_configurator
{
public:
  virtual ~du_manager_interface() = default;

  /// Get entity responsibly for aggregating metrics from all DU layers.
  virtual du_manager_mac_metric_aggregator& get_metrics_aggregator() = 0;
};

} // namespace srs_du
} // namespace srsran
