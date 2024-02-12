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

#include "bbdev.h"

using namespace srsran;
using namespace dpdk;

expected<::rte_bbdev_info> dpdk::bbdev_start(const bbdev_acc_configuration& cfg, srslog::basic_logger& logger)
{
  ::rte_bbdev_info info = {};

  // Get information on the utilized bbdev hardware-accelerator.
  ::rte_bbdev_info_get(cfg.id, &info);

  // Setup the required queues.
  unsigned nof_queues = cfg.nof_ldpc_enc_lcores + cfg.nof_ldpc_dec_lcores + cfg.nof_fft_lcores;
  if (::rte_bbdev_setup_queues(cfg.id, nof_queues, info.socket_id) < 0) {
    logger.error("[bbdev] queues for device {} not setup properly.", cfg.id);
    return default_error_t{};
  }

  // Basic checking of hardware-accelerator capabilities.
  const ::rte_bbdev_op_cap* op_cap           = info.drv.capabilities;
  bool                      ldpc_enc_capable = false;
  bool                      ldpc_dec_capable = false;
  bool                      fft_capable      = false;

  while (op_cap->type != RTE_BBDEV_OP_NONE) {
    if (op_cap->type == RTE_BBDEV_OP_LDPC_ENC) {
      ldpc_enc_capable = true;
    } else if (op_cap->type == RTE_BBDEV_OP_LDPC_DEC) {
      ldpc_dec_capable = true;
    } else if (op_cap->type == RTE_BBDEV_OP_FFT) {
      fft_capable = true;
    }
    ++op_cap;
  }
  if ((cfg.nof_ldpc_enc_lcores > 0 && !ldpc_enc_capable) || (cfg.nof_ldpc_dec_lcores > 0 && !ldpc_dec_capable) ||
      (cfg.nof_fft_lcores > 0 && !fft_capable)) {
    logger.error("[bbdev] device {} does not provide the requested acceleration functions.", cfg.id);
    return default_error_t{};
  }

  // Enable interruptions.
  if (::rte_bbdev_intr_enable(cfg.id) < 0) {
    logger.error("[bbdev] interrupts for device {} not setup properly.", cfg.id);
    return default_error_t{};
  }

  // Configure the queues (only those required).
  // The type of accelerated function assigned to a queue is following a known pattern: LDPC encoder, LDPC decoder and
  // FFT.
  ::rte_bbdev_queue_conf queue_conf = {0};
  unsigned               queue_id   = 0;
  queue_conf.socket                 = info.socket_id;
  queue_conf.queue_size             = info.drv.queue_size_lim;
  queue_conf.priority               = 0;
  if (cfg.nof_ldpc_enc_lcores > 0) {
    queue_conf.op_type = RTE_BBDEV_OP_LDPC_ENC;
    for (unsigned qid = 0, lastq = cfg.nof_ldpc_enc_lcores; qid != lastq; ++qid) {
      if (::rte_bbdev_queue_configure(cfg.id, queue_id, &queue_conf) < 0) {
        logger.error("[bbdev] device {} queue {} (ldpc encoder) not configured properly.", cfg.id, queue_id);
        return default_error_t{};
      }
      ++queue_id;
    }
  }
  if (cfg.nof_ldpc_dec_lcores > 0) {
    queue_conf.op_type = RTE_BBDEV_OP_LDPC_DEC;
    for (unsigned qid = 0, lastq = cfg.nof_ldpc_dec_lcores; qid != lastq; ++qid) {
      if (::rte_bbdev_queue_configure(cfg.id, queue_id, &queue_conf) < 0) {
        logger.error("[bbdev] device {} queue {} (ldpc decoder) not configured properly.", cfg.id, queue_id);
        return default_error_t{};
      }
      ++queue_id;
    }
  }
  if (cfg.nof_fft_lcores > 0) {
    queue_conf.op_type = RTE_BBDEV_OP_FFT;
    for (unsigned qid = 0, lastq = cfg.nof_fft_lcores; qid != lastq; ++qid) {
      if (::rte_bbdev_queue_configure(cfg.id, queue_id, &queue_conf) < 0) {
        logger.error("[bbdev] device {} queue {} (fft) not configured properly.", cfg.id, queue_id);
        return default_error_t{};
      }
      ++queue_id;
    }
  }

  if (::rte_bbdev_start(cfg.id) < 0) {
    logger.error("[bbdev] device {} not started.", cfg.id);
    return default_error_t{};
  }

  return info;
}

bool dpdk::bbdev_stop(unsigned dev_id, srslog::basic_logger& logger)
{
  if (::rte_bbdev_close(dev_id) < 0) {
    logger.error("[bbdev] device {} not closed.", dev_id);
    return false;
  }

  return true;
}

::rte_mempool* dpdk::create_op_pool(const char*           pool_name,
                                    ::rte_bbdev_op_type   op_type,
                                    uint16_t              nof_elements,
                                    int                   socket,
                                    srslog::basic_logger& logger)

{
  static constexpr unsigned NB_MBUF = 2048;

  ::rte_mempool* op_pool = ::rte_bbdev_op_pool_create(pool_name, op_type, NB_MBUF, nof_elements, socket);

  if (op_pool == nullptr) {
    logger.error("dpdk: create_op_pool '{}' failed", pool_name);
  }

  return op_pool;
}
