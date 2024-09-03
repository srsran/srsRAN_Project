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
#include "srsran/adt/mpmc_queue.h"
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
/// Maximum number of VF instances supported by a bbdev-based hardware-accelerator.
static constexpr unsigned MAX_NOF_BBDEV_VF_INSTANCES = 64;

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

/// Wireless Baseband Device (bbdev) interface.
class bbdev_acc
{
public:
  /// Default destructor.
  virtual ~bbdev_acc() = default;

  /// Returns the ID of the bbdev-based hardware-accelerator device.
  virtual unsigned get_device_id() const = 0;

  /// Returns the ID of the socket used by the bbdev-based hardware-accelerator.
  virtual int get_socket_id() const = 0;

  /// Returns the number of LDPC encoder cores provided by the bbdev-based hardware-accelerator.
  virtual unsigned get_nof_ldpc_enc_cores() const = 0;

  /// Returns the number of LDPC decoder cores provided by the bbdev-based hardware-accelerator.
  virtual unsigned get_nof_ldpc_dec_cores() const = 0;

  /// Returns the number of FFT cores provided by the bbdev-based hardware-accelerator.
  virtual unsigned get_nof_fft_cores() const = 0;

  /// Returns the size of the (external) HARQ buffer size embedded in the hardware-accelerator.
  virtual uint64_t get_harq_buff_size_bytes() const = 0;

  /// Returns the size of each mbuf used to exchange unencoded and unrate-matched messages with the accelerator.
  virtual units::bytes get_msg_mbuf_size() const = 0;

  /// Returns the size of each mbuf used to exchange encoded and rate-matched messages with the accelerator.
  virtual units::bytes get_rm_mbuf_size() const = 0;

  /// Returns the number of mbufs in each memory pool used to exchange data with the accelerator.
  virtual unsigned get_nof_mbuf() const = 0;

  /// Returns the internal SRS logger.
  virtual srslog::basic_logger& get_logger() = 0;

  /// \brief Reserves a free queue to be used by a specific hardware-accelerated channel processor function.
  /// \param[in] op_type Type of bbdev op.
  /// \return The identifier of the reserved queue if successful, otherwise a negative integer.
  virtual int reserve_queue(::rte_bbdev_op_type op_type) = 0;

  /// \brief Frees a queue used by a specific hardware-accelerated channel processor function.
  /// \param[in] queue_id Identifier of the queue to be freed.
  virtual void free_queue(::rte_bbdev_op_type op_type, unsigned queue_id) = 0;

  /// Returns a unique ID for an instance of an LDPC encoder using the bbdev-based accelerator.
  virtual unsigned reserve_encoder() = 0;

  /// Returns a unique ID for an instance of an LDPC decoder using the bbdev-based accelerator.
  virtual unsigned reserve_decoder() = 0;
};

} // namespace dpdk
} // namespace srsran
