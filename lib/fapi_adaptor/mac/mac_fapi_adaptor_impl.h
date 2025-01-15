/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/fapi_adaptor/mac/mac_fapi_adaptor.h"
#include "srsran/fapi_adaptor/mac/mac_fapi_sector_adaptor.h"
#include <memory>
#include <vector>

namespace srsran {
namespace fapi_adaptor {

/// MAC/FAPI adaptor implementation.
class mac_fapi_adaptor_impl : public mac_fapi_adaptor
{
public:
  explicit mac_fapi_adaptor_impl(std::vector<std::unique_ptr<mac_fapi_sector_adaptor>> sector_adaptors_);

  // See interface for documentation.
  mac_fapi_sector_adaptor& get_sector_adaptor(unsigned cell_id) override;

private:
  std::vector<std::unique_ptr<mac_fapi_sector_adaptor>> sector_adaptors;
};

} // namespace fapi_adaptor
} // namespace srsran
