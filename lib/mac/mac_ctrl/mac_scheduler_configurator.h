
#ifndef SRSGNB_MAC_SCHEDULER_CONFIGURATOR_H
#define SRSGNB_MAC_SCHEDULER_CONFIGURATOR_H

#include "srsgnb/mac/mac_ue_configurator.h"
#include "srsgnb/scheduler/scheduler_configurator.h"
#include "srsgnb/support/async/async_task.h"

namespace srsgnb {

/// \brief Adapter interface between MAC and scheduler that allows the srsGNB MAC to configure with different scheduler
/// implementations.
/// Note: This adapter interface makes scheduler configuration methods awaitable.
class mac_scheduler_configurator
{
public:
  virtual ~mac_scheduler_configurator() = default;

  virtual void add_cell(const mac_cell_creation_request& msg) = 0;

  virtual void remove_cell(du_cell_index_t cell_index) = 0;

  virtual async_task<bool> handle_ue_creation_request(const mac_ue_create_request_message& msg) = 0;

  virtual async_task<bool> handle_ue_reconfiguration_request(const mac_ue_reconfiguration_request_message& msg) = 0;

  /// \brief Removes UE from MAC scheduler in an asynchronous manner.
  /// The scheduler shouldn't allocate more grants directed at the UE being removed after this procedure is complete.
  virtual async_task<bool> handle_ue_deletion_request(const mac_ue_delete_request_message& msg) = 0;

  virtual sched_configuration_notifier& get_sched_notifier() = 0;
};

} // namespace srsgnb

#endif // SRSGNB_MAC_SCHEDULER_CONFIGURATOR_H
