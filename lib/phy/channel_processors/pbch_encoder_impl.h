/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#ifndef SRSGNB_CHANNEL_PROCESSORS_PBCH_ENCODER_IMPL_H_
#define SRSGNB_CHANNEL_PROCESSORS_PBCH_ENCODER_IMPL_H_

#include "srsgnb/phy/channel_coding/crc_calculator.h"
#include "srsgnb/phy/channel_processors/pbch_encoder.h"

namespace srsgnb {

class pbch_encoder_impl : public pbch_encoder
{
private:
  std::unique_ptr<pola>           crc24c;
  std::unique_ptr<crc_calculator> crc24c;

public:
  struct args_t {
    std::unique_ptr<crc_calculator> crc24c;
  };

  explicit pbch_encoder_impl(args_t& args);

  ~pbch_encoder_impl() = default;

  void encode(const pbch_msg_t& pbch_msg, std::array<uint8_t, 864>& encoded) override;
};

} // namespace srsgnb

#endif // SRSGNB_CHANNEL_PROCESSORS_PBCH_ENCODER_IMPL_H_
