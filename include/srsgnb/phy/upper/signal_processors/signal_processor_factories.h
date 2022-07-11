#pragma once

#include "srsgnb/phy/upper/sequence_generators/sequence_generator_factories.h"
#include "srsgnb/phy/upper/signal_processors/csi_rs_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pbch_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdcch_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pdsch_processor.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pusch_estimator.h"

namespace srsgnb {

std::unique_ptr<csi_rs_processor> create_csi_rs_processor();

std::unique_ptr<dmrs_pbch_processor> create_dmrs_pbch_processor();

std::unique_ptr<dmrs_pdcch_processor> create_dmrs_pdcch_processor();

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
