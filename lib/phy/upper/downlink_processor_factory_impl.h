/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_DOWNLINK_PROCESSOR_FACTORY_H
#define SRSGNB_LIB_PHY_UPPER_DOWNLINK_PROCESSOR_FACTORY_H

#include "srsgnb/phy/upper/downlink_processor_factory.h"

namespace srsgnb {

class upper_phy_rg_gateway;
class pdcch_processor_factory;
class pdsch_processor_factory;
class ssb_processor_factory;
class task_executor;

/// Factory that allows to create downlink processors.
class downlink_processor_factory_impl : public downlink_processor_factory
{
public:
  downlink_processor_factory_impl(upper_phy_rg_gateway&                    gateway,
                                  std::shared_ptr<pdcch_processor_factory> pdcch_proc,
                                  std::shared_ptr<pdsch_processor_factory> pdsch_proc,
                                  std::shared_ptr<ssb_processor_factory>   ssb_proc,
                                  task_executor&                           executor) :
    gateway(gateway), pdcch_proc(pdcch_proc), pdsch_proc(pdsch_proc), ssb_proc(ssb_proc), executor(executor)
  {
  }

  // See interface for documentation.
  std::unique_ptr<downlink_processor> create(const downlink_processor_config& config) override;

private:
  upper_phy_rg_gateway&                    gateway;
  std::shared_ptr<pdcch_processor_factory> pdcch_proc;
  std::shared_ptr<pdsch_processor_factory> pdsch_proc;
  std::shared_ptr<ssb_processor_factory>   ssb_proc;
  task_executor&                           executor;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_DOWNLINK_PROCESSOR_FACTORY_H
