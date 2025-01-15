/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/bounded_integer.h"
#include <variant>

namespace srsran {

/// \brief Subset of PMIs addressed by TPMI, where PMIs are those supported by UEs with maximum coherence capabilities.
///
/// The UE maximum coherence capability is given by TS38.331 higher layer capability parameter \e pusch-TransCoherence
/// in TS38.331 Section 6.3.2, Information Element \e MIMO-ParametersPerBand.
///
/// The PUSCH transmission scheme codebook subset is given by the higher layer parameter \e codebookSubset in TS38.331
/// Section 6.3.2, Information Element \e PUSCH-Config.
///
/// See TS38.214, Section 6.1.1.1 for related physical layer procedures.
enum class tx_scheme_codebook_subset : unsigned {
  fully_and_partial_and_non_coherent,
  partial_and_non_coherent,
  non_coherent
};

/// \defgroup tx_schemes PUSCH transmission schemes defined in TS 38.214 Section 6.1.1.
/// @{
/// \brief Codebook based transmit scheme.
struct tx_scheme_codebook {
  /// \brief Maximum number of layers in PUSCH. Values {1,..,4}.
  ///
  /// The parameter is given by the field \e maxRank in the TS 38.331 Section 6.3.2, Information Element \e
  /// PUSCH-Config.
  bounded_integer<uint8_t, 1, 4> max_rank;
  /// \brief PUSCH precoding codebook subset.
  ///
  /// Restricts the available PMIs to those supported by the UE, according to its coherence capabilities. Its value is
  /// indicated by the higher layer parameter \e codebookSubset (see TS38.331 Section 6.3.2, Information Element \e
  /// PUSCH-Config).
  tx_scheme_codebook_subset codebook_subset;

  bool operator==(const tx_scheme_codebook& rhs) const
  {
    return rhs.max_rank == max_rank && codebook_subset == rhs.codebook_subset;
  }
};
/// Non-codebook based transmission scheme.
struct tx_scheme_non_codebook {
  // Empty.

  bool operator==(const tx_scheme_non_codebook& rhs) const { return true; }
};
/// @}

/// PUSCH transmit scheme configuration.
using pusch_tx_scheme_configuration = std::variant<tx_scheme_codebook, tx_scheme_non_codebook>;

} // namespace srsran
