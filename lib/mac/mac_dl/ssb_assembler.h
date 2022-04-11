
#ifndef SRSGNB_SSB_ASSEMBLER_H
#define SRSGNB_SSB_ASSEMBLER_H

#include "../../helpers/band_helper.h"
#include "srsgnb/mac/cell_configuration.h"
#include "srsgnb/mac/mac_cell_result.h"
#include "srsgnb/mac/sched_result.h"
#include <cstddef>

#define BCH_PAYLOAD_GENERATION_OPTION 2

namespace srsgnb {

enum class ssb_pattern_case;

/// This class implements a helper to compute some SSB-specific parameters from the MAC's Cell configuration. These
/// parameters are passed to the scheduler and then used to assemble the SSB message to be sent to the PHY.
class ssb_assembler
{
public:
  /// \brief Assemble SSB message to be sent to PHY.
  /// This function fill the SSB msg to send to PHY using parameters from: (i) MAC configuration (general and SSB); (ii)
  /// SSB-specific dependent parameters; (iii) SSB scheduling results.
  /// \param[out] ssb_pdu SSB message to be sent to PHY.
  /// \param[in]  cell_cfg MAC's Cell configuration.
  /// \param[in]  ssb_info SSB scheduling results.
  void assemble_ssb(dl_ssb_pdu& ssb_pdu, const mac_cell_configuration& cell_cfg, const ssb_information& ssb_info)
  {
    auto& ssb_cfg = cell_cfg.ssb_cfg;
    ssb_pdu.pci   = cell_cfg.pci;
    // TODO: ORAN only allows 2 values, 0 or 1. FAPI allows 3 values (0, 1, or 255)
    ssb_pdu.beta_pss_profile_nr = 0;
    // TODO: because of ORAN to FAPI mismatch above, this will not be filled with meaningful values
    ssb_pdu.beta_pss_profile_sss = ssb_cfg.beta_pss;
    ssb_pdu.ssb_index            = ssb_info.ssb_index;
    // TODO: Verify whether this is the correct SCS
    ssb_pdu.scs                   = cell_cfg.ssb_cfg.scs;
    ssb_pdu.ssb_subcarrier_offset = ssb_cfg.ssb_subcarrier_offset;
    ssb_pdu.offset_to_point_A     = ssb_cfg.ssb_offset_to_point_A;
    ssb_pdu.bch_payload_flag      = BCH_PAYLOAD_GENERATION_OPTION;
    ssb_pdu.ssb_case              = ssb_case;
    ssb_pdu.L_max                 = L_max;

    /// Fields required for PBCH payload/MIB generation.
    // TODO: Understand where these parameters should be taken (They come from RRC MIB msg)
    ssb_pdu.mib_data.cell_barred            = false;
    ssb_pdu.mib_data.intra_freq_reselection = false;
    ssb_pdu.mib_data.dmrs_typeA_position    = 0;
    ssb_pdu.mib_data.pdcch_config_sib1      = 0;
  }

  void set_ssb_configuration(const mac_cell_configuration& cell_cfg)
  {
    // Derive the SSB-specific parameters (SSB pattern case, SSB L_max and SSB paired_spectrum flag) from those in the
    // MAC's Cell config.
    uint16_t dl_idx_nr_band = band_helper::get_band_from_dl_arfcn(cell_cfg.dl_carrier.arfcn);
    srsran_assert(dl_idx_nr_band < UINT16_MAX, "Invalid NR band index");
    ssb_case        = band_helper::get_ssb_pattern(dl_idx_nr_band, cell_cfg.ssb_scs);
    paired_spectrum = band_helper::is_paired_spectrum(dl_idx_nr_band);

    // Get L_max from SSB pattern case and carrier frequency and paired spectrum flag.
    uint32_t f_arfcn = cell_cfg.dl_carrier.arfcn;
    if (ssb_case == ssb_pattern_case::A or ssb_case == ssb_pattern_case::B) {
      // See TS 38.311, Section 4.1, "Case A" and "Case B".
      L_max = f_arfcn < CUTOFF_FREQ_ARFCN_CASE_A_B_C ? 4 : 8;
    }
    if (ssb_case == ssb_pattern_case::C) {
      // See TS 38.311, Section 4.1, "Case C".
      uint32_t ssb_cutoff_freq = paired_spectrum ? CUTOFF_FREQ_ARFCN_CASE_A_B_C : CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED;
      L_max                    = f_arfcn < ssb_cutoff_freq ? 4 : 8;
    } else {
      srsran_assert(ssb_case < ssb_pattern_case::invalid, "Invalid SSB case");
    }
  }

  /// Getters
  ssb_pattern_case get_ssb_case() const { return ssb_case; }
  uint8_t          get_ssb_L_max() const { return L_max; }
  bool             get_ssb_paired_spectrum() const { return paired_spectrum; }

private:
  /// SSB pattern case, see TS 38.213, Section 4.1. Possible values are {A, B, C, D, E}.
  ssb_pattern_case ssb_case;
  /// L_max, or max number of SSB occasions per SSB period. See TS 38.213, Section 4.1. Possible values are {4, 8, 64}.
  uint8_t          L_max;
  /// Flag indicating whether cell is on paired spectrum (FDD) or unpaired (TDD, SDL, SUL).
  /// NOTE: no reference to "Paired Spectrum" could be found in TS docs.
  bool             paired_spectrum;
};

} // namespace srsgnb

#endif // SRSGNB_SSB_ASSEMBLER_H
