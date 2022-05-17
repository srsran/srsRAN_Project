/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/phy/upper/channel_coding/polar/polar_allocator.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_code.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_deallocator.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_decoder.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_encoder.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_rate_dematcher.h"
#include "srsgnb/phy/upper/channel_coding/polar/polar_rate_matcher.h"
#include "srsgnb/srsvec/aligned_vec.h"
#include "srsgnb/support/srsgnb_test.h"
#include <getopt.h>
#include <random>

using namespace srsgnb;

/*!
 * \file polar_chain_test.c
 * \brief Ent-to-end test for the Polar coding chain including: subchannel allocator, encoder, rate-matcher,
 rate-dematcher, decoder and subchannel deallocation.
 *
 * A batch of example messages is randomly generated, frozen bits are added, encoded, rate-matched, 2-PAM modulated,
 * sent over an AWGN channel, rate-dematched, and, finally, decoded by all three types of
 * decoder. Transmitted and received messages are compared to estimate the WER.
 * Multiple batches are simulated if the number of errors is not significant
 * enough.
 *
 * Synopsis: **polar_chain_test [options]**
 *
 * Options:
 *
 *  - <b>-n \<number\></b> nMax,  [Default 9] -- Use 9 for downlink, and 10 for uplink configuration.
 *  - <b>-k \<number\></b> Message size (K),  [Default 128]. K includes the CRC bits if applicable.
 *  If nMax = 9, K must satisfy 165 > K > 35. If nMax = 10, K must satisfy K > 17 and K <1024, excluding 31 > K > 25.
 *  - <b>-e \<number\></b> Rate matching size (E), [Default 256]. If 17 < K < 26, E must satisfy K +3 < E < 8193.
 * If K > 30, E must satisfy K < E < 8193.
 *  - <b>-i \<number\></b> Enable bit interleaver (bil),  [Default 0] -- Set bil = 0 to disable the
 * bit interleaver at rate matching. Choose 0 for downlink and 1 for uplink configuration.
 *  - <b>-s \<number\></b>  SNR [dB, Default 3.00 dB] -- Use 100 for scan, and 101 for noiseless.
 *  - <b>-o \<number\></b>  Print output results [Default 0] -- Use 0 for detailed, Use 1 for 1 line, Use 2 for vector
 * form.
 *
 * Example 1: BCH - ./polar_chain_test -n9 -k56 -e864 -i0 -s101 -o1
 *
 * Example 2: DCI - ./polar_chain_test -n9 -k40 -e100 -i0 -s101 -o1
 *
 * Example 3: UCI - PC bits - ./polar_chain_test -n10 -k20 -e256 -i1 -s101 -o1
 *
 * Example 4: UCI - puncturing 19 first bits - ./polar_chain_test -n10 -k18 -e45 -i1 -s101 -o1
 *
 * Example 5: UCI - shortening 26 last bits - ./polar_chain_test -n10 -k18 -e38 -i1 -s101 -o1
 *
 *
 */

// default values
static uint16_t        K      = 56;  /*!< \brief Number of message bits (data and CRC). */
static uint16_t        E      = 864; /*!< \brief Number of bits of the codeword after rate matching. */
static uint8_t         nMax   = 9;   /*!< \brief Maximum \f$log_2(N)\f$, where \f$N\f$ is the codeword size.*/
static polar_code_ibil bil    = polar_code_ibil::not_present; /*!< \brief If bil = 0 channel interleaver disabled. */
static double          snr_db = 3;                            /*!< \brief SNR in dB (101 for no noise, 100 for scan). */
static int print_output       = 0; /*!< \brief print output form (0 for detailed, 1 for one line, 2 for vector). */

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
  printf("\t-i Bit interleaver indicator [Default %d]\n", (int)bil);
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
        bil = strtol(optarg, nullptr, 10) ? polar_code_ibil::present : polar_code_ibil::not_present;
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

  // Create deallocator
  std::unique_ptr<polar_deallocator> deallocator = create_polar_deallocator();

  // Create rate matcher
  std::unique_ptr<polar_rate_matcher> rate_matcher = create_polar_rate_matcher();

  // Create rate dematcher
  std::unique_ptr<polar_rate_dematcher> rate_dematcher = create_polar_rate_dematcher();

  // Create Tx data and fill
  srsvec::aligned_vec<uint8_t> data_tx(K);
  for (uint8_t& v : data_tx) {
    v = dist(rgen);
  }

  // Set code
  code->set(K, E, nMax, bil);

  // Allocate TX data
  srsvec::aligned_vec<uint8_t> allocated_tx(code->get_N());
  allocator->allocate(data_tx, allocated_tx, *code);

  // Encoder TX data
  srsvec::aligned_vec<uint8_t> encoded_tx(code->get_N());
  encoder->encode(allocated_tx, code->get_n(), encoded_tx);

  // Rate matching
  srsvec::aligned_vec<uint8_t> rate_matched_tx(E);
  rate_matcher->rate_match(encoded_tx, rate_matched_tx, *code);

  // Modulate
  srsvec::aligned_vec<int8_t> rate_matched_rx(E);
  for (std::size_t i = 0, size = rate_matched_rx.size(); i != size; ++i) {
    rate_matched_rx[i] = (rate_matched_tx[i] == 0) ? +1 : -1;
  }

  // Undo rate matching
  srsvec::aligned_vec<int8_t> encoded_rx(code->get_N());
  rate_dematcher->rate_dematch(rate_matched_rx, encoded_rx, *code);

  // Decode Rx data
  srsvec::aligned_vec<uint8_t> allocated_rx(code->get_N());
  decoder->decode(encoded_rx, allocated_rx, *code);

  // Deallocate RX data
  srsvec::aligned_vec<uint8_t> data_rx(K);
  deallocator->deallocate(allocated_rx, data_rx, *code);

  // Assert decoded message
  TESTASSERT(std::equal(data_tx.begin(), data_tx.end(), data_rx.begin()));
}
