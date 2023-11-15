/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "backend_pcap_writer.h"
#include "srsran/support/executors/sync_task_executor.h"

using namespace srsran;

backend_pcap_writer::backend_pcap_writer(uint32_t           dlt,
                                         const std::string& layer_name_,
                                         const std::string& filename,
                                         task_executor&     backend_exec_) :
  layer_name(layer_name_), backend_exec(backend_exec_), logger(srslog::fetch_basic_logger("ALL"))
{
  writer.dlt_pcap_open(dlt, filename);
}

backend_pcap_writer::~backend_pcap_writer()
{
  close();
}

void backend_pcap_writer::close()
{
  std::lock_guard<std::mutex> lock(close_mutex);
  if (is_open.load(std::memory_order_relaxed)) {
    // The pcap writing is still enabled. Dispatch closing of the pcap writer to backend executor.

    // Note: We use a sync executor to ensure the pcap file is closed before we return.
    auto sync_exec = make_sync_executor(&backend_exec);

    // Note: We need to handle the case where we fail to dispatch the pcap closing task because the task worker
    // queue is full.
    const unsigned nof_dispatch_attempts = 20;
    unsigned       count                 = 0;
    auto           close_task            = [this]() {
      writer.dlt_pcap_close();
      is_open = false;
    };
    for (; count < nof_dispatch_attempts and not sync_exec->execute(close_task); ++count) {
      // give some time for task executor queue to be flushed.
      std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    // make sure dlt_pcap_close is called.
    if (count == nof_dispatch_attempts) {
      logger.warning("Failed to dispatch task to close {} pcap file. Forcing close...", layer_name);
      close_task();
    }
  }
}
void backend_pcap_writer::dispatch_impl(unique_task t)
{
  if (not is_write_enabled()) {
    logger.warning("Dropped {} PCAP PDU. Cause: The pcap file is closed", layer_name);
  }
  if (not backend_exec.defer(std::move(t))) {
    logger.warning("Dropped {} PCAP PDU. Cause: Task executor queue is full", layer_name);
  }
}
