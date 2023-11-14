/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "pcap_rlc_impl.h"
#include <linux/udp.h>
#include <netinet/in.h>
#include <sys/time.h>

using namespace srsran;

// PCAP tags as defined in Wireshark's "packet-rlc-nr.h"
constexpr const char* PCAP_RLC_NR_START_STRING    = "rlc-nr";
constexpr uint8_t     PCAP_RLC_NR_PAYLOAD_TAG     = 0x01;
constexpr uint8_t     PCAP_RLC_NR_DIRECTION_TAG   = 0x02;
constexpr uint8_t     PCAP_RLC_NR_UEID_TAG        = 0x03;
constexpr uint8_t     PCAP_RLC_NR_BEARER_TYPE_TAG = 0x04;
constexpr uint8_t     PCAP_RLC_NR_BEARER_ID_TAG   = 0x05;

// Other constants
constexpr uint16_t UDP_DLT = 149;

constexpr uint32_t pcap_rlc_max_pdu_len = 131072;

int nr_pcap_pack_rlc_context_to_buffer(const pcap_rlc_pdu_context& context, uint8_t* buffer, unsigned int length);

pcap_rlc_impl::pcap_rlc_impl()
{
  tmp_mem.resize(pcap_rlc_max_pdu_len);
}

pcap_rlc_impl::pcap_rlc_impl(const srsran::os_sched_affinity_bitmask& mask) : cpu_mask(mask)
{
  tmp_mem.resize(pcap_rlc_max_pdu_len);
}

pcap_rlc_impl::~pcap_rlc_impl()
{
  close();
}

void pcap_rlc_impl::open(const std::string& filename_)
{
  uint16_t dlt = UDP_DLT;

  worker = std::make_unique<task_worker>("rlc_pcap", 1024, os_thread_realtime_priority::no_realtime(), cpu_mask);

  // Capture filename_ by copy to prevent it goes out-of-scope when the lambda is executed later
  auto fn = [this, dlt, filename_]() { writter.dlt_pcap_open(dlt, filename_); };
  worker->push_task_blocking(fn);
  is_open.store(true, std::memory_order_relaxed);
}

void pcap_rlc_impl::close()
{
  if (is_open.load(std::memory_order_relaxed)) {
    worker->wait_pending_tasks();
    is_open.store(false, std::memory_order_relaxed); // any further tasks will see it closed
    auto fn = [this]() { writter.dlt_pcap_close(); };
    worker->push_task_blocking(fn);
    worker->wait_pending_tasks(); // make sure dlt_pcap_close is processed
    worker->stop();
  }
}

bool pcap_rlc_impl::is_write_enabled()
{
  return is_open.load(std::memory_order_relaxed);
}

void pcap_rlc_impl::push_pdu(const pcap_rlc_pdu_context& context, const byte_buffer_chain& pdu)
{
  if (!is_write_enabled() || pdu.empty()) {
    return;
  }

  // Filter DRBs if disabled
  if (!drb_enabled && context.bearer_type == PCAP_RLC_BEARER_TYPE_DRB) {
    return;
  }
  // Filter SRBs if disabled
  if (!srb_enabled &&
      (context.bearer_type == PCAP_RLC_BEARER_TYPE_SRB || context.bearer_type == PCAP_RLC_BEARER_TYPE_CCCH)) {
    return;
  }

  byte_buffer buffer = pdu.deep_copy(); // TODO: optimize copy
  auto        fn     = [this, context, buffer = std::move(buffer)]() mutable { write_pdu(context, buffer); };
  if (not worker->push_task(fn)) {
    srslog::fetch_basic_logger("ALL").warning("Dropped RLC PCAP PDU. Cause: worker task is full");
  }
}

void pcap_rlc_impl::push_pdu(const pcap_rlc_pdu_context& context, const byte_buffer_slice& pdu)
{
  if (!is_write_enabled() || pdu.empty()) {
    return;
  }

  // Filter DRBs if disabled
  if (!drb_enabled && context.bearer_type == PCAP_RLC_BEARER_TYPE_DRB) {
    return;
  }
  // Filter SRBs if disabled
  if (!srb_enabled &&
      (context.bearer_type == PCAP_RLC_BEARER_TYPE_SRB || context.bearer_type == PCAP_RLC_BEARER_TYPE_CCCH)) {
    return;
  }

  byte_buffer buffer;
  buffer.append(pdu); // TODO: optimize copy
  auto fn = [this, context, buffer = std::move(buffer)]() mutable { write_pdu(context, buffer); };
  if (not worker->push_task(fn)) {
    srslog::fetch_basic_logger("ALL").warning("Dropped RLC PCAP PDU. Cause: worker task is full");
  }
}

void pcap_rlc_impl::write_pdu(const pcap_rlc_pdu_context& context, const byte_buffer& buf)
{
  if (!is_write_enabled() || buf.empty()) {
    return;
  }

  if (buf.length() > pcap_rlc_max_pdu_len) {
    srslog::fetch_basic_logger("ALL").warning(
        "Dropped RLC PCAP PDU. PDU is too big. pdu_len={} max_size={}", buf.length(), pcap_rlc_max_pdu_len);
    return;
  }

  span<const uint8_t> pdu = to_span(buf, span<uint8_t>(tmp_mem).first(buf.length()));

  uint8_t        context_header[PCAP_CONTEXT_HEADER_MAX] = {};
  const uint16_t length                                  = pdu.size();

  struct udphdr* udp_header;
  int            offset = 0;

  // Add dummy UDP header, start with src and dest port
  udp_header       = (struct udphdr*)context_header;
  udp_header->dest = htons(0xdead);
  offset += 2;
  udp_header->source = htons(0xbeef);
  offset += 2;
  // length to be filled later
  udp_header->len = 0x0000;
  offset += 2;
  // dummy CRC
  udp_header->check = 0x0000;
  offset += 2;

  // Start magic string
  memcpy(&context_header[offset], PCAP_RLC_NR_START_STRING, strlen(PCAP_RLC_NR_START_STRING));
  offset += strlen(PCAP_RLC_NR_START_STRING);

  offset += nr_pcap_pack_rlc_context_to_buffer(context, &context_header[offset], PCAP_CONTEXT_HEADER_MAX);

  udp_header->len = htons(offset + length);

  // Write header
  writter.write_pcap_header(offset + pdu.size());
  // Write context
  writter.write_pcap_pdu(span<uint8_t>(context_header, offset));
  // Write PDU
  writter.write_pcap_pdu(pdu);
}

/// Helper function to serialize RLC NR context
int nr_pcap_pack_rlc_context_to_buffer(const pcap_rlc_pdu_context& context, uint8_t* buffer, unsigned int length)
{
  int      offset = {};
  uint16_t tmp16  = {};

  if (buffer == nullptr || length < PCAP_CONTEXT_HEADER_MAX) {
    printf("Error: Writing buffer null or length to small\n");
    return -1;
  }

  /*****************************************************************/
  /* Context information (same as written by UDP heuristic clients */
  buffer[offset++] = context.rlc_mode;
  buffer[offset++] = context.sequence_number_length;

  /* Direction */
  buffer[offset++] = PCAP_RLC_NR_DIRECTION_TAG;
  buffer[offset++] = context.direction;

  /* UEID */
  buffer[offset++] = PCAP_RLC_NR_UEID_TAG;
  tmp16            = htons(context.ueid);
  memcpy(buffer + offset, &tmp16, 2);
  offset += 2;

  /* Bearer type */
  buffer[offset++] = PCAP_RLC_NR_BEARER_TYPE_TAG;
  buffer[offset++] = context.bearer_type;

  /* Bearer ID */
  buffer[offset++] = PCAP_RLC_NR_BEARER_ID_TAG;
  buffer[offset++] = context.bearer_id;

  /* Data tag immediately preceding PDU */
  buffer[offset++] = PCAP_RLC_NR_PAYLOAD_TAG;
  return offset;
}

void pcap_rlc_impl::capture_drb(bool drb_enabled_)
{
  drb_enabled = drb_enabled_;
}

void pcap_rlc_impl::capture_srb(bool srb_enabled_)
{
  srb_enabled = srb_enabled_;
}
