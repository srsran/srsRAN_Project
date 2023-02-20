/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
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
      fftwf_import_wisdom_from_filename(wisdom_filename.get());
    }
  }

  // Prepare FFTW plan creation arguments.
  int            fftw_size   = static_cast<int>(dft_config.size);
  fftwf_complex* fftw_input  = (fftwf_complex*)input.data();
  fftwf_complex* fftw_output = (fftwf_complex*)output.data();
  unsigned int   fftw_flags  = 0;
  int            fftw_sign   = (dir == direction::DIRECT) ? FFTW_FORWARD : FFTW_BACKWARD;

  // Create plan.
  plan = fftwf_plan_dft_1d(fftw_size, fftw_input, fftw_output, fftw_sign, fftw_flags);

  // Make sure plan is valid.
  srsgnb_assert(plan != nullptr,
                "FFTW Plan creation for size=%d dir=%s failed.",
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
    fftwf_export_wisdom_to_filename(wisdom_filename.get());

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
