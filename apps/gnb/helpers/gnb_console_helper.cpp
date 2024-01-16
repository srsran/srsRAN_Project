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

#include "gnb_console_helper.h"
#include "string_helpers.h"
#include "srsran/ran/band_helper.h"
#include "srsran/ran/bs_channel_bandwidth.h"
#include "srsran/support/build_info/build_info.h"
#include "srsran/support/io/io_broker.h"
#include <fcntl.h>
#include <list>
#include <signal.h>
#include <unistd.h>

using namespace srsran;

gnb_console_helper::gnb_console_helper(io_broker&           io_broker_,
                                       srslog::log_channel& log_chan_,
                                       bool                 autostart_stdout_metrics_) :
  logger(srslog::fetch_basic_logger("GNB")),
  io_broker_handle(io_broker_),
  metrics_json(log_chan_),
  autostart_stdout_metrics(autostart_stdout_metrics_)
{
  // set STDIN file descripter into non-blocking mode
  int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
  if (fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) == -1) {
    logger.error("Couldn't configure fd to non-blocking");
  }

  if (io_broker_handle.register_fd(STDIN_FILENO, [this](int fd) { stdin_handler(fd); }) == false) {
    logger.error("Couldn't register stdin handler");
  }
}

gnb_console_helper::~gnb_console_helper()
{
  io_broker_handle.unregister_fd(STDIN_FILENO);
}

void gnb_console_helper::stdin_handler(int fd)
{
  static const unsigned               read_chunk = 256;
  std::array<uint8_t, 4 * read_chunk> buffer;
  int                                 bytes_read       = 0;
  int                                 total_bytes_read = 0;

  logger.debug("Stdin has data to read");

  do {
    // read from stdin until EWOULDBLOCK is set
    bytes_read = read(fd, &buffer[total_bytes_read], read_chunk);
    if (bytes_read < 0) {
      if (errno == EWOULDBLOCK || errno == EAGAIN || errno == EINTR) {
        break;
      }
    } else if (bytes_read == 0) {
      logger.debug("Connection closed");
      return;
    } else {
      total_bytes_read += bytes_read;
      if (total_bytes_read + read_chunk > buffer.size()) {
        logger.error("Can't read more than {} B from stdin", buffer.size());
        return;
      }
    }
  } while (true);

  logger.debug("read {} B from stdin", total_bytes_read);

  // convert buffer to string
  std::string input_line(buffer.begin(), buffer.begin() + total_bytes_read);

  std::list<std::string> cmd_list;
  srsran::string_parse_list(input_line, ';', cmd_list);
  for (auto& cmd : cmd_list) {
    cmd.erase(std::remove(cmd.begin(), cmd.end(), '\n'), cmd.cend());
    handle_command(cmd);
  }
}

void gnb_console_helper::handle_command(const std::string& command)
{
  if (command == "q") {
    raise(SIGTERM);
  } else if (command == "t") {
    metrics_plotter.toggle_print();
  } else {
    print_help();
  }
}

void gnb_console_helper::print_help()
{
  fmt::print("Available commands:\n");
  fmt::print("\tt: start/stop console trace\n");
  fmt::print("\tq: quit application\n");
  fmt::print("\n");
}

void gnb_console_helper::set_cells(const span<du_cell_config>& cells_)
{
  cells = {cells_.begin(), cells_.end()};
}

void gnb_console_helper::on_app_starting()
{
  fmt::print("\n--== srsRAN gNB (commit {}) ==--\n\n", get_build_hash());
}

void gnb_console_helper::on_app_running()
{
  for (const auto& cell : cells) {
    fmt::print("Cell pci={}, bw={} MHz, dl_arfcn={} (n{}), dl_freq={} MHz, dl_ssb_arfcn={}, ul_freq={} MHz\n",
               cell.pci,
               cell.dl_carrier.carrier_bw_mhz,
               cell.dl_carrier.arfcn,
               srsran::nr_band_to_uint(cell.dl_carrier.band),
               srsran::band_helper::nr_arfcn_to_freq(cell.dl_carrier.arfcn) / 1e6,
               cell.dl_cfg_common.freq_info_dl.absolute_frequency_ssb,
               srsran::band_helper::nr_arfcn_to_freq(cell.ul_carrier.arfcn) / 1e6);
  }
  fmt::print("\n");

  fmt::print("==== gNodeB started ===\n");
  fmt::print("Type <t> to view trace\n");

  if (autostart_stdout_metrics) {
    metrics_plotter.enable_print();
  }
}

void gnb_console_helper::on_app_stopping()
{
  fmt::print("Stopping ..\n");
}
