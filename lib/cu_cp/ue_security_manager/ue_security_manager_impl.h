/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
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

#include "srsran/cu_cp/security_manager_config.h"
#include <map>

namespace srsran::srs_cu_cp {

/// UE security manager implementation
class ue_security_manager
{
public:
  ue_security_manager(const security_manager_config& cfg_);
  ~ue_security_manager() = default;

  // up_ue_security_manager
  [[nodiscard]] bool                        is_security_context_initialized() const;
  [[nodiscard]] security::sec_as_config     get_up_as_config() const;
  [[nodiscard]] security::sec_128_as_config get_up_128_as_config() const;

  // ngap_ue_security_manager
  bool               init_security_context(security::security_context sec_ctxt);
  [[nodiscard]] bool is_security_enabled() const;

  // rrc_ue_security_manager
  void                                       enable_security();
  [[nodiscard]] security::security_context   get_security_context() const;
  [[nodiscard]] security::sec_selected_algos get_security_algos() const;
  [[nodiscard]] security::sec_as_config      get_rrc_as_config() const;
  [[nodiscard]] security::sec_128_as_config  get_rrc_128_as_config() const;
  void                                       update_security_context(security::security_context sec_ctxt);
  void perform_horizontal_key_derivation(pci_t target_pci, unsigned target_ssb_arfcn);

private:
  security_manager_config    cfg;
  security::security_context sec_context;
  bool                       security_enabled = false;

  srslog::basic_logger& logger;
};

} // namespace srsran::srs_cu_cp
