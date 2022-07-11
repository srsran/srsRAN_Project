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
#include "srsgnb/phy/modulation_scheme.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsgnb/phy/upper/dmrs_mapping.h"
#include "srsgnb/phy/upper/rb_allocation.h"
#include "srsgnb/phy/upper/re_pattern.h"
#include "srsgnb/phy/upper/rx_softbuffer.h"
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
  /// \brief Maximum number of resource elements per resource block in a shared channel transmission.
  ///
  /// As per TS38.214 section 6.1.4.2 Transport block size determination.
  static constexpr unsigned MAX_NRE_PER_RB = 156;

  /// \brief Describes a codeword configuration.
  /// \note The transport block size is given by the transport block data size.
  struct codeword_description {
    /// Indicates the modulation scheme.
    modulation_scheme modulation;
    /// Redundancy version index.
    unsigned rv;
    /// LDPC base graph to use for CW generation.
    ldpc_base_graph_type ldpc_base_graph;
    /// Flags new data transmissions.
    bool new_data;
  };

  /// Describes UCI transmission parameters.
  struct uci_description {
    /// HARQ-ACK payload.
    bounded_bitset<uci_constants::MAX_NOF_HARQ_ACK_BITS> harq_ack;
    /// CSI Part1 payload.
    bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_BITS> csi_part1;
    /// CSI Part2 payload.
    bounded_bitset<uci_constants::MAX_NOF_CSI_PART1_BITS> csi_part2;
    /// Parameter \f$\alpha\f$ as per TS38.212 Section 6.3.2.4.
    float scaling;
    /// Beta offset index for HARQ-ACK bits to derive the parameter \f$O_{ACK}\f$ in TS38.212 Section 6.3.2.4.1.1 from
    /// TS38.213 Table 9.3-1.
    unsigned beta_offset_harq_ack;
    /// Beta offset index for CSI Part 1 bits to derive the parameter \f$O_{CSI-1}\f$ in TS38.212 Section 6.3.2.4.1.2
    /// from TS38.213 Table 9.3-2.
    unsigned beta_offset_csi_part1;
    /// Beta offset index for CSI Part 2 bits to derive the parameter \f$O_{CSI-2}\f$ in TS38.212 Section 6.3.2.4.1.2
    /// from TS38.213 Table 9.3-2.
    unsigned beta_offset_csi_part2;
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
    /// Provides codeword description if present.
    optional<codeword_description> codeword;
    /// Provides UCI if present.
    optional<uci_description> uci;
    /// \brief Parameter \f$n_{ID}\f$ from TS 38.211 section 6.3.1.1.
    ///
    /// It is equal to:
    /// - {0...1023} if the higher-layer parameter dataScramblingIdentityPUSCH if configured,
    /// - \f$N^{cell}_{ID}\f$ otherwise.
    unsigned n_id;
    /// Port indexes the PUSCH transmission is mapped to. The number of ports indicates the number of layers.
    static_vector<uint8_t, MAX_PORTS> ports;
    // Ignores the transmission scheme.
    // ...
    /// Indicates the reference point for PUSCH DMRS \c k.
    enum {
      /// When the reference point is subcarrier 0 of the common resource block 0 (CRB 0).
      CRB0,
      ///  When the reference point is subcarrier 0 of the physical resource block 0 of the bandwidth part (BWP). Use
      ///  this option when PUSCH is signalled by CORESET 0. For this case, the BWP parameters must align with CORESET0.
      PRB0
    } ref_point;
    /// Indicates which symbol in the slot transmit DMRS.
    std::array<bool, MAX_NSYMB_PER_SLOT> dmrs_symbol_mask;
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
    /// Indicates the reserved resource elements which cannot carry PUSCH.
    re_pattern_list reserved;
    /// Ratio of PUSCH DMRS EPRE to SSS EPRE in decibels.
    float ratio_pusch_dmrs_to_sss_dB;
    /// Ratio of PUSCH data EPRE to SSS EPRE in decibels.
    float ratio_pusch_data_to_sss_dB;
  };

  /// Describes channel state information from the received DMRS.
  struct csi_indication {
    /// Time alignment measurement in microseconds.
    float time_aligment_us;
    /// Average EPRE in decibels relative to 1.
    float epre_dB;
    /// Average RSRP in decibels relative to 1.
    float rsrp_dB;
    /// Average SINR in decibels.
    float sinr_dB;
  };

  /// Describes the data information.
  struct data_indication {
    /// Set to true
    bool crc;
    /// Average number of iterations.
    float average_iterations;
  };

  /// Describes the UCI information.
  struct uci_indication {
    // TBD.
  };

  /// Groups the PUSCH processor outcome.
  struct outcome {
    /// CSI from DMRS.
    csi_indication csi;
    /// SCH decoding information
    data_indication data;
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
  virtual void
  process(span<uint8_t> data, rx_softbuffer& softbuffer, const resource_grid_reader& grid, const pdu_t& pdu) = 0;
};

} // namespace srsgnb
