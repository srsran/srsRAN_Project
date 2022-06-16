#ifndef SRSGNB_PHY_LOWER_LOWER_PHY_CONTROLLER_H
#define SRSGNB_PHY_LOWER_LOWER_PHY_CONTROLLER_H

#include "srsgnb/phy/lower/lower_phy_input_gateway.h"
#include "srsgnb/phy/lower/lower_phy_rx_symbol_notifier.h"
#include "srsgnb/phy/lower/lower_phy_timing_notifier.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {
/// Describes the lower physical layer control interface.
class lower_phy_controller : public lower_phy_input_gateway
{
public:
  /// \brief Starts the lower physical layer operation.
  /// \param[in] realtime_task_executor Provides a task executor for real time execution.
  /// \note The real time task executor is exclusively dedicated to the lower physical layer operation.
  virtual void start(task_executor& realtime_task_executor) = 0;

  /// Stops the lower physical layer operation.
  virtual void stop() = 0;
};
} // namespace srsgnb

#endif // SRSGNB_PHY_LOWER_LOWER_PHY_CONTROLLER_H
