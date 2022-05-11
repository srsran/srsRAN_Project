/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_ENCODER_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_ENCODER_H

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_constants.h"
#include <memory>

namespace srsgnb {

/// Describes the PDCCH encoder interface.
class pdcch_encoder
{
public:
  /// Describes the necessary parameters to encode PDCCH message.
  struct config_t {
    /// Number of rate-matched bits.
    unsigned E;
    /// RNTI used for CRC bits scrambling according to TS 38.212 section 7.3.2.
    unsigned rnti;
  };

  /// Default destructor.
  virtual ~pdcch_encoder() = default;

  /// \brief Encodes a PDCCH message.
  /// \param [in]  data    - unencoded message bits
  /// \param [out] encoded - encoded bits
  /// \param [in]  config  - encoder configuration
  ///
  /// \remark size of + 24 bits of CRC gives K parameter for the Polar encoder

  virtual void encode(span<uint8_t> encoded, span<const uint8_t> data, const config_t& config) = 0;
};

/// Creates a PDCCH encoder instance.
std::unique_ptr<pdcch_encoder> create_pdcch_encoder();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_ENCODER_H
