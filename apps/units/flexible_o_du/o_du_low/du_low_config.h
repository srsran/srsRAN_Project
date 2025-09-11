/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
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

#include "apps/helpers/metrics/metrics_config.h"
#include "apps/services/worker_manager/os_sched_affinity_manager.h"
#include <optional>
#include <string>
#include <vector>

namespace srsran {

/// Expert upper physical layer configuration.
struct du_low_unit_expert_upper_phy_config {
  /// \brief Sets the maximum allowed downlink processing delay in slots.
  ///
  /// Higher values increase the downlink processing pipeline length, which improves performance and stability for
  /// demanding cell configurations, such as using large bandwidths or higher order MIMO. Higher values also increase
  /// the round trip latency of the radio link.
  unsigned max_processing_delay_slots = 5U;
  /// Number of PUSCH LDPC decoder iterations.
  unsigned pusch_decoder_max_iterations = 6;
  /// Set to true to enable the PUSCH LDPC decoder early stop.
  bool pusch_decoder_early_stop = true;
  /// Set to true for forcing the LDPC decoder to decode even if the number of soft bits is insufficient.
  bool pusch_decoder_force_decoding = false;
  /// \brief Selects a PUSCH SINR calculation method.
  ///
  /// Available methods:
  /// -\c channel_estimator: SINR is calculated by the channel estimator using the DM-RS.
  /// -\c post_equalization: SINR is calculated using the post-equalization noise variances of the equalized RE.
  /// -\c evm: SINR is obtained from the EVM of the PUSCH symbols.
  std::string pusch_sinr_calc_method = "post_equalization";
  /// \brief PUSCH channel estimator frequency-domain smoothing strategy.
  ///
  /// Use one of these options:
  /// - \c filter: applies a low pass filter to the channel estimates, or
  /// - \c mean: averages the channel estimates, or
  /// - \c none: it does not apply any smoothing strategy.
  std::string pusch_channel_estimator_fd_strategy = "filter";
  /// \brief PUSCH channel estimator time-domain interpolation strategy.
  ///
  /// Use one of these options:
  /// - \c average: averages the DM-RS in time domain, or
  /// - \c interpolate: performs linear interpolation between the OFDM symbols containing DM-RS.
  ///
  /// The \c average strategy is more robust against noise and interference while \c interpolate is more robust for
  /// fast fading channels.
  std::string pusch_channel_estimator_td_strategy = "average";
  /// PUSCH channel estimator CFO compensation.
  bool pusch_channel_estimator_cfo_compensation = true;
  /// \brief PUSCH channel equalizer algorithm.
  ///
  /// Use one of these options:
  /// - \c zf: use zero-forcing algorithm, or
  /// - \c mmse: use minimum mean square error algorithm.
  std::string pusch_channel_equalizer_algorithm = "zf";
  /// \brief Request headroom size in slots.
  ///
  /// The request headroom size is the number of delayed slots that the upper physical layer will accept, ie, if the
  /// current slot is M, the upper phy will consider the slot M - nof_slots_request_headroom as valid and process it.
  unsigned nof_slots_request_headroom = 0U;
  /// \brief Allows resource grid requests on empty uplink slots.
  ///
  /// An uplink slot is considered empty when it does not contain PUCCH/PUSCH/SRS PDUs.
  bool allow_request_on_empty_uplink_slot = false;
  /// Enables the PHY tap plugin if present.
  bool enable_phy_tap = true;
};

/// DU low logging functionalities.
struct du_low_unit_logger_config {
  /// Upper physical layer log level.
  srslog::basic_levels phy_level = srslog::basic_levels::warning;
  /// Hardware Abstraction Layer log level.
  srslog::basic_levels hal_level = srslog::basic_levels::warning;
  /// Set to true to log broadcasting messages and all PRACH opportunities.
  bool broadcast_enabled = false;
  /// Maximum number of bytes to write when dumping hex arrays.
  int hex_max_size = 0;
  /// Set to a valid file path to print the received symbols.
  std::string phy_rx_symbols_filename;
  /// Set to a valid Rx port number or empty for all ports.
  std::optional<unsigned> phy_rx_symbols_port = 0;
  /// If true, prints the PRACH frequency-domain symbols.
  bool phy_rx_symbols_prach = false;
};

/// CPU affinities configuration for the cell.
struct du_low_unit_cpu_affinities_cell_config {
  /// L1 downlink workers CPU affinity mask.
  os_sched_affinity_config l1_dl_cpu_cfg = {sched_affinity_mask_types::l1_dl, {}, sched_affinity_mask_policy::mask};
};

/// Expert threads configuration of the gNB app.
struct du_low_unit_expert_threads_config {
  du_low_unit_expert_threads_config()
  {
    unsigned nof_threads = cpu_architecture_info::get().get_host_nof_available_cpus();

    max_pucch_concurrency = 0;
    if (nof_threads <= 4) {
      max_pusch_and_srs_concurrency = 1;
    } else if (nof_threads < 8) {
      max_pusch_and_srs_concurrency = 2;
    } else if (nof_threads < 16) {
      max_pusch_and_srs_concurrency = 2;
    } else {
      max_pusch_and_srs_concurrency = 4;
    }
  }

  /// Codeblock batch length for ensuring synchronous processing within the flexible PDSCH processor implementation.
  static constexpr unsigned synchronous_cb_batch_length = std::numeric_limits<unsigned>::max();
  /// Codeblock default batch length.
  static constexpr unsigned default_cb_batch_length = 4;

  /// \brief PDSCH processor type.
  ///
  /// Use of one of these options:
  /// - \c auto: selects \c flexible implementation, or
  /// - \c generic: for using unoptimized PDSCH processing, or
  /// - \c flexible: for using a memory optimized processor if the number of codeblocks is smaller than \c
  /// pdsch_cb_batch_length, or a performance-optimized implementation that processes code blocks in parallel otherwise.
  std::string pdsch_processor_type = "auto";
  /// \brief PDSCH codeblock-batch length per thread (flexible PDSCH processor only).
  ///
  /// Set it to \c pdsch_cb_batch_length_sync for guaranteeing synchronous processing with the most memory-optimized
  /// processor.
  unsigned pdsch_cb_batch_length = default_cb_batch_length;
  /// \brief Maximum concurrency level for PUCCH.
  ///
  /// Maximum number of threads that can concurrently process Physical Uplink Control Channel (PUCCH). Set to zero for
  /// no limitation.
  unsigned max_pucch_concurrency = 0;
  /// \brief Maximum joint concurrency level for PUSCH and SRS.
  ///
  /// Maximum number of threads that can concurrently process Physical Uplink Shared Channel (PUSCH) and Sounding
  /// Reference Signals (SRS). Set to zero for no limitation.
  unsigned max_pusch_and_srs_concurrency = 1;
  /// \brief Maximum concurrency level for PDSCH processing.
  ///
  /// Maximum number of threads that can concurrently process Physical Downlink Shared Channel (PDSCH). Set to zero for
  /// no limitation.
  ///
  /// This parameter is necessary when hardware acceleration is used to limit the number of threads accessing the
  /// physical resources.
  unsigned max_pdsch_concurrency = 0;
};

/// Expert configuration of the gNB app.
struct du_low_unit_expert_execution_config {
  /// Expert thread configuration of the gNB app.
  du_low_unit_expert_threads_config threads;
  /// \brief CPU affinities per cell of the gNB app.
  ///
  /// \note Add one cell by default.
  std::vector<du_low_unit_cpu_affinities_cell_config> cell_affinities = {{}};
};

/// Hardware-accelerated PDSCH encoder configuration of the DU low.
struct hwacc_pdsch_appconfig {
  /// \brief Number of hardware-accelerated PDSCH encoding functions.
  unsigned nof_hwacc;
  /// \brief Operation mode of the PDSCH encoder (CB = true, TB = false [default]).
  bool cb_mode = false;
  /// \brief Maximum supported buffer size in bytes (CB mode will be forced for larger TBs). Only used in TB mode to
  /// size the mbufs.
  ///
  /// Set to the maximum supported size by default.
  std::optional<unsigned> max_buffer_size;
  /// \brief Type of hardware queue usage (dedicated = true [default], shared = false). In case of a shared usage, the
  /// accelerated function needs to reserve a hardware-queue for each operation.
  bool dedicated_queue = true;
};

/// Hardware-accelerated PUSCH decoder configuration of the DU low.
struct hwacc_pusch_appconfig {
  /// \brief Number of hardware-accelerated PUSCH decoding functions.
  unsigned nof_hwacc;
  /// \brief Size of the HARQ context repository.
  ///
  /// Set to the maximum number of CBs supported by the gNB config by default.
  std::optional<unsigned> harq_context_size;
  /// \brief Force using the host memory to implement the HARQ buffer (disabled by default).
  bool force_local_harq = false;
  /// \brief Type of hardware queue usage (dedicated = true [default], shared = false). In case of a shared usage, the
  /// accelerated function needs to reserve a hardware-queue for each operation.
  bool dedicated_queue = true;
};

/// BBDEV configuration of the DU low.
struct bbdev_appconfig {
  /// \brief Type of BBDEV hardware-accelerator.
  std::string hwacc_type;
  /// \brief ID of the BBDEV-based hardware-accelerator.
  unsigned id;
  /// \brief Structure providing the configuration of hardware-accelerated PDSCH encoding functions.
  std::optional<hwacc_pdsch_appconfig> pdsch_enc;
  /// \brief Structure providing the configuration of hardware-accelerated PUSCH decoding functions.
  std::optional<hwacc_pusch_appconfig> pusch_dec;
  /// \brief Size (in bytes) of each DPDK memory buffer (mbuf) used to exchange unencoded and unrate-matched messages
  /// with the accelerator.
  ///
  /// Set to the maximum supported size by default.
  std::optional<unsigned> msg_mbuf_size;
  /// \brief Size (in bytes) of each DPDK memory buffer (mbuf) used to exchange encoded and rate-matched messages with
  /// the accelerator.
  ///
  /// Set to the maximum supported size by default.
  std::optional<unsigned> rm_mbuf_size;
  /// \brief Number of DPDK memory buffers (mbufs) in each memory pool.
  ///
  /// Set to the maximum number of CBs supported by the gNB config by default.
  std::optional<unsigned> nof_mbuf;
};

/// HAL configuration of the DU low.
struct du_low_unit_hal_config {
  /// BBDEV-based hardware-accelerator arguments.
  std::optional<bbdev_appconfig> bbdev_hwacc;
};

/// Metrics configuration of the DU low.
struct du_low_unit_metrics_config {
  app_helpers::metrics_config common_metrics_cfg;
  bool                        enable_du_low    = false;
  unsigned                    du_report_period = 1000;
};

/// DU low configuration.
struct du_low_unit_config {
  /// Loggers.
  du_low_unit_logger_config loggers;
  /// Expert physical layer configuration.
  du_low_unit_expert_upper_phy_config expert_phy_cfg;
  /// Expert execution parameters for the DU low.
  du_low_unit_expert_execution_config expert_execution_cfg;
  /// HAL configuration.
  std::optional<du_low_unit_hal_config> hal_config;
  /// Metrics configuration.
  du_low_unit_metrics_config metrics_cfg;
};

} // namespace srsran
