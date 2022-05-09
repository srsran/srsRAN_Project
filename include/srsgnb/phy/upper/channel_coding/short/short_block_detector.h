/// \file
/// \brief Detector interface for short blocks.

#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_DETECTOR_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_DETECTOR_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/modulation_scheme.h"

#include <memory>

namespace srsgnb {

/// Short-block detector interface.
class short_block_detector
{
public:
  /// Default destructor.
  virtual ~short_block_detector() = default;

  /// \brief Detection, decoding and rate-dematching of short blocks.
  ///
  /// Checks whether the provided signal contains a codeblock and, if so, decodes its content assuming the encoding
  /// followed TS38.212 Section 5.3.3 and the rate-matching scheme of TS38.212 Section 5.4.3.
  /// \param[out] output Detected message (one bit per entry).
  /// \param[in]  input  Received codeblock (log-likelihood ratios).
  /// \param[in]  mod    Modulation scheme.
  /// \return True if \c input passes a detection test.
  /// \remark When the detection test fails, the function returns \c false and the content of \c output is unreliable.
  virtual bool detect(span<uint8_t> output, span<const int8_t> input, modulation_scheme mod) = 0;
};

std::unique_ptr<short_block_detector> create_short_block_detector();
} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_DETECTOR_H
