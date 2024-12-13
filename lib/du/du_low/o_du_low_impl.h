/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/du/du_low/du_low.h"
#include "srsran/du/du_low/o_du_low.h"
#include "srsran/fapi_adaptor/phy/phy_fapi_adaptor.h"
#include <memory>

namespace srsran {
namespace srs_du {

class o_du_low_impl final : public o_du_low
{
public:
  o_du_low_impl(std::unique_ptr<du_low>                         du_lo_,
                std::unique_ptr<fapi_adaptor::phy_fapi_adaptor> fapi_adaptor_,
                unsigned                                        nof_cells);

  // See interface for documentation.
  du_low& get_du_low() override;

  // See interface for documentation.
  fapi_adaptor::phy_fapi_adaptor& get_phy_fapi_adaptor() override;

private:
  std::unique_ptr<du_low>                         du_lo;
  std::unique_ptr<fapi_adaptor::phy_fapi_adaptor> fapi_adaptor;
};

} // namespace srs_du
} // namespace srsran
