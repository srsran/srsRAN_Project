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

#include "srsran/fapi_adaptor/mac/p7/mac_fapi_p7_sector_fastpath_adaptor.h"
#include <memory>

namespace srsran {
namespace fapi_adaptor {

struct mac_fapi_p7_sector_fastpath_adaptor_dependencies;
struct mac_fapi_p7_sector_fastpath_adaptor_config;

/// Creates a MAC-FAPI P7 sector fastpath adaptor.
std::unique_ptr<mac_fapi_p7_sector_fastpath_adaptor>
create_mac_fapi_p7_sector_fastpath_adaptor(const mac_fapi_p7_sector_fastpath_adaptor_config& config,
                                           mac_fapi_p7_sector_fastpath_adaptor_dependencies  dependencies);

} // namespace fapi_adaptor
} // namespace srsran
