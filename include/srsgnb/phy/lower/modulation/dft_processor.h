
#ifndef SRSGNB_PHY_LOWER_MODULATION_DFT_PROCESSOR_H
#define SRSGNB_PHY_LOWER_MODULATION_DFT_PROCESSOR_H

#include "srsgnb/adt/complex.h"
#include "srsgnb/adt/span.h"

namespace srsgnb {

/// \brief Describes a Discrete Fourier Transform (DFT) processor.
/// \remark The DFT processor allocates the input and output buffers for optimization purposes.
/// \remark The input buffer is available using get_input().
/// \remark The output buffer is available using run().
class dft_processor
{
public:
  /// Indicates the DFT direction.
  enum class direction {
    /// \brief Converts a time domain signal into frequency domain.
    /// \remark For the FFTW based libraries, it is equivalent to \c FFTW_FORWARD sign.
    DIRECT = 0,
    /// \brief Converts a frequency domain signal into frequency (inverse DFT).
    /// \remark For the FFTW based libraries, it is equivalent to \c FFTW_BACKWARD sign.
    INVERSE
  };

  /// Convert a DFT direction to string.
  static std::string direction_to_string(direction dir) { return dir == direction::DIRECT ? "direct" : "inverse"; }

  /// Default destructor.
  virtual ~dft_processor() = default;

  /// \brief Get the DFT direction.
  virtual direction get_direction() const = 0;

  /// \brief Get the DFT number of points.
  virtual unsigned get_size() const = 0;

  /// \brief Get a view of the internal input DFT buffer.
  virtual span<cf_t> get_input() const = 0;

  /// \brief Execute the DFT from the internal input data.
  /// \return A view of the internal output DFT buffer.
  virtual span<const cf_t> run() = 0;
};

} // namespace srsgnb

#endif // SRSGNB_PHY_LOWER_MODULATION_DFT_PROCESSOR_H
