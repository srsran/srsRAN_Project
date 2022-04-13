/// \file
/// \brief Demodulator implementation.

#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_MODULATION_DEMODULATION_MAPPER_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_MODULATION_DEMODULATION_MAPPER_IMPL_H

#include "srsgnb/phy/upper/channel_modulation/demodulation_mapper.h"

namespace srsgnb {

/// \brief Approximate-LLR demodulator.
///
/// Implementation of the interface provided by demodulation_mapper. Instead of the defining expression, the
/// log-likelihood ratios are computed using the widely accepted approximation
/// \f[
/// \ell(b_{m+i}) = -\frac{1}{\sigma_m^2} \biggl( \min_{s\in \mathcal{S}_{i,0}} \abs{y_m - s}^2
///               - \min_{s\in \mathcal{S}_{i,1}} \abs{y_m - s}^2 \biggr)
/// \f]
/// where \f$y_m\f$ denotes the noisy modulation symbol, \f$\sigma_m^2\f$ is the equivalent (after equalization) noise
/// variance and \f$\mathcal{S}_{i,B}\f$ is the set of constellation points for which \f$b_{m+i}=B\in\{0,1\}\f$.
class demodulation_mapper_impl : public demodulation_mapper
{
public:
  // See interface for the documentation.
  void demodulate_soft(span<int8_t>      llrs,
                       span<const cf_t>  symbols,
                       span<const float> noise_vars,
                       modulation_scheme mod) override;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_MODULATION_DEMODULATION_MAPPER_IMPL_H
