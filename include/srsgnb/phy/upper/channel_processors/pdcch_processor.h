#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_PROCESSOR_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_PROCESSOR_H

#include "srsgnb/adt/static_vector.h"
#include <complex>

namespace srsgnb {

/// Describes the PDCCH processor interface.
class pdcch_processor
{
public:
  /// Describes the PDCCH processing parameters.
  struct pdu_t {};

  /// \brief Processes a PDCCH transmission.
  ///
  /// \param[out] grid Provides the destination resource grid.
  /// \param[in] codewords Provides the codewords to transmit.
  /// \param[in] pdu Provides the necessary parameters to process the PDCCH transmission.
  virtual void process(resource_grid_writer& grid, span<const> data, pdu_t& pdu) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_PROCESSOR_H
