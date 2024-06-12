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

#include "srsran/security/security.h"

namespace srsran {
namespace srs_cu_cp {

struct ue_security_manager_config {
  security::preferred_integrity_algorithms int_algo_pref_list; ///< Integrity protection algorithms preference list
  security::preferred_ciphering_algorithms enc_algo_pref_list; ///< Encryption algorithms preference list
};

class up_ue_security_manager
{
public:
  virtual ~up_ue_security_manager() = default;

  /// \brief Get the AS configuration for the UP domain
  virtual security::sec_as_config get_up_as_config() = 0;

  /// \brief Get the AS configuration for the UP domain with 128-bit keys
  virtual security::sec_128_as_config get_up_128_as_config() = 0;
};

class ngap_ue_security_manager
{
public:
  virtual ~ngap_ue_security_manager() = default;

  /// \brief Initialize security context by selecting security algorithms and generating K_rrc_enc and K_rrc_int
  /// \param[in] sec_ctxt The received security context
  /// \return True if the security context was successfully initialized, false otherwise
  virtual bool init_security_context(security::security_context sec_ctxt) = 0;

  /// \brief Check if security is enabled
  [[nodiscard]] virtual bool is_security_enabled() const = 0;
};

class rrc_ue_security_manager
{
public:
  virtual ~rrc_ue_security_manager() = default;

  /// \brief Get the AS configuration for the RRC domain
  virtual security::sec_as_config get_rrc_as_config() = 0;

  /// \brief Get the AS configuration for the RRC domain with 128-bit keys
  virtual security::sec_128_as_config get_rrc_128_as_config() = 0;

  /// \brief Enable security
  virtual void enable_security() = 0;

  /// \brief Get the current security context
  virtual security::security_context get_security_context() = 0;

  /// \brief Get the selected security algorithms
  virtual security::sec_selected_algos get_security_algos() = 0;

  /// \brief Update the security context
  /// \param[in] sec_ctxt The new security context
  virtual void update_security_context(security::security_context sec_ctxt) = 0;

  /// \brief Perform horizontal key derivation
  virtual void horizontal_key_derivation(pci_t target_pci, unsigned target_ssb_arfcn) = 0;
};

} // namespace srs_cu_cp
} // namespace srsran
