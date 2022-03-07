/// \file
/// \brief LDPC rate matching and rate recovering
#ifndef SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_RATE_MATCHING
#define SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_RATE_MATCHING

#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
namespace srsgnb {

/// LDPC rate matching (i.e., bit selection and bit interleaving) interface.
class ldpc_rate_matching
{
public:
  /// Default virtual destructor.
  virtual ~ldpc_rate_matching() = default;

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
  /// \param[out] output  Rate matched codeblock.
  /// \param[in]  input   Original, full codeblock (may contain filler bits).
  /// \param[in]  cfg     Configuration parameters.
  /// \remark The sizes of \c input and \c output determine the behavior of the rate matching algorithm.
  virtual void rate_match(span<uint8_t> output, span<const uint8_t> input, const config_t& cfg) = 0;
};

/// LDPC rate recovering interface. It reverts the operations of ldpc_rate_matching.
class ldpc_rate_recovering
{
public:
  /// Default virtual destructor.
  virtual ~ldpc_rate_recovering() = default;

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
  /// \param[out] output  Original, full codeblock (log-likelihood ratios).
  /// \param[in]  input   Rate-matched codeblock (log-likelihood ratios).
  /// \param[in]  cfg     Configuration parameters.
  /// \remark The sizes of \c input and \c output determine the behavior of the rate recovering algorithm.
  virtual void rate_recover(span<int8_t> output, span<const int8_t> input, const config_t& cfg) = 0;
};

std::unique_ptr<ldpc_rate_matching>   create_ldpc_rate_matching();
std::unique_ptr<ldpc_rate_recovering> create_ldpc_rate_recovering();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_CODING_LDPC_RATE_MATCHING
