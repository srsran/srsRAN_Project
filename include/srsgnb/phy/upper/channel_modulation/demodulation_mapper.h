/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

/// \file
/// \brief Demodulator interface.

#pragma once

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/modulation_scheme.h"
#include "srsgnb/phy/upper/log_likelihood_ratio.h"
#include <memory>

namespace srsgnb {

/// \brief Demodulator interface.
///
/// The demodulation mapper reverts the operations described in TS38.211 Section 5.1. It takes as input a sequence of
/// noisy, complex-valued modulation symbols and it returns a sequence of log-likelihood ratios (soft bits). More
/// specifically, the \f$m\f$th modulation symbol will be mapped to the soft bits
/// \f[
/// \ell(b_{m+i}) = \log \frac{\Pr(b_{m+i}=0|y_m)}{\Pr(b_{m+i}=1|y_m)}
/// \f]
/// where \f$i = 0,1,\dots,Q_{\mathrm{m}}-1\f$, with \f$Q_{\mathrm{m}}\f$ denoting the modulation order as defined in
/// TS38.211 Table 6.3.1.2-1, and where \f$\Pr(b_{m+i}=B|y_m)\f$ is the conditional probability of bit \f$b_{m+i}\f$
/// being equal to \f$B\f$ given the modulation symbol \f$y_m\f$.
class demodulation_mapper
{
public:
  /// Default destructor.
  virtual ~demodulation_mapper() = default;

  /// \brief Soft demodulation.
  ///
  /// Demodulates complex symbols into log-likelihood ratios (i.e., soft bits).
  /// \param[out] llrs       Demodulated log-likelihood ratios.
  /// \param[in]  symbols    Complex symbols to demodulate (equalization output).
  /// \param[in]  noise_vars Noise variances (after equalization).
  /// \param[in]  mod        Modulation scheme.
  /// \remark Parameters \c symbols and \c noise_vars must have the same length. Each entry of \c noise_vars
  ///         contains the (estimated) noise variance after equalization corresponding to the symbol with the same
  ///         index.
  /// \remark The length of \c llrs is given by the number of symbols multiplied by the modulation order of \c mod.
  virtual void demodulate_soft(span<log_likelihood_ratio> llrs,
                               span<const cf_t>           symbols,
                               span<const float>          noise_vars,
                               modulation_scheme          mod) = 0;
};

std::unique_ptr<demodulation_mapper> create_demodulation_mapper();

} // namespace srsgnb
