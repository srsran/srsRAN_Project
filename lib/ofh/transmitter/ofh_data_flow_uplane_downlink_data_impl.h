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

#include "ofh_data_flow_uplane_downlink_data.h"
#include "sequence_identifier_generator.h"
#include "srsran/instrumentation/traces/ofh_traces.h"
#include "srsran/ofh/compression/iq_compressor.h"
#include "srsran/ofh/ecpri/ecpri_packet_builder.h"
#include "srsran/ofh/ethernet/vlan_ethernet_frame_builder.h"
#include "srsran/ofh/serdes/ofh_uplane_message_builder.h"
#include "srsran/ran/cyclic_prefix.h"
#include "srsran/srslog/srslog.h"

namespace srsran {
struct resource_grid_context;

namespace ether {
class eth_frame_pool;
}

namespace ofh {

/// Open Fronthaul User-Plane downlink data flow implementation configuration.
struct data_flow_uplane_downlink_data_impl_config {
  /// Cyclic prefix.
  cyclic_prefix cp;
  /// RU bandwidth in PRBs.
  unsigned ru_nof_prbs;
  /// Downlink eAxCs.
  static_vector<unsigned, MAX_NOF_SUPPORTED_EAXC> dl_eaxc;
  /// VLAN frame parameters.
  ether::vlan_frame_params vlan_params;
  /// Compression parameters.
  ru_compression_params compr_params;
};

/// Open Fronthaul User-Plane downlink data flow implementation dependencies.
struct data_flow_uplane_downlink_data_impl_dependencies {
  /// Logger
  srslog::basic_logger* logger = nullptr;
  /// Ethernet frame pool.
  std::shared_ptr<ether::eth_frame_pool> frame_pool;
  /// VLAN frame builder.
  std::unique_ptr<ether::vlan_frame_builder> eth_builder;
  /// eCPRI packet builder.
  std::unique_ptr<ecpri::packet_builder> ecpri_builder;
  /// IQ compressor.
  std::unique_ptr<iq_compressor> compressor_sel;
  /// User-Plane message builder.
  std::unique_ptr<uplane_message_builder> up_builder;
};

/// Stores trace names used by the \c data_flow_uplane_downlink_data_impl class when OFH tracing is enabled.
template <bool Enabled = true>
class ofh_uplane_trace_names
{
  std::array<std::string, MAX_SUPPORTED_EAXC_ID_VALUE> trace_names;

public:
  explicit ofh_uplane_trace_names(span<const unsigned> dl_eaxc)
  {
    for (unsigned eaxc : dl_eaxc) {
      trace_names[eaxc] = fmt::format("ofh_uplane_eaxc_{}", eaxc);
    }
  }

  const std::string& operator[](std::size_t eaxc) const { return trace_names[eaxc]; }
};

/// Specialization of ofh_uplane_trace_names used when OFH event tracing is disabled.
template <>
class ofh_uplane_trace_names<false>
{
public:
  explicit ofh_uplane_trace_names(span<const unsigned> dl_eaxc) {}

  const std::string operator[](std::size_t eaxc) const { return ""; }
};

/// Open Fronthaul User-Plane downlink data flow implementation.
class data_flow_uplane_downlink_data_impl : public data_flow_uplane_downlink_data
{
public:
  explicit data_flow_uplane_downlink_data_impl(const data_flow_uplane_downlink_data_impl_config&  config,
                                               data_flow_uplane_downlink_data_impl_dependencies&& dependencies);

  // See interface for documentation.
  void enqueue_section_type_1_message(const data_flow_uplane_resource_grid_context& context,
                                      const resource_grid_reader&                   grid) override;

private:
  /// Enqueues an User-Plane message burst.
  void enqueue_section_type_1_message_symbol_burst(const data_flow_uplane_resource_grid_context& context,
                                                   const resource_grid_reader&                   grid);

  /// Enqueues an User-Plane message symbol with the given context and grid.
  unsigned enqueue_section_type_1_message_symbol(span<const cbf16_t>          iq_symbol_data,
                                                 const uplane_message_params& params,
                                                 unsigned                     eaxc,
                                                 span<uint8_t>                buffer);

private:
  srslog::basic_logger&                      logger;
  const unsigned                             nof_symbols_per_slot;
  const unsigned                             ru_nof_prbs;
  const ether::vlan_frame_params             vlan_params;
  const ru_compression_params                compr_params;
  sequence_identifier_generator              up_seq_gen;
  std::shared_ptr<ether::eth_frame_pool>     frame_pool;
  std::unique_ptr<iq_compressor>             compressor_sel;
  std::unique_ptr<ether::vlan_frame_builder> eth_builder;
  std::unique_ptr<ecpri::packet_builder>     ecpri_builder;
  std::unique_ptr<uplane_message_builder>    up_builder;
  ofh_uplane_trace_names<OFH_TRACE_ENABLED>  formatted_trace_names;
};

} // namespace ofh
} // namespace srsran
