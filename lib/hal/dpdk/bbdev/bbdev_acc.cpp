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
  queue_used(cfg.nof_ldpc_enc_lcores + cfg.nof_ldpc_dec_lcores + cfg.nof_fft_lcores),
  msg_mbuf_size(cfg.msg_mbuf_size),
  rm_mbuf_size(cfg.rm_mbuf_size),
  nof_mbuf(cfg.nof_mbuf),
  logger(logger_)
{
}

bbdev_acc::~bbdev_acc()
{
  // bbdev device stop procedure.
  ::bbdev_stop(id, logger);
}

int bbdev_acc::reserve_queue(::rte_bbdev_op_type op_type)
{
  int queue_id = -1;

  // Hardware-accelerated LDPC encoder functions use queues 0:(nof_ldpc_enc_lcores - 1)
  if (op_type == RTE_BBDEV_OP_LDPC_ENC) {
    for (unsigned qid = 0, lastq = nof_ldpc_enc_lcores; qid != lastq; qid++) {
      if (!queue_used.test(qid)) {
        queue_id = qid;
        queue_used.set(qid);
        break;
      }
    }
  }
  // Hardware-accelerated LDPC decoder functions use queues nof_ldpc_enc_lcores:(nof_ldpc_enc_lcores +
  // nof_ldpc_dec_lcores - 1)
  else if (op_type == RTE_BBDEV_OP_LDPC_DEC) {
    for (unsigned qid = nof_ldpc_enc_lcores, lastq = nof_ldpc_enc_lcores + nof_ldpc_dec_lcores; qid != lastq; qid++) {
      if (!queue_used.test(qid)) {
        queue_id = qid;
        queue_used.set(qid);
        break;
      }
    }
  }
  // Hardware-accelerated FFT functions use queues (nof_ldpc_enc_lcores + nof_ldpc_dec_lcores):(nof_ldpc_enc_lcores +
  // nof_ldpc_dec_lcores + nof_fft_lcores - 1)
  else {
    for (unsigned qid   = nof_ldpc_enc_lcores + nof_ldpc_dec_lcores,
                  lastq = nof_ldpc_enc_lcores + nof_ldpc_dec_lcores + nof_fft_lcores;
         qid != lastq;
         qid++) {
      if (!queue_used.test(qid)) {
        queue_id = qid;
        queue_used.set(qid);
        break;
      }
    }
  }

  // Log failure to obtain a free queue.
  if (queue_id < 0) {
    if (op_type == RTE_BBDEV_OP_LDPC_ENC) {
      logger.error("[bbdev] unable to reserve a free LDPC encoder queue in device {}.", id);
    } else if (op_type == RTE_BBDEV_OP_LDPC_DEC) {
      logger.error("[bbdev] unable to reserve a free LDPC decoder queue in device {}.", id);
    } else {
      logger.error("[bbdev] unable to reserve a free FFT queue in device {}.", id);
    }
  }

  return queue_id;
}

void bbdev_acc::free_queue(unsigned queue_id)
{
  queue_used.reset(queue_id);
}
