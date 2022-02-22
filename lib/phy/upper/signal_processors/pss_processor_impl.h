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

#ifndef SRSGNB_SIGNAL_PROCESSORS_PSS_PROCESSOR_IMPL_H_
#define SRSGNB_SIGNAL_PROCESSORS_PSS_PROCESSOR_IMPL_H_

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/upper/signal_processors/pss_processor.h"

namespace srsgnb {
class pss_processor_impl : public pss_processor
{
private:
  static const unsigned SSB_K_BEGIN  = 56;  ///< First subcarrier in the SS/PBCH block
  static const unsigned SSB_L        = 0;   ///< Symbol index in the SSB where the PSS is mapped
  static const unsigned SEQUENCE_LEN = 127; ///< PSS Sequence length in the SSB

  static inline unsigned M(unsigned N_id_2) { return ((43U * (N_id_2)) % SEQUENCE_LEN); }

  struct pregen_signal_s : public std::array<cf_t, SEQUENCE_LEN> {
    pregen_signal_s();
  };

  static const pregen_signal_s signal;

  /**
   * @bried Implements TS 38.211 section 7.4.2.2.1 Sequence generation
   */
  void generation(std::array<cf_t, SEQUENCE_LEN>& sequence, const config_t& args) const;

  /**
   * @bried Implements TS 38.211 section 7.4.2.2.2 Mapping to physical resources
   */
  void mapping(const std::array<cf_t, SEQUENCE_LEN>& sequence, resource_grid_writer& grid, const config_t& args) const;

public:
  ~pss_processor_impl() override = default;
  void map(resource_grid_writer& grid, const config_t& args) override;
};

} // namespace srsgnb

#endif // SRSGNB_SIGNAL_PROCESSORS_PSS_PROCESSOR_IMPL_H_
