
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

class ssb_assembler
{
public:
  ssb_assembler(){};

  /// Enqueue scheduler events.
  void assemble_ssb(dl_ssb_pdu& ssb_pdu, const mac_cell_configuration& cell_cfg, const ssb_information& ssb_info)
  {
    auto& ssb_cfg = cell_cfg.ssb_cfg;
    ssb_pdu.pci   = cell_cfg.pci;
    // TODO: ORAN only allows 2 values, 0 or 1. FAPI allows 3 values (0, 1, or 255)
    ssb_pdu.beta_pss_profile_nr = ssb_cfg.beta_pss;
    // TODO: because of ORAN to FAPI mismatch above, this will not be filled with meaningful values
    ssb_pdu.beta_pss_profile_sss  = 0;
    ssb_pdu.ssb_index             = ssb_info.ssb_index;
    ssb_pdu.ssb_subcarrier_offset = ssb_cfg.ssb_subcarrier_offset;
    ssb_pdu.offset_to_point_A     = ssb_cfg.ssb_offset_to_point_A;
    ssb_pdu.bch_payload_flag      = BCH_PAYLOAD_GENERATION_OPTION;
    ssb_pdu.ssb_case              = ssb_cfg.ssb_case;
    ssb_pdu.L_max                 = 4;
  }

  void set_ssb_configuration(const mac_cell_configuration& cell_cfg)
  {
    // At this point, we need to derive the some SSB parameters case from the available ones
    uint16_t dl_idx_nr_band = band_helper::get_band_from_dl_arfcn(cell_cfg.dl_carrier.arfcn);
    srsran_assert(dl_idx_nr_band < UINT16_MAX, "Invalid NR band index");
    ssb_case        = band_helper::get_ssb_pattern(dl_idx_nr_band, cell_cfg.ssb_scs);
    paired_spectrum = band_helper::is_paired_spectrum(dl_idx_nr_band);

    uint32_t f_arfcn = cell_cfg.dl_carrier.arfcn;
    if (ssb_case == ssb_pattern_case::A or ssb_case == ssb_pattern_case::B) {
      L_max = f_arfcn < CUTOFF_FREQ_ARFCN_CASE_A_B_C ? 4 : 8;
    }
    if (ssb_case == ssb_pattern_case::C) {
      uint32_t ssb_cutoff_freq = paired_spectrum ? CUTOFF_FREQ_ARFCN_CASE_A_B_C : CUTOFF_FREQ_ARFCN_CASE_C_UNPAIRED;
      L_max                    = f_arfcn < ssb_cutoff_freq ? 4 : 8;
    } else {
      srsran_assert(ssb_case < ssb_pattern_case::invalid, "Invalid SSB case");
    }
  }

  ssb_pattern_case get_ssb_case() const { return ssb_case; }
  uint8_t          get_ssb_L_max() const { return L_max; }
  bool             get_ssb_paired_spectrum() const { return paired_spectrum; }

private:
  ssb_pattern_case ssb_case;
  uint8_t          L_max;
  bool             paired_spectrum;
};

} // namespace srsgnb

#endif // SRSGNB_SSB_ASSEMBLER_H
