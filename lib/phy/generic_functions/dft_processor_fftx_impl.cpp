
#include "dft_processor_fftx_impl.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

#define FFTX_PROTOTYPES_SIZE(SIZE)                                                                                     \
  extern "C" {                                                                                                         \
  void init_fftx_dftbat_1_##SIZE##_1d_CPU();                                                                           \
  void fftx_dftbat_1_##SIZE##_1d_CPU(float* Y, float* X);                                                              \
  void init_fftx_idftbat_1_##SIZE##_1d_CPU();                                                                          \
  void fftx_idftbat_1_##SIZE##_1d_CPU(float* Y, float* X);                                                             \
  }

#define FFTX_CREATE_SIZE(SIZE)                                                                                         \
  do {                                                                                                                 \
    if (input.size() == SIZE && dir == direction::DIRECT) {                                                            \
      init_fftx_dftbat_1_##SIZE##_1d_CPU();                                                                            \
      function = [this]() { fftx_dftbat_1_##SIZE##_1d_CPU((float*)input.data(), (float*)output.data()); };             \
      return;                                                                                                          \
    }                                                                                                                  \
    if (input.size() == SIZE && dir == direction::INVERSE) {                                                           \
      init_fftx_idftbat_1_##SIZE##_1d_CPU();                                                                           \
      function = [this]() { fftx_idftbat_1_##SIZE##_1d_CPU((float*)input.data(), (float*)output.data()); };            \
      return;                                                                                                          \
    }                                                                                                                  \
  } while (false)

FFTX_PROTOTYPES_SIZE(128)
FFTX_PROTOTYPES_SIZE(256)
FFTX_PROTOTYPES_SIZE(384)
FFTX_PROTOTYPES_SIZE(512)
FFTX_PROTOTYPES_SIZE(768)
FFTX_PROTOTYPES_SIZE(1024)
FFTX_PROTOTYPES_SIZE(1536)
FFTX_PROTOTYPES_SIZE(2048)
FFTX_PROTOTYPES_SIZE(3072)
FFTX_PROTOTYPES_SIZE(4096)
FFTX_PROTOTYPES_SIZE(4608)
FFTX_PROTOTYPES_SIZE(6144)
FFTX_PROTOTYPES_SIZE(9216)
FFTX_PROTOTYPES_SIZE(12288)
FFTX_PROTOTYPES_SIZE(18432)
FFTX_PROTOTYPES_SIZE(24576)
FFTX_PROTOTYPES_SIZE(36864)
FFTX_PROTOTYPES_SIZE(49152)

dft_processor_fftx_impl::dft_processor_fftx_impl(const configuration& dft_config) :
  dir(dft_config.dir), input(dft_config.size), output(dft_config.size)
{
  FFTX_CREATE_SIZE(128);
  FFTX_CREATE_SIZE(256);
  FFTX_CREATE_SIZE(384);
  FFTX_CREATE_SIZE(512);
  FFTX_CREATE_SIZE(768);
  FFTX_CREATE_SIZE(1024);
  FFTX_CREATE_SIZE(1536);
  FFTX_CREATE_SIZE(2048);
  FFTX_CREATE_SIZE(3072);
  FFTX_CREATE_SIZE(4096);
  FFTX_CREATE_SIZE(4608);
  FFTX_CREATE_SIZE(6144);
  FFTX_CREATE_SIZE(9216);
  FFTX_CREATE_SIZE(12288);
  FFTX_CREATE_SIZE(18432);
  FFTX_CREATE_SIZE(24576);
  FFTX_CREATE_SIZE(36864);
  FFTX_CREATE_SIZE(49152);

  fmt::print(
      "The {} DFT {} points is not implemented.", dir == direction::DIRECT ? "direct" : "inverse", dft_config.size);
}

span<const cf_t> dft_processor_fftx_impl::run()
{
  srsran_always_assert(function, "Invalid function of size {}.", input.size());
  std::fill(output.begin(), output.end(), 0);
  function();

  // Return the view of the output data.
  return output;
}
