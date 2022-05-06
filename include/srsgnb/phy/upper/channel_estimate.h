
#ifndef SRSGNB_PHY_UPPER_CHANNEL_ESTIMATE_H
#define SRSGNB_PHY_UPPER_CHANNEL_ESTIMATE_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/cyclic_prefix.h"
#include "srsgnb/support/srsran_assert.h"
#include "srsgnb/adt/static_vector.h"

namespace srsgnb {

/// \brief Describes channel estimation results.
class channel_estimate
{
private:
    static constexpr unsigned MAX_SLOT_LEN_RE = MAX_RB * NRE * MAX_NSYMB_PER_SLOT;
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
  static_vector<cf_t, MAX_SLOT_LEN_RE> ce;

  /// Construct given number of RBs used by reference signal.
  explicit channel_estimate(unsigned nof_prb, const cyclic_prefix& cp) noexcept
  {
    size_t slot_len_re = std::min(nof_prb, MAX_RB) * NRE * get_nsymb_per_slot(cp);
    // reserve memory for channel estimates and initialize with 1.0
    ce.resize(slot_len_re, 1.0);
  }

  /// Default destructor
  ~channel_estimate() = default;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_ESTIMATE_H
