
#ifndef SRSGNB_SSB_ASSEMBLER_H
#define SRSGNB_SSB_ASSEMBLER_H

#include "../../helpers/band_helper.h"
#include <cstddef>

#define BCH_PAYLOAD_GENERATION_OPTION 2

namespace srsgnb {

enum class ssb_pattern_case;
struct dl_ssb_pdu;
struct mac_cell_configuration;
struct ssb_information;

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
  void assemble_ssb(dl_ssb_pdu& ssb_pdu, const mac_cell_configuration& cell_cfg, const ssb_information& ssb_info);

  /// \brief Retrieve the SSB configuration.
  void set_ssb_configuration(const mac_cell_configuration& cell_cfg);

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
