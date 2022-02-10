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

#ifndef SRSGNB_SIGNAL_PROCESSORS_SSS_PROCESSOR_IMPL_H_
#define SRSGNB_SIGNAL_PROCESSORS_SSS_PROCESSOR_IMPL_H_

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/signal_processors/sss_processor.h"

namespace srsgnb {
class sss_processor_impl : public sss_processor
{
private:
  static const unsigned SSB_K_BEGIN  = 56;  ///< First subcarrier in the SS/PBCH block
  static const unsigned SSB_L        = 2;   ///< Symbol index in the SSB where the SSS is mapped
  static const unsigned SEQUENCE_LEN = 127; ///< SSS Sequence length in the SSB

  static inline unsigned M(unsigned N_id_2) { return ((43U * (N_id_2)) % SEQUENCE_LEN); }

  struct pregen_d0_s : public std::array<float, SEQUENCE_LEN> {
    pregen_d0_s();
  };

  struct pregen_d1_s : public std::array<float, SEQUENCE_LEN> {
    pregen_d1_s();
  };

  static const pregen_d0_s d0;
  static const pregen_d1_s d1;

  /**
   * @bried Implements TS 38.211 section 7.4.2.2.1 Sequence generation
   */
  void generation(std::array<float, SEQUENCE_LEN>& sequence, const args_t& args) const;

  /**
   * @bried Implements TS 38.211 section 7.4.2.2.2 Mapping to physical resources
   */
  void mapping(const std::array<float, SEQUENCE_LEN>& sequence, resource_grid& grid, const args_t& args) const;

public:
  ~sss_processor_impl() override = default;
  void map(resource_grid& grid, const args_t& args) override;
};

} // namespace srsgnb

#endif // SRSGNB_SIGNAL_PROCESSORS_SSS_PROCESSOR_IMPL_H_
