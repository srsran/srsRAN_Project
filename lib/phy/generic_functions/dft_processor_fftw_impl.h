/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "srsran/phy/generic_functions/dft_processor.h"
#include "srsran/srsvec/aligned_vec.h"
#include <cstring>
#include <fftw3.h>
#include <memory>
#include <mutex>

namespace srsran {

/// \brief FFTW optimization flags.
///
/// FFTW planning rigor flags, arranged from fastest to slowest in terms of plan creation time, and from least to most
/// optimized FFT algorithm selection.
enum class fftw_plan_optimization : unsigned {
  /// Picks a sub-optimal plan quickly, using heuristics.
  fftw_estimate = FFTW_ESTIMATE,
  /// Finds an optimized FFT plan by computing several FFTs and measuring their execution time.
  fftw_measure = FFTW_MEASURE,
  /// Same as \ref fftw_measure but more exhaustive.
  fftw_exhaustive = FFTW_EXHAUSTIVE
};

/// Describes the specific FFTW DFT parameters.
struct dft_processor_fftw_configuration {
  /// FFTW plan optimization flag, used for DFT plan creation.
  fftw_plan_optimization optimization_flag;
  /// FFTW plan creation maximum duration in seconds. Set to zero or lower for infinite.
  double plan_creation_timeout_s;
  /// Set to true to avoid loading the FFTW wisdom from a file.
  bool avoid_wisdom;
  /// FFTW wisdom filename. Leave empty for default value and ignore if wisdom is disabled.
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
  dft_processor_fftw_impl(const dft_processor_fftw_configuration& fftw_config, const configuration& dft_config);

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

} // namespace srsran
