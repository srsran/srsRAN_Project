
#include "dft_processor_fftw_impl.h"
#include "srsgnb/support/srsran_assert.h"
#include <pwd.h>
#include <unistd.h>

using namespace srsgnb;

std::mutex dft_processor_fftw_impl::mutex_init = {};

std::string dft_processor_fftw_impl::wisdom_filename = dft_processor_fftw_impl::get_default_fftw_wisdom_file();

unsigned dft_processor_fftw_impl::fftw_count = 0;

std::string dft_processor_fftw_impl::get_default_fftw_wisdom_file()
{
  // Default FFT wisdom file inside home directory.
  static const char* FFTW_WISDOM_FILE = "%s/.srsran_fftwisdom";

  // Get the user home directory.
  const char* homedir = nullptr;
  if ((homedir = getenv("HOME")) == nullptr) {
    homedir = getpwuid(getuid())->pw_dir;
  }

  // Create full path to the wisdom file.
  char full_path[256] = {};
  int  n              = snprintf(full_path, sizeof(full_path), FFTW_WISDOM_FILE, homedir);
  srsran_assert(n < 256, "Default FFTW wisdom path exceeds the maximum length. HOME=%s.", homedir);
  srsran_assert(n > 0, "Error getting the default FFTW wisdom path.");

  // Create C++ string from the C style string.
  return std::string(full_path);
}

dft_processor_fftw_impl::dft_processor_fftw_impl(const dft_processor_fftw_config& config) :
  dir(config.direction), input(config.size), output(config.size)
{
  // Avoid that two or more plan creations are called at same time.
  std::lock_guard<std::mutex> lock(mutex_init);

  // Load FFT wisdom if it is not marked to avoid, and it was not loaded by a different instance.
  if (!config.avoid_wisdom && wisdom_filename.empty()) {
    // Use file name given in the configuration by default.
    wisdom_filename = config.wisdom_filename;

    // If no file name is given, get default.
    if (wisdom_filename.empty()) {
      wisdom_filename = get_default_fftw_wisdom_file();
    }

    // Load FFTW wisdom from file name.
    fftwf_import_wisdom_from_filename(wisdom_filename.c_str());
  }

  // Prepare FFTW plan creation arguments.
  int            fftw_size   = (int)config.size;
  fftwf_complex* fftw_input  = (fftwf_complex*)input.data();
  fftwf_complex* fftw_output = (fftwf_complex*)output.data();
  unsigned int   fftw_flags  = 0;
  int            fftw_sign   = (dir == direction::FORWARD) ? FFTW_FORWARD : FFTW_BACKWARD;

  // Create plan.
  plan = fftwf_plan_dft_1d(fftw_size, fftw_input, fftw_output, fftw_sign, fftw_flags);

  // Make sure plan is valid.
  srsran_assert(plan != nullptr,
                "FFTW Plan creation for size=%d dir=%s failed.",
                fftw_size,
                config.direction == direction::FORWARD ? "forward" : "backward");

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
    fftwf_export_wisdom_to_filename(wisdom_filename.c_str());

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

std::unique_ptr<dft_processor> srsgnb::create_dft_processor_fftw(const dft_processor_fftw_config& config)
{
  return std::make_unique<dft_processor_fftw_impl>(config);
}
