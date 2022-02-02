/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/channel_coding/polar_allocator.h"
#include "srsgnb/phy/channel_coding/polar_code.h"
#include "srsgnb/phy/channel_coding/polar_decoder.h"
#include "srsgnb/phy/channel_coding/polar_encoder.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include <getopt.h>
#include <random>

using namespace srsgnb;

// default values
static uint16_t K            = 128; /*!< \brief Number of message bits (data and CRC). */
static uint16_t E            = 256; /*!< \brief Number of bits of the codeword after rate matching. */
static uint8_t  nMax         = 9;   /*!< \brief Maximum \f$log_2(N)\f$, where \f$N\f$ is the codeword size.*/
static uint8_t  bil          = 0;   /*!< \brief If bil = 0 channel interleaver disabled. */
static double   snr_db       = 3;   /*!< \brief SNR in dB (101 for no noise, 100 for scan). */
static int      print_output = 0;   /*!< \brief print output form (0 for detailed, 1 for one line, 2 for vector). */

static std::mt19937 rgen(0);

/*!
 * \brief Prints test help when a wrong parameter is passed as input.
 */
void usage(char* prog)
{
  printf("Usage: %s [-nX] [-kX] [-eX] [-iX] [-sX] [-oX]\n", prog);
  printf("\t-n nMax [Default %d]\n", nMax);
  printf("\t-k Message size [Default %d]\n", K);
  printf("\t-e Rate matching size [Default %d]\n", E);
  printf("\t-i Bit interleaver indicator [Default %d]\n", bil);
  printf("\t-s SNR [dB, Default %.2f dB] -- Use 100 for scan, and 101 for noiseless\n", snr_db);
  printf("\t-o Print output results [Default %d] -- Use 0 for detailed, Use 1 for 1 line, Use 2 for vector form\n",
         print_output);
}

/*!
 * \brief Parses the input line.
 */
void parse_args(int argc, char** argv)
{
  int opt = 0;
  while ((opt = getopt(argc, argv, "n:k:e:i:s:o:")) != -1) {
    //  printf("opt : %d\n", opt);
    switch (opt) {
      case 'e':
        E = (int)strtol(optarg, nullptr, 10);
        break;
      case 'k':
        K = (int)strtol(optarg, nullptr, 10);
        break;
      case 'n':
        nMax = (uint8_t)strtol(optarg, nullptr, 10);
        break;
      case 'i':
        bil = (int)strtol(optarg, nullptr, 10);
        break;
      case 's':
        snr_db = strtof(optarg, nullptr);
        break;
      case 'o':
        print_output = (int)strtol(optarg, nullptr, 10);
        break;
      default:
        usage(argv[0]);
        exit(-1);
    }
  }
}

/*!
 * \brief Main function.
 */
int main(int argc, char** argv)
{
  // Random distribution generates unpacked bits
  std::uniform_int_distribution<uint8_t> dist(0, 1);

  parse_args(argc, argv);

  // Create code
  std::unique_ptr<polar_code> code = create_polar_code();

  // Create encoder
  std::unique_ptr<polar_encoder> encoder = create_polar_encoder_pipelined(nMax);

  // Create allocator
  std::unique_ptr<polar_allocator> allocator = create_polar_allocator();

  // Create decoder
  std::unique_ptr<polar_decoder> decoder = create_polar_decoder_ssc(create_polar_encoder_pipelined(nMax), nMax);

  // Create Tx data and fill
  srsvec::aligned_vec<uint8_t> data_tx(K);
  for (uint8_t& v : data_tx) {
    v = dist(rgen);
  }

  // Set code
  //  code->set(K, E, nMax);

  // Allocate TX data
  //  srsvec::aligned_vec<uint8_t> allocated_tx(K);
  //  allocator->allocate(data_tx, allocated_tx, *code);

  //  // Encoder TX data
  //  srsvec::aligned_vec<uint8_t> encoded_tx(E);
  //  encoder->encode(allocated_tx, nMax, encoded_tx);
  //
  //  // Modulate
  //  srsvec::aligned_vec<int8_t> llr(E);
  //  for (std::size_t i = 0, size = llr.size(); i != size; ++i) {
  //    llr[i] = (encoded_tx[i] == 0) ? +1 : -1;
  //  }
  //
  //  srsvec::aligned_vec<uint8_t> data_rx(K);
}
