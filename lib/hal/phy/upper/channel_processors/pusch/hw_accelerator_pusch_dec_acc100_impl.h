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
/// \brief Hardware accelerated PUSCH decoder functions declaration.

#pragma once

#include "../../../../dpdk/bbdev/bbdev.h"
#include "hw_accelerator_pusch_dec_impl.h"
#include "srsran/adt/static_vector.h"
#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "srsran/hal/dpdk/bbdev/bbdev_op_pool.h"
#include "srsran/hal/dpdk/mbuf_pool.h"
#include "srsran/hal/phy/upper/channel_processors/pusch/ext_harq_buffer_context_repository_factory.h"
#include "srsran/ran/sch/sch_constants.h"

namespace srsran {
namespace hal {

/// Class representing the ACC100 implementation of PUSCH decoding.
class hw_accelerator_pusch_dec_acc100_impl : public hw_accelerator_pusch_dec_impl
{
  /// Hardware-specific implementation of the enqueue_operation function.
  bool hw_enqueue(span<const int8_t> data, span<const int8_t> soft_data, unsigned cb_index) override;
  /// Hardware-specific implementation of the dequeue_operation function.
  bool hw_dequeue(span<uint8_t> data, span<int8_t> soft_data, unsigned segment_index) override;
  /// Hardware-specific configuration function.
  void hw_config(const hw_pusch_decoder_configuration& config, unsigned cb_index) override;
  /// Hardware-specific operation status outputs recovery function.
  void hw_read_outputs(hw_pusch_decoder_outputs& out, unsigned cb_index, unsigned absolute_cb_id) override;
  /// Hardware-specific HARQ buffer context freeing function.
  void hw_free_harq_context(unsigned absolute_cb_id) override;
  /// Hardware-specific external HARQ buffer checking function.
  bool is_hw_external_harq_supported() const override;

  /// Allocate the required resources from the bbdev-based hardware-accelerator.
  void allocate_resources();

  /// \file
  /// \brief Members specific to bbdev-accelerated LDPC decoder functions.

  /// Pointer to a bbdev-based hardware-accelerator.
  std::shared_ptr<srsran::dpdk::bbdev_acc> bbdev_accelerator;

  /// Private member to store the ID of the bbdev-based hardware-accelerator.
  unsigned device_id;

  /// Private member to store the ID of the queue used by the hardware-accelerated LDPC decoder function.
  uint16_t queue_id;

  /// Indicates the number of decoding operations in the queue.
  unsigned nof_enqueued_op = 0;

  /// Operation pool used by the hardware-accelerated LDPC decoder.
  std::unique_ptr<srsran::dpdk::bbdev_op_pool> op_pool;

  /// Structure specifying each hardware-accelerated decode operation.
  ::rte_bbdev_dec_op op[MAX_NOF_SEGMENTS];

  /// Input-data mbuf pool used by the hardware-accelerated LDPC decoder.
  std::unique_ptr<srsran::dpdk::mbuf_pool> in_mbuf_pool;

  /// HARQ input-data mbuf pool used by the hardware-accelerated LDPC decoder.
  std::unique_ptr<srsran::dpdk::mbuf_pool> harq_in_mbuf_pool;

  /// Output-data mbuf pool used by the hardware-accelerated LDPC decoder.
  std::unique_ptr<srsran::dpdk::mbuf_pool> out_mbuf_pool;

  /// HARQ output-data mbuf pool used by the hardware-accelerated LDPC decoder.
  std::unique_ptr<srsran::dpdk::mbuf_pool> harq_out_mbuf_pool;

  /// \file
  /// \brief Common members to all hardware-accelerated LDPC decoder functions.

  /// Private member to store the configuration of the current operation.
  hw_pusch_decoder_configuration dec_config;

  /// Private member to store the soft-buffer implementation type.
  bool ext_softbuffer;

  /// Context of the external HARQ buffer.
  std::shared_ptr<ext_harq_buffer_context_repository> harq_buffer_context;

  /// Array flagging those decoding operations that will be dropped due to enqueueing errors.
  bounded_bitset<MAX_NOF_SEGMENTS> drop_op;

  /// Private member to store the outputs of the current operation.
  hw_pusch_decoder_outputs dec_out;

  /// HARQ context repository entry for the current decoding operation.
  std::vector<ext_harq_buffer_context_entry*> harq_context_entries;

public:
  /// Constructor taking care of obtaining a bbdev-based hardware-accelerator queue and allocating the required
  /// resources.
  hw_accelerator_pusch_dec_acc100_impl(std::shared_ptr<srsran::dpdk::bbdev_acc>            bbdev_accelerator_,
                                       bool                                                ext_softbuffer_,
                                       std::shared_ptr<ext_harq_buffer_context_repository> harq_buffer_context_) :
    bbdev_accelerator(std::move(bbdev_accelerator_)),
    ext_softbuffer(ext_softbuffer_),
    harq_buffer_context(std::move(harq_buffer_context_))
  {
    int qid = bbdev_accelerator->reserve_queue(RTE_BBDEV_OP_LDPC_DEC);
    srsran_assert(qid >= 0, "No free bbdev queues available.");
    queue_id  = static_cast<uint16_t>(qid);
    device_id = bbdev_accelerator->get_device_id();
    allocate_resources();
    drop_op.resize(MAX_NOF_SEGMENTS);
    drop_op.reset();
    harq_context_entries.reserve(MAX_NOF_SEGMENTS);

    // HAL logging.
    srslog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[acc100] new decoder: queue={}.", queue_id);
  }

  /// Destructor taking care of freeing the utilized resources and hardware-accelerator queue.
  ~hw_accelerator_pusch_dec_acc100_impl()
  {
    bbdev_accelerator->free_queue(queue_id);

    // HAL logging.
    srslog::basic_logger& logger = bbdev_accelerator->get_logger();
    logger.info("[acc100] destroyed decoder: queue={}.", queue_id);
  }
};

} // namespace hal
} // namespace srsran
