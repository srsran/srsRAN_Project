#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_PROCESSOR_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_PROCESSOR_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/ran/slot_point.h"
#include <srsgnb/phy/upper/channel_coding/ldpc/ldpc.h>

namespace srsgnb {

/// Describes the PDSCH processor interface.
class pdsch_processor
{
public:
  /// Defines the maximum number of codewords that can be encoded in a PDSCH transmission.
  static constexpr unsigned MAX_NOF_TRANSPORT_BLOCKS = 2;

  /// \brief Describes a codeword configuration.
  /// \note The transport block size is given by transport block data size.
  struct codeword_description {
    /// Target coding rate.
    float target_code_rate;
    /// Indicates the modulation scheme.
    modulation_scheme modulation;
    /// Modulation and code scheme index {0...31}.
    unsigned mcs;
    /// Indicates the MCS table used to determine the amount of redundancy.
    enum { QAM64 = 0, QAM256, QAM64LOWSE } mcs_table;
    /// Redundancy version index.
    unsigned rv;
  };

  /// \brief Describes the PDSCH processing parameters.
  struct pdu_t {
    /// Indicates the slot and numerology.
    slot_point slot;
    /// Provides \f$n_{RNTI}\f$ from TS 38.211 section 7.3.1.1 Scrambling.
    uint16_t rnti;
    /// Number of contiguous PRBs allocated to the BWP.
    unsigned bwp_size_rb;
    /// BWP start RB index from reference CRB.
    unsigned bwp_start_rb;
    /// Cyclic prefix type.
    cyclic_prefix cp;
    /// Provides codeword description.
    static_vector<codeword_description, MAX_NOF_TRANSPORT_BLOCKS> codewords;
    /// Higher layer parameter PDCCH-DMRS-ScramblingID if it is given, otherwise the physical cell identifier.
    unsigned n_id;
    /// Ports indexes the PDSCH transmission is mapped to. The number of ports indicates the number of layers.
    static_vector<uint8_t, MAX_PORTS> ports;
    /// Indicates the transmission scheme.
    enum {
      /// Transmission mode 1: up to 8 transmission layers on antenna ports 1000-1011 as defined in TS 38.211
      /// Clause 7.3.1.4
      TM1 = 0,
    };
    /// Indicates the reference point for PDSCH DMRS \c k.
    enum {
      /// When the reference point is subcarrier 0 of the common resource block 0 (CRB 0).
      CRB0,
      ///  When the reference point is subcarrier 0 of the physical resource block 0 of the bandwidth part (BWP). Use
      ///  this option when PDSCH is signalled by CORESET 0. For this case, the BWP parameters must align with CORESET0.
      PRB0
    } ref_point;
    /// Indicates which symbol in the slot transmit DMRS.
    std::array<bool, MAX_NSYMB_PER_SLOT> dmrs_symbol_mask;
    /// Indicates the DMRS type.
    dmrs_type dmrs;
    /// PDSCH DMRS-Scrambling-ID (pdschDmrsScramblingId).
    unsigned scrambling_id;
    /// DMRS sequence initialization (\f$n_{SCID}\f$).
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
    /// Ratio of PDSCH EPRE to NZP CSI-RS EPRE in decibels.
    float ratio_pdsch_to_ssb_dB;
    /// Ratio of NZP CSI-RS EPRE to SSB/PBCH block EPRE in decibels.
    float ratio_nzp_csi_rs_to_ssb_dB;
    // Ignore CBGs.
    // ...
    /// Indicates the transmission type of PDSCH.
    enum {
      /// Non-interleaved PDSCH which is scheduled by DCI format 1_0 in a common search space.
      NON_INTERLAVED_COMMON_SS = 0,
      /// Any non-interleaved PDSCH except above case.
      NON_INTERLAVED_OTHER = 1,
      /// Interleaved PDSCH which is scheduled by PDCCH DCI format 1_0 in Type0-PDCCH common search space in CORESET 0.
      /// In this case, BWP start and size of the PDSCH PDU shall be set to CORESET 0 start and size respectively
      /// instead of active downlink BWP start and size.
      INTERLEAVED_COMMON_TYPE0_CORESET0 = 2,
      /// Interleaved PDSCH which is scheduled by PDCCH DCI format 1_0 in any common search space other than Type0-PDCCH
      /// common search space in CORESET 0 when CORESET 0 is configured for the cell.
      INTERLEAVED_COMMON_ANY_CORESET0_PRESENT = 3,
      /// Interleaved PDSCH which is scheduled by PDCCH DCI format 1_0 in any common search space other than Type0 PDCCH
      /// common search space in CORESET 0 when CORESET 0 is not configured for the cell.
      INTERLEAVED_COMMON_ANY_CORESET0_NOT_PRESENT = 4,
      /// Any interleaved PDSCH which not fall into above 3 interleaved PDSCH categories.
      INTERLEAVED_OTHER = 5
    } transmission_type;
    /// The PRB index {0...274} of lowest-numbered RB in the CORESET in which PDCCH carrying scheduling info for the
    /// PDSCH PDU is received. Refer to TS 38.211 section 7.3.1.6 \f$N^{CORESET}_{start}\f$. Valid when \c
    /// transmission_type is set to cases 0, 3 or 4.
    unsigned coreset_start_rb;
    /// The size of initial downlink BWP {0,274} used for the cell, shall be set to size of CORESET 0 if CORESET 0 is
    /// configured for the cell, and initial downlink BWP otherwise. Refer to TS 38.211 section 7.3.1.6
    /// \f$N^{size}_{BWP,init}\f$. Valid when \c transmission_type is set to cases 3 or 4.
    unsigned initial_dl_bwp_size;
    /// LDPC base graph to use for CW generation.
    ldpc::base_graph_t ldpc_base_graph;
    /// Parameter \f$TBS_{LBRM}\f$ from 3GPP TS 38.212 section 5.4.2.1, for computing the size of the circular buffer.
    unsigned tbs_lbrm_bytes;
    // Ignore tbCrcRequired
    // ...
    /// Indicates the reserved resource elements which cannot carry PDSCH.
    re_pattern_list reserved;
    /// Ratio of PDSCH DMRS EPRE to SSS EPRE in decibels.
    float ratio_pdsch_dmrs_to_sss_dB;
    /// Ratio of PDSCH data EPRE to SSS EPRE in decibels.
    float ratio_pdsch_data_to_sss_dB;
  };

  /// Default destructor.
  virtual ~pdsch_processor() = default;

  /// \brief Processes a PDSCH transmission.
  ///
  /// \param[out] grid Provides the destination resource grid.
  /// \param[in] data Provides the codewords to transmit.
  /// \param[in] pdu Provides the necessary parameters to process the PDSCH transmission.
  virtual void process(resource_grid_writer&                                        grid,
                       static_vector<span<const uint8_t>, MAX_NOF_TRANSPORT_BLOCKS> data,
                       const pdu_t&                                                 pdu) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDSCH_PROCESSOR_H
