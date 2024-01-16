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

#include "dft_processor_fftw_impl.h"
#include "srsran/support/srsran_assert.h"
#include <pwd.h>
#include <unistd.h>

using namespace srsran;

std::mutex dft_processor_fftw_impl::mutex_init = {};

dft_processor_fftw_impl::fftw_wisdom_filename dft_processor_fftw_impl::wisdom_filename =
    dft_processor_fftw_impl::fftw_wisdom_filename();

unsigned dft_processor_fftw_impl::fftw_count = 0;

static void dft_processor_fftw_import_wisdom(const char* full_path)
{
  // lockf needs a file descriptor open for writing, so this must be r+.
  std::FILE* fd = std::fopen(full_path, "r+");

  // Check that wisdom file exists.
  if (fd == nullptr) {
    return;
  }

  // Lock and import wisdom file.
  if (lockf(fileno(fd), F_LOCK, 0) == -1) {
    fmt::print(stderr, "Error locking FFTW wisdom file.\n");
    std::fclose(fd);
    return;
  }
  fftwf_import_wisdom_from_file(fd);
  if (lockf(fileno(fd), F_ULOCK, 0) == -1) {
    fmt::print(stderr, "Error unlocking FFTW wisdom file.\n");
    std::fclose(fd);
    return;
  }
  std::fclose(fd);
}

static void dft_processor_fftw_export_wisdom(const char* full_path)
{
  // Open wisdom file for writing. If it does not exist, create a new file.
  std::FILE* fd = std::fopen(full_path, "w");

  if (fd == nullptr) {
    return;
  }

  // Lock and export wisdom file.
  if (lockf(fileno(fd), F_LOCK, 0) == -1) {
    fmt::print(stderr, "Error locking FFTW wisdom file.\n");
    std::fclose(fd);
    return;
  }
  fftwf_export_wisdom_to_file(fd);
  if (lockf(fileno(fd), F_ULOCK, 0) == -1) {
    fmt::print(stderr, "Error unlocking FFTW wisdom file.\n");
    std::fclose(fd);
    return;
  }
  std::fclose(fd);
}

dft_processor_fftw_impl::fftw_wisdom_filename::fftw_wisdom_filename()
{
  // Make sure data does not contain any text.
  clear();

  // Default FFT wisdom file inside home directory.
  static const char* FFTW_WISDOM_FILE = "%s/.srsran_fftwisdom";

  // Get the user home directory from environment.
  const char* homedir = getenv("HOME");

  // If the environment variable is not set, try to get it from the user database.
  if (homedir == nullptr) {
    // Get user entry in users database.
    const passwd* pwuid = getpwuid(getuid());
    if (pwuid == nullptr) {
      printf("Failed to load home directory: %s. Skipping wisdom load/save.\n", strerror(errno));
      return;
    }

    // Get home directory from user database entry.
    homedir = pwuid->pw_dir;
    if (homedir == nullptr) {
      printf("Home dir is not available. Skipping wisdom load/save.\n");
      return;
    }
  }

  // Create full path to the wisdom file.
  int n = snprintf(data.data(), data.size(), FFTW_WISDOM_FILE, homedir);
  if (n == 0 || n == 256) {
    printf("Default FFTW wisdom path exceeds the maximum length. HOME=%s. Skipping wisdom load/save.\n", homedir);
    return;
  }
}

dft_processor_fftw_impl::dft_processor_fftw_impl(const dft_processor_fftw_configuration& fftw_config,
                                                 const configuration&                    dft_config) :
  dir(dft_config.dir), input(dft_config.size), output(dft_config.size)
{
  // Avoid that two or more plan creations are called at same time.
  std::lock_guard<std::mutex> lock(mutex_init);

  // Load FFT wisdom if it is not marked to avoid.
  if (!fftw_config.avoid_wisdom) {
    // Use file name given in the configuration by default.
    if (!fftw_config.wisdom_filename.empty()) {
      wisdom_filename = fftw_config.wisdom_filename;
    }

    // If no file name is given, get default.
    if (wisdom_filename.empty()) {
      wisdom_filename = fftw_wisdom_filename();
    }

    // Load FFTW wisdom from file name if it is available.
    if (!wisdom_filename.empty()) {
      dft_processor_fftw_import_wisdom(wisdom_filename.get());
    }
  }

  // Prepare FFTW plan creation arguments.
  int            fftw_size   = static_cast<int>(dft_config.size);
  fftwf_complex* fftw_input  = (fftwf_complex*)input.data();
  fftwf_complex* fftw_output = (fftwf_complex*)output.data();
  unsigned fftw_flags = static_cast<std::underlying_type_t<fftw_plan_optimization>>(fftw_config.optimization_flag);
  int      fftw_sign  = (dir == direction::DIRECT) ? FFTW_FORWARD : FFTW_BACKWARD;

  // Set FFTW plan creation duration limit.
  if (std::isnormal(fftw_config.plan_creation_timeout_s) && (fftw_config.plan_creation_timeout_s > 0)) {
    fftwf_set_timelimit(fftw_config.plan_creation_timeout_s);
  }

  // Create plan.
  plan = fftwf_plan_dft_1d(fftw_size, fftw_input, fftw_output, fftw_sign, fftw_flags);

  // Make sure plan is valid.
  srsran_assert(plan != nullptr,
                "FFTW Plan creation for size={} dir={} failed.",
                fftw_size,
                dft_processor::direction_to_string(dir).c_str());

  // Count as another successful instance.
  ++fftw_count;
}

dft_processor_fftw_impl::~dft_processor_fftw_impl()
{
  // Avoid that two or more plan creations are called at same time.
  std::lock_guard<std::mutex> lock(mutex_init);

  // Save wisdom if it was loaded.
  if (!wisdom_filename.empty()) {
    // Save the FFT wisdom file with the same name it was loaded.
    dft_processor_fftw_export_wisdom(wisdom_filename.get());

    // Make sure other instances do not save the FFTW wisdom file.
    wisdom_filename.clear();
  }

  // Destroy FFTW plan.
  fftwf_destroy_plan(plan);

  // Decrease FFTW instance counter.
  --fftw_count;

  // Clean up if this is the last destroyed instance.
  if (fftw_count == 0) {
    fftwf_cleanup();
  }
}

span<const cf_t> dft_processor_fftw_impl::run()
{
  // Execute FFTW plan.
  fftwf_execute(plan);

  // Return the view of the output data.
  return output;
}
