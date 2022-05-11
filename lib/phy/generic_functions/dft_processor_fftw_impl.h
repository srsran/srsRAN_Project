/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef LIB_PHY_GENERIC_FUNCTIONS_DFT_PROCESSOR_FFTW_IMPL_H
#define LIB_PHY_GENERIC_FUNCTIONS_DFT_PROCESSOR_FFTW_IMPL_H

#include "srsgnb/phy/generic_functions/dft_processor.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include <cstring>
#include <fftw3.h>
#include <memory>
#include <mutex>

namespace srsgnb {

/// Provides the necessary initialization parameters to dft_processor_fftw_impl.
struct dft_processor_factory_fftw_config {
  /// Sets to true to avoid loading the FFTW wisdom from a file.
  bool avoid_wisdom;
  /// Provides the FFTW wisdom filename. Leave empty for default value and ignore if wisdom is disabled.
  std::string wisdom_filename;
};

/// Describes a DFT processor class configuration based on the FFTW library.
class dft_processor_fftw_impl : public dft_processor
{
private:
  /// Describes an FFTW wisdom file name with static memory.
  class fftw_wisdom_filename
  {
  private:
    /// Persistent memory. It does not get freed.
    std::array<char, 256> data;

  public:
    /// Creates a default FFTW wisdom file name.
    fftw_wisdom_filename();

    /// Overwrites the current file name.
    fftw_wisdom_filename& operator=(const std::string& new_filename)
    {
      if (new_filename.size() <= data.size()) {
        strcpy(data.data(), new_filename.c_str());
      } else {
        clear();
      }
      return *this;
    }

    /// Erase the current file name.
    void clear() { data[0] = 0; }

    /// Checks if the file name was erased or non existent.
    bool empty() const { return data[0] == 0; }

    /// Get the file name.
    const char* get() const { return data.data(); }
  };

  /// Protects the FFTW library from simultaneous plan creation.
  static std::mutex mutex_init;
  /// If wisdom was loaded earlier by any instance, it contains the FFTW wisdom file name. Otherwise, it is empty.
  static fftw_wisdom_filename wisdom_filename;
  /// Counts the number of FFTW instances. Used to clean up the FFTW context.
  static unsigned fftw_count;
  /// Stores the DFT direction.
  direction dir;
  /// DFT input buffer ownership.
  srsvec::aligned_vec<cf_t> input;
  /// DFT output buffer ownership.
  srsvec::aligned_vec<cf_t> output;
  /// FFTW actual plan.
  fftwf_plan plan;

public:
  /// \brief Constructs a DFT processor based on the FFTW library.
  /// \remark The FFTW wisdom is loaded with the first instance.
  /// \param [in] fftw_config Provides the FFTW specific parameters.
  /// \param [in] dft_config Provides the generic DFT processor parameters.
  dft_processor_fftw_impl(const dft_processor_factory_fftw_config& fftw_config, const configuration& dft_config);

  /// Frees the input and output buffers, destroys the FFTW plan, saves the wisdom if it is the first instance to be
  /// destroyed and cleans up the FFTW context if it is the last instance to be destroyed.
  ~dft_processor_fftw_impl() override;

  // See interface for documentation.
  direction get_direction() const override { return dir; }

  // See interface for documentation.
  unsigned int get_size() const override { return input.size(); }

  // See interface for documentation.
  span<cf_t> get_input() override { return input; }

  // See interface for documentation.
  span<const cf_t> run() override;
};

/// Describes a DFT processor factory class configuration based on the FFTW library.
class dft_processor_factory_fftw_impl : public dft_processor_factory
{
  /// Stores the FFTW specific configuration.
  dft_processor_factory_fftw_config config;

public:
  /// \brief Constructs a DFT processor based on the FFTW library.
  /// \remark The FFTW wisdom is loaded with the first instance.
  /// \param [in] factory_config Provides the required parameters to create the FFTW factory.
  dft_processor_factory_fftw_impl(const dft_processor_factory_fftw_config& factory_config) : config(factory_config)
  {
    // Do nothing.
  }

private:
  std::unique_ptr<dft_processor> create(const dft_processor::configuration& dft_config) override;
};

/// \brief Creates a DFT processor factory based on the FFTW library.
/// \param [in] factory_config Provides the required parameters to create the DFT processor factory based on the FFTW
/// library.
/// \return A unique pointer of a DFT processor factory based on FFTW library if the configuration is valid.
std::unique_ptr<dft_processor_factory>
create_dft_processor_factory_fftw(const dft_processor_factory_fftw_config& factory_config);

} // namespace srsgnb

#endif // LIB_PHY_GENERIC_FUNCTIONS_DFT_PROCESSOR_FFTW_IMPL_H
