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

#include "dlt_pcap_impl.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/support/executors/sync_task_executor.h"

using namespace srsran;

// DLT PCAP values for different layers
constexpr uint16_t PCAP_NGAP_DLT = 152;
constexpr uint16_t PCAP_E1AP_DLT = 153;
constexpr uint16_t PCAP_F1AP_DLT = 154;
constexpr uint16_t PCAP_E2AP_DLT = 155;
constexpr uint16_t PCAP_GTPU_DLT = 156;

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
  if (not is_write_enabled() or pdu.empty()) {
    // skip.
    return;
  }
  writer.write_pdu(byte_buffer(pdu));
}

void dlt_pcap_impl::push_pdu(byte_buffer pdu)
{
  if (not is_write_enabled() or pdu.empty()) {
    // skip.
    return;
  }
  writer.write_pdu(std::move(pdu));
}

static std::unique_ptr<dlt_pcap>
create_dlt_pcap(unsigned dlt, const std::string& layer_name, const std::string& filename, task_executor& backend_exec)
{
  srsran_assert(not filename.empty(), "File name is empty");
  return std::make_unique<dlt_pcap_impl>(dlt, layer_name, filename, backend_exec);
}

std::unique_ptr<dlt_pcap> srsran::create_null_dlt_pcap()
{
  return std::make_unique<null_dlt_pcap>();
}

std::unique_ptr<dlt_pcap> srsran::create_ngap_pcap(const std::string& filename, task_executor& backend_exec)
{
  return create_dlt_pcap(PCAP_NGAP_DLT, "NGAP", filename, backend_exec);
}

std::unique_ptr<dlt_pcap> srsran::create_f1ap_pcap(const std::string& filename, task_executor& backend_exec)
{
  return create_dlt_pcap(PCAP_F1AP_DLT, "F1AP", filename, backend_exec);
}

std::unique_ptr<dlt_pcap> srsran::create_e1ap_pcap(const std::string& filename, task_executor& backend_exec)
{
  return create_dlt_pcap(PCAP_E1AP_DLT, "E1AP", filename, backend_exec);
}

std::unique_ptr<dlt_pcap> srsran::create_gtpu_pcap(const std::string& filename, task_executor& backend_exec)
{
  return create_dlt_pcap(PCAP_GTPU_DLT, "GTPU", filename, backend_exec);
}

std::unique_ptr<dlt_pcap> srsran::create_e2ap_pcap(const std::string& filename, task_executor& backend_exec)
{
  return create_dlt_pcap(PCAP_E2AP_DLT, "E2AP", filename, backend_exec);
}
