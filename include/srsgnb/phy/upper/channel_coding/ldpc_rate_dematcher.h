/// \file
/// \brief LDPC rate dematching.
#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_RATE_DEMATCHING
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_RATE_DEMATCHING

#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
namespace srsgnb {

/// LDPC rate dematcher interface. It reverts the operations of ldpc_rate_matcher.
class ldpc_rate_dematcher
{
public:
  /// Default virtual destructor.
  virtual ~ldpc_rate_dematcher() = default;

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

  /// \brief Recovers a full codeblock from its rate-matched version.
  ///
  /// \param[out] output          Original, full codeblock (log-likelihood ratios).
  /// \param[in]  input           Rate-matched codeblock (log-likelihood ratios).
  /// \param[in]  nof_filler_bits Number of filler bits in the full codeblock.
  /// \param[in]  cfg             Configuration parameters.
  /// \remark The sizes of \c input and \c output determine the behavior of the rate recovering algorithm.
  virtual void
  rate_dematch(span<int8_t> output, span<const int8_t> input, unsigned nof_filler_bits, const config_t& cfg) = 0;
};

std::unique_ptr<ldpc_rate_dematcher> create_ldpc_rate_dematcher();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_RATE_DEMATCHING
