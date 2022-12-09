/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/support/resource_grid.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsgnb/phy/upper/channel_processors/pusch_processor_result.h"
#include "srsgnb/phy/upper/dmrs_mapping.h"
#include "srsgnb/phy/upper/rb_allocation.h"
#include "srsgnb/phy/upper/re_pattern.h"
#include "srsgnb/phy/upper/rx_softbuffer.h"
#include "srsgnb/ran/sch_mcs.h"
#include "srsgnb/ran/slot_point.h"
#include "srsgnb/ran/uci/uci_constants.h"

namespace srsgnb {

/// \brief Describes the PUSCH processor interface.
///
/// It performs the decoding of PUSCH described in TS38.211 Section 6.3.1.
///
class pusch_processor
{
public:
  /// \brief Collects specific codeword parameters.
  /// \note The transport block size is given by the transport block data size.
  struct codeword_description {
    /// Redundancy version index.
    unsigned rv;
    /// LDPC base graph to use for CW generation.
    ldpc_base_graph_type ldpc_base_graph;
    /// \brief Flags new data transmissions.
    ///
    /// Unlike the NDI field in DCI, it does not toggle to indicate new transmission. It is true for the first
    /// transmission of the HARQ process. Otherwise, it is false.
    bool new_data;
  };

  /// Collects Uplink Control Information parameters.
  struct uci_description {
    /// Number of HARQ-ACK information bits.
    unsigned nof_harq_ack;
    /// Number of CSI-Part1 information bits.
    unsigned nof_csi_part1;
    /// Number of CSI-Part2 information bits.
    unsigned nof_csi_part2;
    /// \brief Parameter \f$\alpha\f$ as per TS38.212 Section 6.3.2.4.
    ///
    /// Provided by the higher layer parameter \e scaling in TS38.331 Section 6.3.2, Information Element \e UCI-OnPUSCH.
    float alpha_scaling;
    /// Parameter \f$\beta _\textup{offset} ^\textup{HARQ-ACK}\f$ derived from TS38.213 Table 9.3-1.
    float beta_offset_harq_ack;
    /// Parameter \f$\beta _\textup{offset} ^\textup{CSI-1}\f$ derived from TS38.213 Table 9.3-2.
    float beta_offset_csi_part1;
    /// Parameter \f$\beta _\textup{offset} ^\textup{CSI-2}\f$ derived from TS38.213 Table 9.3-2.
    float beta_offset_csi_part2;
  };

  /// \brief Describes the PUSCH processing parameters.
  ///
  /// For a valid PUSCH transmission the codeword, the UCI information or both must be present.
  struct pdu_t {
    /// Indicates the slot and numerology.
    slot_point slot;
    /// Provides \f$n_{RNTI}\f$ from TS 38.211 section 6.3.1.1 Scrambling.
    uint16_t rnti;
    /// Number of contiguous PRBs allocated to the BWP {1...275}.
    unsigned bwp_size_rb;
    /// BWP start RB index from reference CRB {0...274}.
    unsigned bwp_start_rb;
    /// Cyclic prefix type.
    cyclic_prefix cp;
    /// Modulation and coding scheme.
    sch_mcs_description mcs_descr;
    /// Provides codeword description if present.
    optional<codeword_description> codeword;
    /// Uplink control information parameters.
    uci_description uci;
    /// \brief Parameter \f$n_{ID}\f$ from TS 38.211 section 6.3.1.1.
    ///
    /// It is equal to:
    /// - {0...1023} if the higher-layer parameter dataScramblingIdentityPUSCH if configured,
    /// - \f$N^{cell}_{ID}\f$ otherwise.
    unsigned n_id;
    /// Number of transmission layers as per TS38.211 Section 6.3.1.3.
    unsigned nof_tx_layers;
    /// Port indexes the PUSCH reception is mapped to.
    static_vector<uint8_t, MAX_PORTS> rx_ports;
    /// Indicates which symbol in the slot transmit DMRS.
    symbol_slot_mask dmrs_symbol_mask;
    /// Indicates the DMRS type.
    dmrs_type dmrs;
    /// \brief Parameter \f$N^{n_{SCID}}_{ID}\f$ TS 38.211 section 6.4.1.1.1.
    ///
    /// It is equal to:
    /// - {0,1, … ,65535} given by the higher-layer parameters scramblingID0 and scramblingID1,
    /// - \f$N^{cell}_{ID}\f$ otherwise.
    unsigned scrambling_id;
    /// \brief Parameter \f$n_{SCID}\f$ from TS 38.211 section 6.4.1.1.1.
    ///
    /// It is equal to:
    /// - \c true or \c false according DM-RS sequence initialization field, in the DCI associated with the PUSCH
    /// transmission if DCI format 0_1 is used,
    /// - \c false otherwise.
    bool n_scid;
    /// Number of DMRS CDM groups without data.
    unsigned nof_cdm_groups_without_data;
    /// Frequency domain allocation.
    rb_allocation freq_alloc;
    /// Time domain allocation start symbol index (0...12).
    unsigned start_symbol_index;
    /// Time domain allocation number of symbols (1...14).
    unsigned nof_symbols;
    /// \brief Limits codeblock encoding circular buffer in bytes.
    ///
    /// Parameter \f$TBS_{LBRM}\f$ from 3GPP TS 38.212 section 5.4.2.1, for computing the size of the circular buffer.
    /// \remark Use <tt> ldpc::MAX_CODEBLOCK_SIZE / 8 </tt> for maximum length.
    /// \remark Zero is reserved.
    unsigned tbs_lbrm_bytes;
  };

  /// Default destructor.
  virtual ~pusch_processor() = default;

  /// \brief Processes a PUSCH transmission.
  ///
  /// The size of each transport block is determined by <tt> data[TB index].size() </tt>.
  ///
  /// \param[out] data Provides the transport block to receive.
  /// \param[in,out] softbuffer Provides the data reception softbuffer.
  /// \param[in] grid Provides the destination resource grid.
  /// \param[in] pdu Provides the necessary parameters to process the PUSCH transmission.
  virtual pusch_processor_result
  process(span<uint8_t> data, rx_softbuffer& softbuffer, const resource_grid_reader& grid, const pdu_t& pdu) = 0;
};

/// \brief Describes the PUSCH processor validator interface.
class pusch_pdu_validator
{
public:
  /// Default destructor.
  virtual ~pusch_pdu_validator() = default;

  /// \brief Validates PUSCH processor configuration parameters.
  /// \return True if the parameters contained in \c pdu are supported, false otherwise.
  virtual bool is_valid(const pusch_processor::pdu_t& pdu) const = 0;
};

} // namespace srsgnb

namespace fmt {

/// \brief Custom formatter for optional<pusch_processor::codeword_description>.
template <>
struct formatter<srsgnb::optional<srsgnb::pusch_processor::codeword_description>> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::optional<srsgnb::pusch_processor::codeword_description>& codeword, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    if (!codeword.has_value()) {
      return format_to(ctx.out(), "{{na}}");
    }
    return format_to(ctx.out(),
                     "{{rv={} bg={} ndi={}}}",
                     codeword.value().rv,
                     codeword.value().ldpc_base_graph,
                     codeword.value().new_data);
  }
};

/// \brief Custom formatter for pusch_processor::uci_description.
template <>
struct formatter<srsgnb::pusch_processor::uci_description> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pusch_processor::uci_description& uci_desc, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "oack={} ocsi1={} ocsi2={} alpha={} betas=[{}, {}, {}]",
                     uci_desc.nof_harq_ack,
                     uci_desc.nof_csi_part1,
                     uci_desc.nof_csi_part2,
                     uci_desc.alpha_scaling,
                     uci_desc.beta_offset_harq_ack,
                     uci_desc.beta_offset_csi_part1,
                     uci_desc.beta_offset_csi_part2);
  }
};

/// \brief Custom formatter for pusch_processor::pdu_t.
template <>
struct formatter<srsgnb::pusch_processor::pdu_t> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::pusch_processor::pdu_t& pdu, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return format_to(ctx.out(),
                     "slot={} rnti=0x{:04x} bwp={}:{} cp={} mod={} tcr={} cw={} uci={} n_id={} dmrs=[{}] n_scidid={} "
                     "n_scid={} ncgwd={} f_alloc={} t_alloc={}:{} ",
                     pdu.slot,
                     pdu.rnti,
                     pdu.bwp_start_rb,
                     pdu.bwp_size_rb,
                     pdu.cp.to_string(),
                     to_string(pdu.mcs_descr.modulation),
                     pdu.mcs_descr.target_code_rate,
                     pdu.codeword,
                     pdu.uci,
                     pdu.n_id,
                     pdu.dmrs_symbol_mask,
                     pdu.scrambling_id,
                     pdu.n_scid,
                     pdu.nof_cdm_groups_without_data,
                     pdu.freq_alloc,
                     pdu.start_symbol_index,
                     pdu.nof_symbols);
  }
};
} // namespace fmt
