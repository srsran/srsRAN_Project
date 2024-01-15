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

#pragma once

#include "srsran/adt/bounded_bitset.h"
#include "srsran/srslog/logger.h"
#include "srsran/support/units.h"
#include <rte_bbdev.h>
#include <rte_bbdev_op.h>

namespace srsran {
namespace dpdk {

/// Maximum number of queues supported by a bbdev-based hardware-accelerator.
static constexpr unsigned MAX_NOF_BBDEV_QUEUES = 128;
/// Maximum number of operations that can be stored in a hardware queue at a given time.
static constexpr unsigned MAX_NOF_OP_IN_QUEUE = 16;

/// Configuration parameters and objects tied to a bbdev-based hardware-accelerator.
struct bbdev_acc_configuration {
  /// ID of the bbdev-based hardware-accelerator.
  unsigned id;
  /// Number of lcores available to the hardware-accelerated LDPC encoder (disabled if 0).
  unsigned nof_ldpc_enc_lcores = 0;
  /// Number of lcores available to the hardware-accelerated LDPC decoder (disabled if 0).
  unsigned nof_ldpc_dec_lcores = 0;
  /// Number of lcores available to the hardware-accelerated FFT (disabled if 0).
  unsigned nof_fft_lcores = 0;
  /// Size of each mbuf used to exchange unencoded and unrate-matched messages with the accelerator in bytes.
  /// Note that by default it is initialized to the maximum size supported by an mbuf.
  unsigned msg_mbuf_size = RTE_BBDEV_LDPC_E_MAX_MBUF;
  /// Size of each mbuf used to exchange encoded and rate-matched messages with the accelerator in bytes.
  /// Note that by default it is initialized to the maximum size supported by an mbuf.
  unsigned rm_mbuf_size = RTE_BBDEV_LDPC_E_MAX_MBUF;
  /// Number of mbufs in each memory pool.
  unsigned nof_mbuf = 256;
};

/// Abstracted interfacing to bbdev-based hardware-accelerators.
class bbdev_acc
{
public:
  /// Constructor.
  /// \param[in] dpdk Pointer to a dpdk-based hardware-accelerator interface.
  /// \param[in] info bbdev Device information.
  /// \param[in] cfg  Configuration parameters of the bbdev-based hardware-accelerator.
  explicit bbdev_acc(const bbdev_acc_configuration& cfg, const ::rte_bbdev_info& info_, srslog::basic_logger& logger);

  /// Destructor.
  ~bbdev_acc();

  /// Returns the ID of the bbdev-based hardware-accelerator device.
  /// \return Device ID.
  unsigned get_device_id() const { return id; }

  /// Returns the ID of the socket used by the bbdev-based hardware-accelerator.
  /// \return Socket ID.
  int get_socket_id() const { return info.socket_id; }

  /// Returns the number of LDPC encoder cores provided by the bbdev-based hardware-accelerator.
  /// \return Number of LDPC encoder cores.
  unsigned get_nof_ldpc_enc_cores() const { return nof_ldpc_enc_lcores; }

  /// Returns the number of LDPC decoder cores provided by the bbdev-based hardware-accelerator.
  /// \return Number of LDPC decoder cores.
  unsigned get_nof_ldpc_dec_cores() const { return nof_ldpc_dec_lcores; }

  /// Returns the number of FFT cores provided by the bbdev-based hardware-accelerator.
  /// \return Number of FFT cores.
  unsigned get_nof_fft_cores() const { return nof_fft_lcores; }

  /// Returns the size of the (external) HARQ buffer size embedded in the hardware-accelerator.
  /// \return HARQ buffer size (in bytes).
  units::bytes get_harq_buff_size() const { return units::bytes(1024 * info.drv.harq_buffer_size); }

  /// Returns the size of each mbuf used to exchange unencoded and unrate-matched messages with the accelerator.
  /// \return Unencoded and unrate-matched mbuf size (in bytes).
  units::bytes get_msg_mbuf_size() const { return units::bytes(msg_mbuf_size); }

  /// Returns the size of each mbuf used to exchange encoded and rate-matched messages with the accelerator.
  /// \return Encoded and rate-matched mbuf size (in bytes).
  units::bytes get_rm_mbuf_size() const { return units::bytes(rm_mbuf_size); }

  /// Returns the number of mbufs in each memory pool used to exchange data with the accelerator.
  /// \return Number of mbufs.
  unsigned get_nof_mbuf() const { return nof_mbuf; }

  /// Returns the internal SRS logger.
  /// \return SRS logger.
  srslog::basic_logger& get_logger() { return logger; }

  /// Reserves a free queue to be used by a specific hardware-accelerated channel processor function.
  /// \param[in] op_type Type of bbdev op.
  /// \return ID of the reserved queue.
  int reserve_queue(rte_bbdev_op_type op_type);

  /// Frees a queue used by a specific hardware-accelerated channel processor function.
  /// \param[in] queue_id ID of the queue to be freed.
  void free_queue(unsigned queue_id);

private:
  /// ID of the bbdev-based hardware-accelerator.
  unsigned id;
  /// Structure providing device information.
  ::rte_bbdev_info info;
  /// Number of lcores available to the hardware-accelerated LDPC encoder (disabled if 0).
  unsigned nof_ldpc_enc_lcores;
  /// Number of lcores available to the hardware-accelerated LDPC decoder (disabled if 0).
  unsigned nof_ldpc_dec_lcores;
  /// Number of lcores available to the hardware-accelerated FFT (disabled if 0).
  unsigned nof_fft_lcores;
  /// Array indicating the queue availability.
  bounded_bitset<MAX_NOF_BBDEV_QUEUES> queue_used;
  /// Size of each mbuf used to exchange unencoded and unrate-matched messages with the accelerator in bytes.
  unsigned msg_mbuf_size;
  /// Size of each mbuf used to exchange encoded and rate-matched messages with the accelerator in bytes.
  unsigned rm_mbuf_size;
  /// Number of mbufs in each memory pool.
  unsigned nof_mbuf;
  /// SRS logger.
  srslog::basic_logger& logger;
};

} // namespace dpdk
} // namespace srsran
