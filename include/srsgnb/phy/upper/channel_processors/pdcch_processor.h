#ifndef SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_PROCESSOR_H
#define SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_PROCESSOR_H

#include "srsgnb/adt/static_vector.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/phy/cyclic_prefix.h"
#include "srsgnb/phy/resource_grid.h"
#include "srsgnb/ran/slot_point.h"

namespace srsgnb {

/// Describes the PDCCH processor interface.
class pdcch_processor
{
public:
  /// Defines the number if resource blocks per frequency resource in a CORESET.
  static constexpr unsigned NOF_RB_PER_FREQ_RESOURCE = 6;

  /// Define the maximum number of frequency resources in a CORESET.
  static constexpr unsigned MAX_NOF_FREQ_RESOUCES = MAX_RB / 6;

  /// Define the maximum number of DCI in a PDCCH transmission.
  static constexpr unsigned MAX_NOF_DCI = 1;

  /// Define the maximum payload size in bits.
  static constexpr unsigned MAX_DCI_PAYLOAD_SIZE = 128;

  /// Describes a DCI transmission.
  struct dci_description {
    /// Parameter \f$n_{ID}\f$ used for DMRS scrambling as per TS 38.211 section 7.4.1.3.1 {0...65535}.
    unsigned n_id_pdcch_dmrs;
    /// Parameter \f$n_{ID}\f$ used in data scrambling in TS 38.211 section 7.3.2.3 {0...65535}.
    unsigned n_id_pdcch_data;
    /// Parameter \f$n_{RNTI}\f$ used for PDCCH data scrambling in TS 38.211 section 7.3.2.3 {1...65535}.
    unsigned n_rnti;
    /// CCE start index used to send the DCI {0, 135}.
    unsigned cce_index;
    /// Aggregation level used as per TS 38.211 section 7.3.2.1 {1,2,4,8,16}.
    unsigned aggregation_level;
    // Ignore precoding and beamforming.
    // ...
    /// Ratio of PDCCH Data EPRE to SSS EPRE in decibels.
    float dmrs_power_offset_dB;
    /// Ratio of PDCCH Data EPRE to SSS EPRE in decibels.
    float data_power_offset_dB;
    /// DCI payload as unpacked bits.
    static_vector<uint8_t, MAX_DCI_PAYLOAD_SIZE> payload;
    /// Port indexes the PDCCH transmission is mapped onto.
    static_vector<uint8_t, MAX_PORTS> ports;
  };

  /// \brief Describes CORESET parameters.
  ///
  /// Describes CORESET parameters.
  ///
  /// For  \c controlResourceSetZero (CORESET 0) there are some exceptions:
  /// - \c type shall be set to \c CORESET0.
  /// - \c bwp_start_rb and \c bwp_size_rb indicate the CORESET start point and contiguous resource blocks.
  /// - \c shift_index shall be equal to the physical cell identifier {0...1007}.
  /// - \c cce_to_reg_mapping_type, \c frequency_resources, \c reg_bundle_size, \c interleaver_size are ignored.
  struct coreset_description {
    /// Number of contiguous PRBs allocated to the BWP {1...275}.
    unsigned bwp_size_rb;
    /// BWP start RB index from reference CRB {0...274}.
    unsigned bwp_start_rb;
    /// Starting OFDM symbol for the CORESET.
    unsigned start_symbol_index;
    /// Contiguous time duration of the CORESET in number of symbols {1,2,3}. Corresponds to L1 parameter
    /// \f$N^{CORESET}_{symb}\f$ as per RS 38.211 section 7.3.2.2.
    unsigned duration;
    /// Frequency domain resources. This is a bitmap defining non-overlapping groups of 6 PRBs in ascending order.
    std::array<bool, MAX_NOF_FREQ_RESOUCES> frequency_resources;
    /// CORESET-CCE-to-REG-mapping-type as per TS 38.211 section 7.3.2.2.
    enum { NON_INTERLEAVED = 0, INTERLEAVED } cce_to_reg_mapping_type;
    /// The number of REGs in a bundle. Ignored if \c cce_to_reg_mapping_type is set to \c NON_INTERLEAVED. Otherwise,
    /// it must be {2,6} for duration of 1 or 2 symbols and {3,6} for duration of 3 symbols.
    unsigned reg_bundle_size;
    /// The interleaver size. For interleaved mapping belongs to {2,3,6} and for non-interleaved mapping is ignored.
    /// Corresponds to parameter R. in TS 38.211 7.3.2.2.
    unsigned interleaver_size;
    /// CORESET type defines the reference point for k in TS 38.211 section 7.4.1.3.2.
    enum {
      /// CORESET is configured by the PBCH or SIB1, (subcarrier 0 of the CORESET)
      CORESET0 = 0,
      /// otherwise (subcarrier 0 of CRB0 for DMRS mapping)
      OTHER
    } type;
    /// Shift index \f$n_{shift}\f$ in TS 38.211 section 7.3.2.2. It should be set to {0,275}, except for CORESET 0.
    unsigned shift_index;
    /// Granularity of precoding as per TS 38.211 section 7.3.2.2.
    enum { SAME_AS_REG_BUNDLE = 0, ALL_CONTIGUOUS_RBS } precoder_granularity;
  };

  /// Describes the PDCCH processing parameters.
  struct pdu_t {
    /// Indicates the slot and numerology.
    slot_point slot;
    /// Cyclic prefix type.
    cyclic_prefix cp;
    /// Provides CORESET description.
    coreset_description coreset;
    /// List of DCI.
    static_vector<dci_description, MAX_NOF_DCI> dci_list;
  };

  /// Default detsructor.
  virtual ~pdcch_processor() = default;

  /// \brief Processes a PDCCH transmission.
  ///
  /// \param[out] grid Provides the destination resource grid.
  /// \param[in] pdu Provides the necessary parameters to process the PDCCH transmission.
  virtual void process(resource_grid_writer& grid, pdu_t& pdu) = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_PROCESSORS_PDCCH_PROCESSOR_H
