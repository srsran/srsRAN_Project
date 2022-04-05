
#ifndef SRSGNB_RADIO_RADIO_FACTORY_H
#define SRSGNB_RADIO_RADIO_FACTORY_H

#include "srsgnb/radio/radio_session.h"
#include "srsgnb/support/executors/task_executor.h"
#include <memory>

namespace srsgnb {

/// Describes a radio session factory for a determined radio driver.
class radio_factory : public radio_base
{
public:
  /// \brief Creates a new radio session with the given configuration parameters.
  /// \param[in] config Provides the configuration parameters.
  /// \param[in] task_executor Provides a task executor for executing asynchronous tasks.
  /// \param[in] notifier Provides radio event notifier interface.
  /// \return The ownership to a radio session if the session was successfully created.
  virtual std::unique_ptr<radio_session>
  create(const radio_configuration::radio& config, task_executor& async_task_executor, radio_notifier& notifier) = 0;
};

std::unique_ptr<radio_factory> create_radio_factory(std::string driver_name);

} // namespace srsgnb

#endif // SRSGNB_RADIO_RADIO_FACTORY_H
