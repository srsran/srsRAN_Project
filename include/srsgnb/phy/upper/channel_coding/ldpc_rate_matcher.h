/// \file
/// \brief LDPC rate matching.
#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_RATE_MATCHER_H
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_RATE_MATCHER_H

#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
namespace srsgnb {

/// LDPC rate matching (i.e., bit selection and bit interleaving) interface.
class ldpc_rate_matcher
{
public:
  /// Default virtual destructor.
  virtual ~ldpc_rate_matcher() = default;

  /// Rate matching configuration parameters.
  struct config_t {
    /// Redundancy version, values in {0, 1, 2, 3}.
    unsigned rv{0};
    /// Modulation scheme.
    modulation_scheme mod{modulation_scheme::BPSK};
    /// \brief Limited buffer rate matching length, as per TS38.212 Section 5.4.2.
    ///
    /// Set to zero for unlimited buffer length.
    unsigned Nref{0};
  };

  /// \brief Carries out the rate matching of a codeblock.
  ///
  /// \param[out] output  Rate matched codeblock. Each \c uint8_t entry corresponds to a single bit.
  /// \param[in]  input   Original, full codeblock (may contain filler bits). Each \c uint8_t entry corresponds to a
  ///                     single bit.
  /// \param[in]  cfg     Configuration parameters. \remark The sizes of \c input and \c output determine
  ///                     the behavior of the rate matching algorithm.
  virtual void rate_match(span<uint8_t> output, span<const uint8_t> input, const config_t& cfg) = 0;
};

std::unique_ptr<ldpc_rate_matcher> create_ldpc_rate_matcher();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_RATE_MATCHER_H
