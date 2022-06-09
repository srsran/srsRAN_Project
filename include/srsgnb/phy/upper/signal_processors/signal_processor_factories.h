#ifndef SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_SIGNAL_PROCESSOR_FACTORIES_H
#define SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_SIGNAL_PROCESSOR_FACTORIES_H

#include "srsgnb/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pusch_estimator.h"

namespace srsgnb {

class dmrs_pdsch_processor_factory
{
public:
  virtual ~dmrs_pdsch_processor_factory()                = default;
  virtual std::unique_ptr<dmrs_pdsch_processor> create() = 0;
};

std::shared_ptr<dmrs_pdsch_processor_factory>
create_dmrs_pdsch_processor_factory_sw(std::shared_ptr<pseudo_random_generator_factory> prg_factory);

struct pusch_estimator_generic_configuration {
  // TBD.
};

std::unique_ptr<dmrs_pusch_estimator> create_pusch_estimator_generic(pusch_estimator_generic_configuration& config);

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_SIGNAL_PROCESSORS_SIGNAL_PROCESSOR_FACTORIES_H
