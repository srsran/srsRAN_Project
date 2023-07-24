/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/ran/pdcch/search_space.h"

namespace srsran {
namespace search_space_helper {

inline bool search_space_supports_dl_dci_format(const search_space_configuration& ss_cfg, dci_dl_format dci_fmt)
{
  if (ss_cfg.is_common_search_space()) {
    const auto& mon_dci_fmt =
        variant_get<search_space_configuration::common_dci_format>(ss_cfg.get_monitored_dci_formats());
    switch (dci_fmt) {
      case dci_dl_format::f1_0:
        return mon_dci_fmt.f0_0_and_f1_0;
      case dci_dl_format::f2_0:
        return mon_dci_fmt.f2_0;
      default:
        srsran_assertion_failure("DCI format {} not supported for common SearchSpace", dci_fmt);
    }
  } else {
    const auto& mon_dci_fmt =
        variant_get<search_space_configuration::ue_specific_dci_format>(ss_cfg.get_monitored_dci_formats());
    switch (dci_fmt) {
      case dci_dl_format::f1_0:
        return mon_dci_fmt == search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0;
      case dci_dl_format::f1_1:
        return mon_dci_fmt == search_space_configuration::ue_specific_dci_format::f0_1_and_1_1;
      default:
        srsran_assertion_failure("DCI format {} not supported for UE-dedicated SearchSpace", dci_fmt);
    }
  }
  return false;
}

/// \brief Gets DL DCI format type to use based on SearchSpace configuration.
/// \param[in] ss_cfg SearchSpace Configuration.
/// \return DL DCI format.
inline dci_dl_format get_dl_dci_format(const search_space_configuration& ss_cfg)
{
  if (ss_cfg.is_common_search_space()) {
    const auto common_dci_fmt =
        variant_get<search_space_configuration::common_dci_format>(ss_cfg.get_monitored_dci_formats());
    if (common_dci_fmt.f0_0_and_f1_0) {
      return dci_dl_format::f1_0;
    }
    // TODO: Handle DCI Formats 2_0, 2_1, 2_2, 2_3 under Common SearchSpace.
    report_fatal_error("Unsupported DL DCI format");
  }
  const auto uss_dci_fmt =
      variant_get<search_space_configuration::ue_specific_dci_format>(ss_cfg.get_monitored_dci_formats());
  switch (uss_dci_fmt) {
    case search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0:
      return dci_dl_format::f1_0;
    case search_space_configuration::ue_specific_dci_format::f0_1_and_1_1:
      return dci_dl_format::f1_1;
  }
  report_fatal_error("Unsupported DL DCI format");
}

/// \brief Gets UL DCI format type to use based on SearchSpace configuration.
/// \param[in] ss_cfg SearchSpace Configuration.
/// \return UL DCI format.
inline dci_ul_format get_ul_dci_format(const search_space_configuration& ss_cfg)
{
  if (ss_cfg.is_common_search_space()) {
    const auto common_dci_fmt =
        variant_get<search_space_configuration::common_dci_format>(ss_cfg.get_monitored_dci_formats());
    if (common_dci_fmt.f0_0_and_f1_0) {
      return dci_ul_format::f0_0;
    }
    // TODO: Handle DCI Formats 2_0, 2_1, 2_2, 2_3 under Common SearchSpace.
    report_fatal_error("Unsupported UL DCI format");
  }
  const auto uss_dci_fmt =
      variant_get<search_space_configuration::ue_specific_dci_format>(ss_cfg.get_monitored_dci_formats());
  switch (uss_dci_fmt) {
    case search_space_configuration::ue_specific_dci_format::f0_0_and_f1_0:
      return dci_ul_format::f0_0;
    case search_space_configuration::ue_specific_dci_format::f0_1_and_1_1:
      return dci_ul_format::f0_1;
  }
  report_fatal_error("Unsupported UL DCI format");
}

} // namespace search_space_helper
} // namespace srsran