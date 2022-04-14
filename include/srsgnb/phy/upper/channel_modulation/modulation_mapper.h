#ifndef SRSGNB_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_H
#define SRSGNB_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"
#include "srsgnb/phy/modulation_scheme.h"
#include <complex>
#include <memory>

namespace srsgnb {

/// \brief Modulation mapper public interface.
///
/// Maps bits to complex symbols as specified in TS38.211 Section 5.1.
class modulation_mapper
{
public:
  /// Default destructor.
  virtual ~modulation_mapper() = default;

  /// \brief Maps a sequence of bits  to a sequence of complex symbols according to the given modulation scheme.
  /// \param[in]  input   The sequence of bits. Each entry corresponds to a single bit.
  /// \param[out] symbols The resulting sequence of complex-valued symbols.
  /// \param[in]  scheme  The modulation scheme determining the mapping.
  /// \remark The length of \c input should be equal to the length of \c symbols times the modulation order (that is,
  /// the number of bits per modulated symbol).
  virtual void modulate(span<const uint8_t> input, span<cf_t> symbols, modulation_scheme scheme) = 0;
};

/// Creates a modulation mapper.
std::unique_ptr<modulation_mapper> create_modulation_mapper();

} // namespace srsgnb

#endif // SRSGNB_PHY_UPPER_CHANNEL_MODULATION_MODULATION_MAPPER_H
