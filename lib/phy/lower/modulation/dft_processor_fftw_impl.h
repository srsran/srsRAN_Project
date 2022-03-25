
#ifndef LIB_PHY_LOWER_MODULATION_DFT_PROCESSOR_FFTW_IMPL_H_
#define LIB_PHY_LOWER_MODULATION_DFT_PROCESSOR_FFTW_IMPL_H_

#include "srsgnb/phy/lower/modulation/dft_processor.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include <fftw3.h>
#include <memory>
#include <mutex>

namespace srsgnb {

/// Provides to DFT processor class based on FFTW library with the necessary initialization parameters.
struct dft_processor_fftw_config {
  /// DFT size in number of points.
  unsigned size;
  /// DFT direction, see attribute type for more documentation.
  dft_processor::direction direction;
  /// Set to true to avoid loading the FFTW wisdom from a file.
  bool avoid_wisdom;
  /// Provide the FFTW wisdom filename. Leave empty for default value and ignore if wisdom is disabled.
  std::string wisdom_filename;
};

/// Describes a DFT processor class configuration based on FFTW library.
class dft_processor_fftw_impl : public dft_processor
{
private:
  /// Protects the FFTW library from simultaneous plan creation.
  static std::mutex mutex_init;
  /// If wisdom was loaded earlier by any instance, it contains the FFTW wisdom file name. Otherwise, it is empty.
  static std::string wisdom_filename;
  /// Counts the number of FFTW instances.
  static unsigned fftw_count;
  /// Stores the DFT direction.
  direction dir;
  /// DFT input buffer ownership.
  srsvec::aligned_vec<cf_t> input;
  /// DFT output buffer ownership.
  srsvec::aligned_vec<cf_t> output;
  /// FFTW actual plan.
  fftwf_plan plan;

  /// Get the default FFTW wisdom file.
  static std::string get_default_fftw_wisdom_file();

public:
  /// \brief Constructs a DFT processor based on FFTW library.
  ///
  /// Constructs a DFT processor based on FFTW library. The FFTW wisdom is loaded with the first instance.
  ///
  /// \param [in] config Provides the required parameters to create the FFTW plan.
  dft_processor_fftw_impl(const dft_processor_fftw_config& config);

  /// Frees the allocated resources and save the wisdom if it is the first instance to be destroyed.
  ~dft_processor_fftw_impl();

  // See interface for documentation.
  direction get_direction() const override { return dir; }

  // See interface for documentation.
  unsigned int get_size() const override { return input.size(); }

  // See interface for documentation.
  span<cf_t> get_input() const override { return input; }

  // See interface for documentation.
  span<const cf_t> run() override;
};

/// \brief Create a DFT processor based on FFTW library.
/// \param [in] config Provides the required parameters to create the FFTW plan.
/// \return A unque pointer of the created instance.
std::unique_ptr<dft_processor> create_dft_processor_fftw(const dft_processor_fftw_config& config);

} // namespace srsgnb
#endif // LIB_PHY_LOWER_MODULATION_DFT_PROCESSOR_FFTW_IMPL_H_
