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

#include "srsgnb/phy/signal_processors/pss_processor.h"

namespace srsgnb {
class pss_processor_impl : public pss_processor
{
public:
  void map(resource_grid& grid, const ssb_config& ssb_config) override;
};

} // namespace srsgnb

#endif // SRSGNB_SIGNAL_PROCESSORS_PSS_PROCESSOR_IMPL_H_
