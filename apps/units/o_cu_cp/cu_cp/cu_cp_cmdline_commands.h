/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "apps/services/cmdline/cmdline_command.h"
#include "apps/services/cmdline/cmdline_command_dispatcher_utils.h"
#include "srsran/adt/expected.h"
#include "srsran/cu_cp/cu_cp_command_handler.h"
#include "srsran/ran/pci.h"
#include "srsran/ran/rnti.h"

namespace srsran {

/// Application command to trigger a handover.
class handover_app_command : public app_services::cmdline_command
{
  srs_cu_cp::cu_cp_command_handler& cu_cp;

public:
  explicit handover_app_command(srs_cu_cp::cu_cp_command_handler& cu_cp_) : cu_cp(cu_cp_) {}

  // See interface for documentation.
  std::string_view get_name() const override { return "ho"; }

  // See interface for documentation.
  std::string_view get_description() const override { return " <serving pci> <rnti> <target pci>: force UE handover"; }

  // See interface for documentation.
  void execute(span<const std::string> args) override
  {
    if (args.size() != 3) {
      fmt::print("Invalid handover command structure. Usage: ho <serving pci> <rnti> <target pci>\n");
      return;
    }

    const auto*                     arg         = args.begin();
    expected<unsigned, std::string> serving_pci = app_services::parse_int<unsigned>(*arg);
    if (not serving_pci.has_value()) {
      fmt::print("Invalid serving PCI.\n");
      return;
    }
    ++arg;
    expected<unsigned, std::string> rnti = app_services::parse_unsigned_hex<unsigned>(*arg);
    if (not rnti.has_value()) {
      fmt::print("Invalid UE RNTI.\n");
      return;
    }
    ++arg;
    expected<unsigned, std::string> target_pci = app_services::parse_int<unsigned>(*arg);
    if (not target_pci.has_value()) {
      fmt::print("Invalid target PCI.\n");
      return;
    }

    cu_cp.get_mobility_command_handler().trigger_handover(static_cast<pci_t>(serving_pci.value()),
                                                          static_cast<rnti_t>(rnti.value()),
                                                          static_cast<pci_t>(target_pci.value()));
    fmt::print("Handover triggered for UE with pci={} rnti={} to pci={}.\n",
               serving_pci.value(),
               static_cast<rnti_t>(rnti.value()),
               target_pci.value());
  }
};

} // namespace srsran
