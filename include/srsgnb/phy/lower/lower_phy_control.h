#ifndef SRSGNB_PHY_LOWER_PHY_CONTROL_H
#define SRSGNB_PHY_LOWER_PHY_CONTROL_H

#include "srsgnb/phy/lower/lower_phy.h"
#include "srsgnb/support/executors/task_executor.h"

namespace srsgnb {
/// Describes the lower physical layer control interface.
class lower_phy_control
{
public:
  /// Default destructor.
  virtual ~lower_phy_control() = default;

  /// \brief Gets the lower physical layer input gateway.
  /// \return A reference to the input gateway.
  virtual lower_phy_input_gateway& get_input_gateway() = 0;

  /// \brief Starts the lower physical layer operation.
  /// \param[in] realtime_task_exeutor Provides a task executor for real time execution.
  virtual void start(task_executor& realtime_task_exeutor) = 0;

  /// Stops the lower physical layer operation.
  virtual void stop() = 0;
};
} // namespace srsgnb

#endif // SRSGNB_PHY_LOWER_PHY_CONTROL_H
