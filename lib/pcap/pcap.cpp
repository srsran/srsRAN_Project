/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/pcap/pcap.h"
#include "dlt_pcap_impl.h"
#include "mac_pcap_impl.h"

using namespace srsran;

std::unique_ptr<dlt_pcap> srsran::create_dlt_pcap(unsigned           dlt,
                                                  const std::string& layer_name,
                                                  const std::string& filename,
                                                  task_executor&     backend_exec)
{
  if (filename.empty()) {
    return std::make_unique<dummy_dlt_pcap>();
  }
  return std::make_unique<dlt_pcap_impl>(dlt, layer_name, filename, backend_exec);
}

std::unique_ptr<mac_pcap>
srsran::create_mac_pcap(const std::string& filename, mac_pcap_type pcap_type, task_executor& backend_exec)
{
  if (filename.empty()) {
    return std::make_unique<dummy_mac_pcap>();
  }
  return std::make_unique<mac_pcap_impl>(filename, pcap_type, backend_exec);
}
