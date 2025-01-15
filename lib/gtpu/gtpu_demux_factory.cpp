/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/gtpu/gtpu_demux_factory.h"
#include "gtpu_demux_impl.h"

using namespace srsran;

std::unique_ptr<gtpu_demux> srsran::create_gtpu_demux(const gtpu_demux_creation_request& msg)
{
  report_fatal_error_if_not(msg.gtpu_pcap, "CU-UP pcap is uninitialized");
  return std::make_unique<gtpu_demux_impl>(msg.cfg, *msg.gtpu_pcap);
}
