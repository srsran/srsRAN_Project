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
  security::sec_as_config     get_up_as_config();
  security::sec_128_as_config get_up_128_as_config();

  // ngap_ue_security_manager
  bool               init_security_context(security::security_context sec_ctxt);
  [[nodiscard]] bool is_security_enabled() const;

  // rrc_ue_security_manager
  void                         enable_security();
  security::security_context   get_security_context();
  security::sec_selected_algos get_security_algos();
  security::sec_as_config      get_rrc_as_config();
  security::sec_128_as_config  get_rrc_128_as_config();
  void                         update_security_context(security::security_context sec_ctxt);
  void                         horizontal_key_derivation(pci_t target_pci, unsigned target_ssb_arfcn);

private:
  security_manager_config    cfg;
  security::security_context sec_context;
  bool                       security_enabled = false;

  srslog::basic_logger& logger;
};

} // namespace srsran::srs_cu_cp
