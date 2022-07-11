/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_PHY_UPPER_UPPER_PHY_FACTORY_H
#define SRSGNB_PHY_UPPER_UPPER_PHY_FACTORY_H

#include "srsgnb/phy/upper/upper_phy.h"
#include <memory>

namespace srsgnb {

class upper_phy_rg_gateway;
class task_executor;

struct upper_phy_config {
  unsigned              sector_id;
  unsigned              nof_ports;
  unsigned              nof_slots;
  unsigned              nof_dl_processors;
  unsigned              bw_rb;
  upper_phy_rg_gateway* gateway;
  task_executor*        executor;
};

/// \brief Factory that builds upper PHY objects.
class upper_phy_factory
{
public:
  virtual ~upper_phy_factory() = default;

  /// \brief Creates and returns an upper PHY object.
  virtual std::unique_ptr<upper_phy> create(const upper_phy_config& config) = 0;
};

/// Creates and returns an upper PHY factory.
std::unique_ptr<upper_phy_factory> create_upper_phy_factory();

} // namespace srsgnb

#endif // #define SRSGNB_PHY_UPPER_UPPER_PHY_FACTORY_H
