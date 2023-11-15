/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dlt_pcap_impl.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/support/executors/sync_task_executor.h"

using namespace srsran;

dlt_pcap_impl::dlt_pcap_impl(uint32_t           dlt,
                             const std::string& layer_name_,
                             const std::string& filename,
                             task_executor&     backend_exec_) :
  layer_name(layer_name_), logger(srslog::fetch_basic_logger("ALL")), writer(dlt, layer_name_, filename, backend_exec_)
{
}

dlt_pcap_impl::~dlt_pcap_impl()
{
  close();
}

void dlt_pcap_impl::close()
{
  writer.close();
}

void dlt_pcap_impl::push_pdu(const_span<uint8_t> pdu)
{
  push_pdu(byte_buffer{pdu});
}

void dlt_pcap_impl::push_pdu(byte_buffer pdu)
{
  if (pdu.empty()) {
    // skip.
    return;
  }

  writer.dispatch([this, pdu = std::move(pdu)](pcap_file_writer& pcap_file) { write_pdu(pdu, pcap_file); });
  if (is_write_enabled()) {
    logger.warning("Dropped {} PCAP PDU. Cause: The pcap file is closed", layer_name);
  }
}

void dlt_pcap_impl::write_pdu(const byte_buffer& pdu, pcap_file_writer& pcap_file)
{
  // write packet header
  unsigned length = pdu.length();
  pcap_file.write_pdu_header(length);

  // write PDU payload
  pcap_file.write_pdu(pdu);
}

std::unique_ptr<dlt_pcap> srsran::create_dlt_pcap(unsigned           dlt,
                                                  const std::string& layer_name,
                                                  const std::string& filename,
                                                  task_executor*     backend_exec)
{
  if (filename.empty()) {
    return std::make_unique<null_dlt_pcap>();
  }
  srsran_assert(backend_exec != nullptr, "Backend executor must not be null");
  return std::make_unique<dlt_pcap_impl>(dlt, layer_name, filename, *backend_exec);
}
