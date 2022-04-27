/// \file
/// \brief Detector interface for short blocks.

#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_DETECTOR_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_DETECTOR_H

#include "srsgnb/adt/span.h"

#include <memory>

namespace srsgnb {

/// Short-block detector interface.
class short_block_detector
{
public:
  /// Default destructor.
  virtual ~short_block_detector() = default;

  /// \brief Detection and decoding of short blocks.
  ///
  /// Checks whether the provided signal contains a codeblock and, if so, decodes its content assuming the encoding
  /// followed TS38.212 Section 5.3.3.
  /// \param[out] output Detected message.
  /// \param[in]  input  Received codeblock (log-likelihood ratios).
  /// \return True if \c input passes a detection test. If false, the content of \c output is unreliable.
  virtual bool detect(span<uint8_t> output, span<const int8_t> input) = 0;

  /// \brief Rate dematching for short blocks.
  ///
  /// Recovers the original short codeblock from its rate-matched version. It reverts the operations described in
  /// TS38.212 Section 5.4.3.
  /// \param[out] output Original codeblock (log-likelihood ratios).
  /// \param[in]  input  Rate-matched codeblock (log-likelihood ratios).
  virtual void rate_dematch(span<int8_t> output, span<const int8_t> input) = 0;
};

std::unique_ptr<short_block_detector> create_short_block_detector();
} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_SHORT_SHORT_BLOCK_DETECTOR_H
