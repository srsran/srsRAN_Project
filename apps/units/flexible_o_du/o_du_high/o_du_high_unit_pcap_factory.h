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
#include "o_du_high_unit_config.h"
#include "srsran/pcap/dlt_pcap.h"
#include "srsran/pcap/mac_pcap.h"
#include "srsran/pcap/rlc_pcap.h"
#include "srsran/support/signal_observer.h"

namespace srsran {

struct flexible_o_du_pcaps {
  // DLT PCAPs
  std::unique_ptr<dlt_pcap> f1ap;
  std::unique_ptr<dlt_pcap> f1u;
  std::unique_ptr<dlt_pcap> e2ap;

  std::unique_ptr<signal_observer> f1ap_sig_handler;
  std::unique_ptr<signal_observer> f1u_sig_handler;
  std::unique_ptr<signal_observer> e2ap_sig_handler;

  // MAC and RLC PCAPs
  std::unique_ptr<mac_pcap> mac;
  std::unique_ptr<rlc_pcap> rlc;

  std::unique_ptr<signal_observer> mac_sig_handler;
  std::unique_ptr<signal_observer> rlc_sig_handler;

  /// \brief Destroy (close and flush) the PCAPs.
  void reset()
  {
    f1ap_sig_handler.reset();
    f1u_sig_handler.reset();
    e2ap_sig_handler.reset();
    mac_sig_handler.reset();
    rlc_sig_handler.reset();

    f1ap.reset();
    f1u.reset();
    e2ap.reset();
    mac.reset();
    rlc.reset();
  }
};

/// Creates the PCAPs of the O-RAN DU.
inline flexible_o_du_pcaps create_o_du_pcaps(const o_du_high_unit_config& config,
                                             du_pcap_executor_mapper&     du_execs,
                                             signal_dispatcher&           signal_source)
{
  flexible_o_du_pcaps pcaps;

  const auto& pcap_cfg = config.du_high_cfg.config.pcaps;
  pcaps.f1ap           = pcap_cfg.f1ap.enabled ? create_f1ap_pcap(pcap_cfg.f1ap.filename, du_execs.get_f1ap_executor())
                                               : create_null_dlt_pcap();
  pcaps.f1ap_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.f1ap->flush(); });

  pcaps.f1u             = pcap_cfg.f1u.enabled ? create_gtpu_pcap(pcap_cfg.f1u.filename, du_execs.get_f1u_executor())
                                               : create_null_dlt_pcap();
  pcaps.f1u_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.f1u->flush(); });

  pcaps.e2ap             = config.e2_cfg.pcaps.enabled
                               ? create_e2ap_pcap(config.e2_cfg.pcaps.filename, du_execs.get_e2ap_executor())
                               : create_null_dlt_pcap();
  pcaps.e2ap_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.e2ap->flush(); });
  if (pcap_cfg.mac.type != "dlt" && pcap_cfg.mac.type != "udp") {
    report_error("Invalid type for MAC PCAP. type={}\n", pcap_cfg.mac.type);
  }
  pcaps.mac             = pcap_cfg.mac.enabled
                              ? create_mac_pcap(pcap_cfg.mac.filename,
                                    pcap_cfg.mac.type == "dlt" ? mac_pcap_type::dlt : mac_pcap_type::udp,
                                    du_execs.get_mac_executor())
                              : create_null_mac_pcap();
  pcaps.mac_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.mac->flush(); });

  if (pcap_cfg.rlc.rb_type != "all" && pcap_cfg.rlc.rb_type != "srb" && pcap_cfg.rlc.rb_type != "drb") {
    report_error("Invalid rb_type for RLC PCAP. rb_type={}\n", pcap_cfg.rlc.rb_type);
  }

  pcaps.rlc             = pcap_cfg.rlc.enabled ? create_rlc_pcap(pcap_cfg.rlc.filename,
                                                     du_execs.get_rlc_executor(),
                                                     pcap_cfg.rlc.rb_type != "drb",
                                                     pcap_cfg.rlc.rb_type != "srb")
                                               : create_null_rlc_pcap();
  pcaps.rlc_sig_handler = std::make_unique<signal_observer>(signal_source, [&pcaps]() { pcaps.rlc->flush(); });

  return pcaps;
}

} // namespace srsran
