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

#include "srsran/support/executors/task_executor.h"

namespace srsran {

/// Mapper of PCAP executors for the DU.
class du_pcap_executor_mapper
{
public:
  virtual ~du_pcap_executor_mapper() = default;

  /// Retrieves the executor for F1AP PCAP writing.
  virtual task_executor& get_f1ap_executor() = 0;

  /// Retrieves the executor for E2AP PCAP writing.
  virtual task_executor& get_e2ap_executor() = 0;

  /// Retrieves the executor for F1U PCAP writing.
  virtual task_executor& get_f1u_executor() = 0;

  /// Retrieves the executor for MAC PCAP writing.
  virtual task_executor& get_mac_executor() = 0;

  /// Retrieves the executor for RLC PCAP writing.
  virtual task_executor& get_rlc_executor() = 0;
};

/// Mapper of PCAP executors for the CU-CP.
class cu_cp_pcap_executor_mapper
{
public:
  virtual ~cu_cp_pcap_executor_mapper() = default;

  /// Retrieves the executor for F1AP PCAP writing.
  virtual task_executor& get_f1ap_executor() = 0;

  /// Retrieves the executor for NGAP PCAP writing.
  virtual task_executor& get_ngap_executor() = 0;

  /// Retrieves the executor for E2AP PCAP writing.
  virtual task_executor& get_e2ap_executor() = 0;

  /// Retrieves the executor for E1AP PCAP writing.
  virtual task_executor& get_e1ap_executor() = 0;
};

/// Mapper of PCAP executors for the CU-UP.
class cu_up_pcap_executor_mapper
{
public:
  virtual ~cu_up_pcap_executor_mapper() = default;

  /// Retrieves the executor for E2AP PCAP writing.
  virtual task_executor& get_e2ap_executor() = 0;

  /// Retrieves the executor for F1U PCAP writing.
  virtual task_executor& get_f1u_executor() = 0;

  /// Retrieves the executor for E1AP PCAP writing.
  virtual task_executor& get_e1ap_executor() = 0;

  /// Retrieves the executor for N3 PCAP writing.
  virtual task_executor& get_n3_executor() = 0;
};

} // namespace srsran
