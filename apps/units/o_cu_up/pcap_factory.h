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

#include "apps/services/worker_manager/pcap_executor_mapper.h"
#include "apps/units/o_cu_up/o_cu_up_unit_config.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/support/signal_observer.h"

namespace srsran {

struct o_cu_up_dlt_pcaps {
  std::unique_ptr<dlt_pcap> n3;
  std::unique_ptr<dlt_pcap> f1u;
  std::unique_ptr<dlt_pcap> e1ap;
  std::unique_ptr<dlt_pcap> e2ap;

  std::unique_ptr<signal_observer> n3_sig_handler;
  std::unique_ptr<signal_observer> f1u_sig_handler;
  std::unique_ptr<signal_observer> e1ap_sig_handler;
  std::unique_ptr<signal_observer> e2ap_sig_handler;

  /// \brief Close (and flush) the PCAPs without destroying the objects.
  void close()
  {
    if (n3) {
      n3->close();
    }
    if (f1u) {
      f1u->close();
    }
    if (e1ap) {
      e1ap->close();
    }
    if (e2ap) {
      e2ap->close();
    }
  }

  /// \brief Destroy (close and flush) the PCAPs.
  void reset()
  {
    n3_sig_handler.reset();
    f1u_sig_handler.reset();
    e1ap_sig_handler.reset();
    e2ap_sig_handler.reset();

    n3.reset();
    f1u.reset();
    e1ap.reset();
    e2ap.reset();
  }
};

/// Creates the DLT PCAPs of the O-RAN CU-UP.
inline o_cu_up_dlt_pcaps create_o_cu_up_dlt_pcaps(const o_cu_up_unit_config&  unit_cfg,
                                                  cu_up_pcap_executor_mapper& exec_mapper,
                                                  signal_dispatcher&          signal_source)
{
  o_cu_up_dlt_pcaps pcaps;

  const auto& cu_pcaps = unit_cfg.cu_up_cfg.pcap_cfg;
  pcaps.n3             = cu_pcaps.n3.enabled ? create_gtpu_pcap(cu_pcaps.n3.filename, exec_mapper.get_n3_executor())
                                             : create_null_dlt_pcap();
  pcaps.n3_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.n3->flush(); });

  pcaps.f1u             = cu_pcaps.f1u.enabled ? create_gtpu_pcap(cu_pcaps.f1u.filename, exec_mapper.get_f1u_executor())
                                               : create_null_dlt_pcap();
  pcaps.f1u_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.f1u->flush(); });

  pcaps.e1ap = cu_pcaps.e1ap.enabled ? create_e1ap_pcap(cu_pcaps.e1ap.filename, exec_mapper.get_e1ap_executor())
                                     : create_null_dlt_pcap();
  pcaps.e1ap_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.e1ap->flush(); });

  pcaps.e2ap             = unit_cfg.e2_cfg.pcaps.enabled
                               ? create_e2ap_pcap(unit_cfg.e2_cfg.pcaps.filename, exec_mapper.get_e2ap_executor())
                               : create_null_dlt_pcap();
  pcaps.e2ap_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.e2ap->flush(); });

  return pcaps;
}

} // namespace srsran
