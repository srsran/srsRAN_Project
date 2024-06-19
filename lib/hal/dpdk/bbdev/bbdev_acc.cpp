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

#include "srsran/hal/dpdk/bbdev/bbdev_acc.h"
#include "bbdev.h"

using namespace srsran;
using namespace dpdk;

bbdev_acc::bbdev_acc(const bbdev_acc_configuration& cfg, const ::rte_bbdev_info& info_, srslog::basic_logger& logger_) :
  id(cfg.id),
  info(info_),
  nof_ldpc_enc_lcores(cfg.nof_ldpc_enc_lcores),
  nof_ldpc_dec_lcores(cfg.nof_ldpc_dec_lcores),
  nof_fft_lcores(cfg.nof_fft_lcores),
  available_ldpc_enc_queue(MAX_NOF_BBDEV_QUEUES),
  available_ldpc_dec_queue(MAX_NOF_BBDEV_QUEUES),
  available_fft_queue(MAX_NOF_BBDEV_QUEUES),
  msg_mbuf_size(cfg.msg_mbuf_size),
  rm_mbuf_size(cfg.rm_mbuf_size),
  nof_mbuf(cfg.nof_mbuf),
  logger(logger_)
{
  unsigned nof_vfs = nof_ldpc_enc_lcores + nof_ldpc_dec_lcores + nof_fft_lcores;
  srsran_assert(nof_ldpc_enc_lcores <= MAX_NOF_BBDEV_VF_INSTANCES,
                "Requested {} LDPC encoder VFs but only {} are available.",
                nof_ldpc_enc_lcores,
                MAX_NOF_BBDEV_VF_INSTANCES);
  srsran_assert(nof_ldpc_dec_lcores <= MAX_NOF_BBDEV_VF_INSTANCES,
                "Requested {} LDPC decoder VFs but only {} are available.",
                nof_ldpc_dec_lcores,
                MAX_NOF_BBDEV_VF_INSTANCES);
  srsran_assert(nof_fft_lcores <= MAX_NOF_BBDEV_VF_INSTANCES,
                "Requested {} FFT VFs but only {} are available.",
                nof_fft_lcores,
                MAX_NOF_BBDEV_VF_INSTANCES);
  srsran_assert(nof_vfs <= MAX_NOF_BBDEV_QUEUES,
                "Requested {} BBDEV VFs but only {} are available.",
                nof_vfs,
                MAX_NOF_BBDEV_QUEUES);

  nof_ldpc_enc_instances = 0;
  nof_ldpc_dec_instances = 0;
  // Hardware-accelerated LDPC encoder functions use queues 0:(nof_ldpc_enc_lcores - 1)
  for (unsigned qid = 0, lastq = nof_ldpc_enc_lcores; qid != lastq; qid++) {
    available_ldpc_enc_queue.try_push(qid);
  }
  // Hardware-accelerated LDPC decoder functions use queues nof_ldpc_enc_lcores:(nof_ldpc_enc_lcores +
  // nof_ldpc_dec_lcores - 1)
  for (unsigned qid = nof_ldpc_enc_lcores, lastq = nof_ldpc_enc_lcores + nof_ldpc_dec_lcores; qid != lastq; qid++) {
    available_ldpc_dec_queue.try_push(qid);
  }
  // Hardware-accelerated FFT functions use queues (nof_ldpc_enc_lcores + nof_ldpc_dec_lcores):(nof_ldpc_enc_lcores +
  // nof_ldpc_dec_lcores + nof_fft_lcores - 1)
  for (unsigned qid = nof_ldpc_enc_lcores + nof_ldpc_dec_lcores, lastq = nof_vfs; qid != lastq; qid++) {
    available_fft_queue.try_push(qid);
  }
}

bbdev_acc::~bbdev_acc()
{
  // bbdev device stop procedure.
  ::bbdev_stop(id, logger);
}

int bbdev_acc::reserve_queue(::rte_bbdev_op_type op_type)
{
  int queue_id = -1;

  if (op_type == RTE_BBDEV_OP_LDPC_ENC) {
    // Try to get an available LDPC encoder queue.
    std::optional<unsigned> qid = available_ldpc_enc_queue.try_pop();
    if (qid.has_value()) {
      queue_id = qid.value();
    }
  } else if (op_type == RTE_BBDEV_OP_LDPC_DEC) {
    // Try to get an available LDPC decoder queue.
    std::optional<unsigned> qid = available_ldpc_dec_queue.try_pop();
    if (qid.has_value()) {
      queue_id = qid.value();
    }
  } else {
    // Try to get an available FFT queue.
    std::optional<unsigned> qid = available_fft_queue.try_pop();
    if (qid.has_value()) {
      queue_id = qid.value();
    }
  }

  return queue_id;
}

void bbdev_acc::free_queue(::rte_bbdev_op_type op_type, unsigned queue_id)
{
  if (op_type == RTE_BBDEV_OP_LDPC_ENC) {
    // Free a LDPC encoder queue.
    while (!available_ldpc_enc_queue.try_push(queue_id)) {
    }
  } else if (op_type == RTE_BBDEV_OP_LDPC_DEC) {
    // Free a LDPC decoder queue.
    while (!available_ldpc_dec_queue.try_push(queue_id)) {
    }
  } else {
    // Free a FFT queue.
    while (!available_fft_queue.try_push(queue_id)) {
    }
  }
}
