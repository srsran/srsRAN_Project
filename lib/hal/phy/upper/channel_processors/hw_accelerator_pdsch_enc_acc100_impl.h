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

/// \file
/// \brief Hardware accelerated PDSCH encoder functions declaration.

#pragma once

#include "../../../dpdk/bbdev/bbdev.h"
#include "hw_accelerator_pdsch_enc_impl.h"
#include "srsran/adt/static_vector.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/dpdk/bbdev/bbdev_op_pool.h"
#include "srsran/hal/dpdk/mbuf_pool.h"
#include "srsran/ran/sch/sch_constants.h"

namespace srsran {
namespace hal {

/// Class representing the ACC100 implementation of PDSCH encoding.
class hw_accelerator_pdsch_enc_acc100_impl : public hw_accelerator_pdsch_enc_impl
{
  /// Hardware-specific implementation of the reserve queue function.
  void hw_reserve_queue() override;
  /// Hardware-specific implementation of the free queue function.
  void hw_free_queue() override;
  /// Hardware-specific implementation of the enqueue_operation function.
  bool hw_enqueue(span<const uint8_t> data, unsigned cb_index) override;
  /// Hardware-specific implementation of the dequeue_operation function.
  bool hw_dequeue(span<uint8_t> data, span<uint8_t> packed_data, unsigned segment_index) override;
  /// Hardware-specific configuration function.
  void hw_config(const hw_pdsch_encoder_configuration& config, unsigned cb_index) override;
  /// Hardware-specific CB mode quering function.
  bool get_hw_cb_mode() const override;
  /// Hardware-specific maximum supported TB size quering function.
  unsigned get_hw_max_tb_size() const override;

  /// Allocate the required resources from the bbdev-based hardware-accelerator.
  void allocate_resources();

  /// \file
  /// \brief Members specific to bbdev-accelerated LDPC encoder functions.

  /// Unique ID of the current hardware-accelerated function.
  unsigned id;

  /// Pointer to a bbdev-based hardware-accelerator.
  std::shared_ptr<srsran::dpdk::bbdev_acc> bbdev_accelerator;

  /// Private member to store the ID of the bbdev-based hardware-accelerator.
  unsigned device_id;

  /// Private member to store the ID of the queue used by the hardware-accelerated LDPC encoder function.
  int queue_id;

  /// Indicates the number of encoding operations in the queue.
  unsigned nof_enqueued_op = 0;

  /// Operation pool used by the hardware-accelerated LDPC encoder.
  std::unique_ptr<srsran::dpdk::bbdev_op_pool> op_pool;

  /// Structure specifying each hardware-accelerated encode operation.
  ::rte_bbdev_enc_op op[MAX_NOF_SEGMENTS];

  /// Input-data mbuf pool used by the hardware-accelerated LDPC encoder.
  std::unique_ptr<srsran::dpdk::mbuf_pool> in_mbuf_pool;

  /// Output-data mbuf pool used by the hardware-accelerated LDPC encoder.
  std::unique_ptr<srsran::dpdk::mbuf_pool> out_mbuf_pool;

  /// \file
  /// \brief Common members to all hardware-accelerated LDPC encoder functions.

  /// Private member to store the configuration of the current operation.
  hw_pdsch_encoder_configuration enc_config;

  /// Private member to store the operation mode.
  bool cb_mode = false;

  /// Private member to store the maximum supported TB size (in bytes).
  unsigned max_tb_size;

  /// Private member to store the TB CRC (only for TB mode operation).
  static_vector<uint8_t, 3> tb_crc;

  /// Array flagging those encoding operations that will be dropped due to enqueueing errors.
  bounded_bitset<MAX_NOF_SEGMENTS> drop_op;

  /// Indicates if the accelerated function uses a dedicated hardware queue or needs to reserve one for each operation.
  bool dedicated_queue;

public:
  /// Constructor taking care of obtaining a bbdev-based hardware-accelerator queue and allocating the required
  /// resources.
  hw_accelerator_pdsch_enc_acc100_impl(std::shared_ptr<srsran::dpdk::bbdev_acc> bbdev_accelerator_,
                                       bool                                     cb_mode_,
                                       unsigned                                 max_tb_size_,
                                       bool                                     dedicated_queue_) :
    bbdev_accelerator(std::move(bbdev_accelerator_)),
    cb_mode(cb_mode_),
    max_tb_size(max_tb_size_),
    dedicated_queue(dedicated_queue_)
  {
    id        = bbdev_accelerator->reserve_encoder();
    device_id = bbdev_accelerator->get_device_id();
    // Reserve a hardware queue in case of dedicated use.
    queue_id = -1;
    if (dedicated_queue) {
      hw_reserve_queue();
      srsran_assert(queue_id >= 0, "No free RTE_BBDEV_OP_LDPC_ENC queues available.");
    }
    allocate_resources();
    drop_op.resize(MAX_NOF_SEGMENTS);
    drop_op.reset();

    // HAL logging.
    srslog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[acc100] new encoder: id={}.", id);
  }

  /// Destructor taking care of freeing the utilized resources.
  ~hw_accelerator_pdsch_enc_acc100_impl()
  {
    // Free the reserved hardware queue in case of dedicated use.
    dedicated_queue = false;
    hw_free_queue();

    // HAL logging.
    srslog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[acc100] destroyed encoder: id={}.", id);
  }
};

} // namespace hal
} // namespace srsran
