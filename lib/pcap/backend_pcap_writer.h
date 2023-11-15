/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "pcap_file_writer.h"
#include "srsran/adt/byte_buffer.h"
#include "srsran/adt/unique_function.h"

namespace srsran {

class task_executor;

/// \brief This class provides a thread-safe interface to write to the pcap file, by only allowing accesses to the
/// file via a single executor.
class backend_pcap_writer
{
public:
  backend_pcap_writer(unsigned           dlt_,
                      const std::string& layer_name_,
                      const std::string& filename,
                      task_executor&     backend_exec_);
  ~backend_pcap_writer();
  backend_pcap_writer(const backend_pcap_writer& other)            = delete;
  backend_pcap_writer& operator=(const backend_pcap_writer& other) = delete;
  backend_pcap_writer(backend_pcap_writer&& other)                 = delete;
  backend_pcap_writer& operator=(backend_pcap_writer&& other)      = delete;

  void close();
  bool is_write_enabled() const { return is_open.load(std::memory_order_relaxed); }

  template <typename WriteFunc>
  void dispatch(WriteFunc&& write_func)
  {
    unique_task t = [this, write_func = std::forward<WriteFunc>(write_func)]() mutable {
      if (!is_write_enabled()) {
        logger.warning("Dropped {} PCAP PDU. Cause: The pcap file is closed", layer_name);
        return;
      }
      write_func(writer);
    };
    dispatch_impl(std::move(t));
  }

private:
  void dispatch_impl(unique_task t);

  std::string           layer_name;
  task_executor&        backend_exec;
  srslog::basic_logger& logger;

  pcap_file_writer  writer;
  std::atomic<bool> is_open{true};

  std::mutex close_mutex;
};

} // namespace srsran