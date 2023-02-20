/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/static_vector.h"
#include "srsran/phy/support/mask_types.h"
#include "srsran/phy/support/resource_grid.h"
#include "srsran/phy/upper/channel_coding/ldpc/ldpc.h"
#include "srsran/phy/upper/dmrs_mapping.h"
#include "srsran/phy/upper/rb_allocation.h"
#include "srsran/phy/upper/re_pattern.h"
#include "srsran/ran/modulation_scheme.h"
#include "srsran/ran/pdsch/pdsch_context.h"
#include "srsran/ran/slot_point.h"

namespace srsran {

/// Describes the PDSCH processor interface.
class pdsch_processor
{
public:
  /// \brief Maximum number of resource elements per resource block in a shared channel transmission.
  /// As per TS 38.214 section 5.1.3.2 Transport block size determination.
  static constexpr unsigned MAX_NRE_PER_RB = 156;
  /// Defines the maximum number of codewords that can be encoded in a PDSCH transmission.
  static constexpr unsigned MAX_NOF_TRANSPORT_BLOCKS = 2;
  /// \brief Defines the maximum number of layers per codeword.
  /// As per TS 38.211 table Table 7.3.1.3-1: Codeword-to-layer mapping for spatial multiplexing.
  static constexpr unsigned MAX_LAYER_PER_CODEWORD = 4;
  /// Defines the maximum codeword size.
  static constexpr unsigned MAX_CODEWORD_SIZE =
      MAX_RB * MAX_NRE_PER_RB * MODULATION_MAX_BITS_PER_SYMBOL * MAX_LAYER_PER_CODEWORD;

  /// \brief Describes a codeword configuration.
  /// \note The transport block size is given by the transport block data size.
  struct codeword_description {
    /// Indicates the modulation scheme.
    modulation_scheme modulation;
    /// Redundancy version index.
    unsigned rv;
  };

  /// \brief Describes the PDSCH processing parameters.
  struct pdu_t {
    /// Context information.
    optional<pdsch_context> context;
    /// Indicates the slot and numerology.
    slot_point slot;
    /// Provides \f$n_{RNTI}\f$ from TS 38.211 section 7.3.1.1 Scrambling.
    uint16_t rnti;
    /// Number of contiguous PRBs allocated to the BWP {1, ..., 275}.
    unsigned bwp_size_rb;
    /// BWP start RB index from Point A {0, ..., 274}.
    unsigned bwp_start_rb;
    /// Cyclic prefix type.
    cyclic_prefix cp;
    /// Provides codeword description.
    static_vector<codeword_description, MAX_NOF_TRANSPORT_BLOCKS> codewords;
    /// \brief Parameter \f$n_{ID}\f$ from TS 38.211 section 7.3.1.1.
    ///
    /// It is equal to:
    /// - {0...1023} if the higher-layer parameter dataScramblingIdentityPDSCH if configured,
    /// - \f$N^{cell}_{ID}\f$ otherwise.
    unsigned n_id;
    /// Port indexes the PDSCH transmission is mapped to. The number of ports indicates the number of layers.
    static_vector<uint8_t, MAX_PORTS> ports;
    // Ignores the transmission scheme.
    // ...
    /// \brief Reference point for the PDSCH DM-RS subcarrier index \e k, as per TS38.211 Section 7.4.1.1.2.
    ///
    /// Set it to \c PRB0 when the corresponding PDCCH is associated with CORESET 0 and Type0-PDCCH common search space
    /// and is addressed to SI-RNTI.
    ///
    /// Otherwise, set it to \c CRB0.
    enum {
      /// The reference point is subcarrier 0 of the common resource block 0 (CRB 0).
      CRB0,
      /// \brief The reference point is subcarrier 0 of the physical resource block 0 of the bandwidth part (BWP).
      ///
      /// Use this option when PDSCH is signalled by CORESET 0. For this case, the BWP parameters must align with
      /// CORESET0.
      PRB0
    } ref_point;
    /// Indicates which symbol in the slot transmit DMRS.
    symbol_slot_mask dmrs_symbol_mask;
    /// Indicates the DMRS type.
    dmrs_type dmrs;
    /// \brief Parameter \f$N^{n_{SCID}}_{ID}\f$ TS 38.211 section 7.4.1.1.1.
    ///
    /// It is equal to:
    /// - {0,1, … ,65535} given by the higher-layer parameters scramblingID0 and scramblingID1,
    /// - \f$N^{cell}_{ID}\f$ otherwise.
    unsigned scrambling_id;
    /// \brief Parameter \f$n_{SCID}\f$ from TS 38.211 section 7.4.1.1.1.
    ///
    /// It is equal to:
    /// - \c true or \c false according DM-RS sequence initialization field, in the DCI associated with the PDSCH
    /// transmission if DCI format 1_1 is used,
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
    // Ignore PTRS.
    // ...
    // Ignore precoding and beamforming.
    // ...
    // Ignore CBGs.
    // ...
    /// LDPC base graph to use for CW generation.
    ldpc_base_graph_type ldpc_base_graph;
    /// \brief Limits codeblock encoding circular buffer in bytes.
    ///
    /// Parameter \f$TBS_{LBRM}\f$ from 3GPP TS 38.212 section 5.4.2.1, for computing the size of the circular buffer.
    /// \remark Use <tt> ldpc::MAX_CODEBLOCK_SIZE / 8 </tt> for maximum length.
    /// \remark Zero is reserved.
    unsigned tbs_lbrm_bytes;
    /// Indicates the reserved resource elements which cannot carry PDSCH.
    re_pattern_list reserved;
    /// \brief Ratio of PDSCH DM-RS EPRE to SSS EPRE in decibels.
    ///
    /// Parameter \f$\beta _\textup{DMRS}\f$ in TS38.214 Section 6.2.2. It is converted to parameter \f$\beta _{PUSCH}
    /// ^{DMRS}\f$ in TS38.211 Section 7.4.1.1.2 as \f$\beta _\textup{PUSCH} ^\textup{DMRS}=10^{-\frac {\beta
    /// _\textup{DMRS}}{20} }\f$.
    float ratio_pdsch_dmrs_to_sss_dB;
    /// Ratio of PDSCH data EPRE to SSS EPRE in decibels.
    float ratio_pdsch_data_to_sss_dB;
  };

  /// Default destructor.
  virtual ~pdsch_processor() = default;

  /// \brief Processes a PDSCH transmission.
  /// \param[out] grid Provides the destination resource grid.
  /// \param[in] data Provides the codewords to transmit.
  /// \param[in] pdu Provides the necessary parameters to process the PDSCH transmission.
  /// \remark The number of transport blocks must be equal to the number of codewords in \c pdu.
  /// \remark The size of each transport block is determined by <tt> data[TB index].size() </tt>
  virtual void process(resource_grid_writer&                                        grid,
                       static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
                       const pdu_t&                                                 pdu) = 0;
};

/// \brief Describes the PDSCH processor validator interface.
class pdsch_pdu_validator
{
public:
  /// Default destructor.
  virtual ~pdsch_pdu_validator() = default;

  /// \brief Validates PDSCH processor configuration parameters.
  /// \return True if the parameters contained in \c pdu are supported, false otherwise.
  virtual bool is_valid(const pdsch_processor::pdu_t& pdu) const = 0;
};

} // namespace srsran
