#ifndef SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_SIGNAL_PROCESSOR_FACTORIES_H
#define SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_SIGNAL_PROCESSOR_FACTORIES_H

#include "srsgnb/phy/upper/signal_processors/dmrs_pusch_estimator.h"

namespace srsgnb {

struct pusch_estimator_generic_configuration {
  // TBD.
};

std::unique_ptr<dmrs_pusch_estimator> create_pusch_estimator_generic(pusch_estimator_generic_configuration& config);

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_SIGNAL_PROCESSOR_FACTORIES_H
