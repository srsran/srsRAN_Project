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

#ifndef SRSGNB_CHANNEL_PROCESSORS_PDCCH_ENCODER_IMPL_H_
#define SRSGNB_CHANNEL_PROCESSORS_PDCCH_ENCODER_IMPL_H_

#include "srsgnb/phy/upper/channel_coding/crc_calculator.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_allocator.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_code.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_encoder.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_interleaver.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_rate_matcher.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_constants.h"
#include "srsgnb/phy/upper/channel_processors/pdcch_encoder.h"

namespace srsgnb {

/// \brief PDCCH encoder, as per TS38.212 Section 7.3.
///
/// For the PHY Downlink Control CHannel (PDCCH) it carries out CRC attachment,
/// Polar encoding and rate matching.
class pdcch_encoder_impl : public pdcch_encoder
{
private:
  std::unique_ptr<crc_calculator>     crc24c;
  std::unique_ptr<polar_interleaver>  interleaver;
  std::unique_ptr<polar_allocator>    alloc;
  std::unique_ptr<polar_code>         code;
  std::unique_ptr<polar_encoder>      encoder;
  std::unique_ptr<polar_rate_matcher> rm;

  /// \brief CRC attachment
  ///
  /// \param[out] c     Payload with CRC attached
  /// \param[in]  a     Payload
  /// \param[in]  rnti  RNTI used for CRC bits scrambling
  ///
  /// \remark CRC attachment follows TS 38.312 section 7.3.2
  void crc_attach(span<uint8_t>& c, span<const uint8_t> a, unsigned rnti);

  /// \brief Payload encoding
  ///
  /// \param[out] d Data after encoding
  /// \param[in]  c Payload after CRC attachment
  ///
  /// \remark Channel codding follows TS 38.312 section 7.3.3
  void channel_coding(span<uint8_t> d, span<const uint8_t> c);

  /// \brief Rate matching of the encoded data
  ///
  /// \param[out] f Data after rate matching
  /// \param[in]  d Data after encoding
  ///
  /// \remark Rate matching follows TS 38.312 section 7.3.4
  void rate_matching(span<uint8_t> f, span<const uint8_t> d);

public:
  explicit pdcch_encoder_impl();

  /// Default destructor
  ~pdcch_encoder_impl() = default;

  // See interface for the documentation.
  virtual void encode(span<uint8_t> encoded, span<const uint8_t> data, const config_t& config) override;
};

} // namespace srsgnb

#endif // SRSGNB_CHANNEL_PROCESSORS_PDCCH_ENCODER_IMPL_H_
