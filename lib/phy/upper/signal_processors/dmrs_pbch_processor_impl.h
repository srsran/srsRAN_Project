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

#ifndef SRSGNB_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_IMPL_H_
#define SRSGNB_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_IMPL_H_

#include "srsgnb/adt/complex.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/upper/sequence_generators/pseudo_random_generator.h"
#include "srsgnb/phy/upper/signal_processors/dmrs_pbch_processor.h"

namespace srsgnb {
class dmrs_pbch_processor_impl : public dmrs_pbch_processor
{
private:
  std::unique_ptr<pseudo_random_generator> prg = create_pseudo_random();

  static unsigned c_init(const config_t& args);

  /**
   * @bried Implements TS 38.211 section 7.4.1.4.1 Sequence generation
   */
  void generation(std::array<cf_t, NOF_RE>& sequence, const config_t& args) const;

  /**
   * @bried Implements TS 38.211 section 7.4.1.4.2 Mapping to physical resources
   */
  void mapping(const std::array<cf_t, NOF_RE>& sequence, resource_grid_writer& grid, const config_t& args) const;

public:
  ~dmrs_pbch_processor_impl() override = default;
  void map(resource_grid_writer& grid, const config_t& args) override;
};

} // namespace srsgnb

#endif // SRSGNB_SIGNAL_PROCESSORS_DMRS_PBCH_PROCESSOR_IMPL_H_
