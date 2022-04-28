
#ifndef SRSGNB_PHY_UPPER_CHANNEL_ESTIMATE_H
#define SRSGNB_PHY_UPPER_CHANNEL_ESTIMATE_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/ran/frame_types.h"
#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

/// \brief Describes channel estimation results.
class channel_estimate
{
public:
  /// Noise estimate (represented as linear value and in dB).
  float noise_estimate      = 0.0f;
  float noise_estimate_dbFs = 0.0f;
  /// Reference Signal Recieved Power (represented as linear value and in dB).
  float rsrp      = 0.0f;
  float rsrp_dBfs = 0.0f;
  /// Energy per resource element (represented as linear value and in dB).
  float epre      = 0.0f;
  float epre_dBfs = 0.0f;
  // Signal to noise ratio (represented as linear value and in dB).
  float snr    = 0.0f;
  float snr_db = 0.0f;
  /// Dynamic array dedicated for storing channel estimates.
  std::vector<cf_t> ce;

  /// Construct given number of RBs used by reference signal.
  channel_estimate(unsigned nof_prb) noexcept
  {
    size_t prbs        = std::max(nof_prb, MAX_RB);
    size_t slot_len_re = prbs * NRE * NOF_OFDM_SYM_PER_SLOT_NORMAL_CP;
    // allocate memory for channel estimates and initialize with 1.0
    ce.resize(slot_len_re, 1.0);
  }

  /// Default destructor
  ~channel_estimate() = default;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_ESTIMATE_H
