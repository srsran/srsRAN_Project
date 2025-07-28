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
