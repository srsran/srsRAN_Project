/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief PDSCH encoder interface.
///
/// PDSCH encoder black-box interface, with a single method that takes a transport block as input and returns a
/// codeword.

#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_ENCODER_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_ENCODER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/codeblock_metadata.h"

namespace srsgnb {

/// \brief PDSCH encoder, as per TS38.212 Section 7.2.
///
/// For a PHY Downlink Shared Data CHannel (PDSCH) transport block, it returns the single codeword obtained after CRC
/// attachment, segmentation, codeblock encoding, rate matching and, finally, codeblock concatenation.
class pdsch_encoder
{
public:
  /// Default destructor.
  virtual ~pdsch_encoder() = default;

  /// \brief Full PDSCH encoding.
  ///
  /// General encoding method that includes the four steps of CRC attachment (see TS38.212 Section 7.2.1), transport
  /// block segmentation (see TS38.212 Section 7.2.3), codeblock encoding (see TS38.212 Section 7.2.4) and rate matching
  /// (see TS38.212 Section 7.2.5), and codeblock concatenation (see TS38.212 Section 7.2.6) into the PDSCH codeword.
  /// \param[out] codeword         Final PDSCH codeword.
  /// \param[in]  transport_block  Transport block to be transmitted.
  /// \param[in]  cfg              PDSCH configuration parameters.
  virtual void encode(span<uint8_t> codeword, span<const uint8_t> transport_block, const segment_config& cfg) = 0;
};

std::unique_ptr<pdsch_encoder> create_pdsch_encoder();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_ENCODER_H
