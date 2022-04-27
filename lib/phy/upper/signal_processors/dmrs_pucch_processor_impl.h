
#ifndef SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_IMPL_H

#include "srsgnb/phy/upper/signal_processors/pucch_orthogonal_sequence.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pucch_processor.h"
#include <srsgnb/phy/upper/sequence_generators/low_papr_sequence_collection.h>

namespace srsgnb {

/// Creates a generic DMRS for PUCCH instance.
std::unique_ptr<dmrs_pucch_processor> create_dmrs_pucch_processor(const pucch_format&                 format,
                                                                  const low_papr_sequence_collection* c   = nullptr,
                                                                  const pucch_orthogonal_sequence*    occ = nullptr);
} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_SIGNAL_PROCESSORS_DMRS_PUCCH_PROCESSOR_IMPL_H