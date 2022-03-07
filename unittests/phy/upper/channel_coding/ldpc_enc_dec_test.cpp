/// \file
/// \brief Unit test for LDPC encoder and decoder.
///
/// For all possible base graphs and lifting sizes, the test extracts from a file a small set of messages and
/// corresponding codeblocks. The messages are fed to the encoder, whose output is compared to the codeblocks.
/// Similarly, the codeblocks are fed to the decoder and the resulting messages are compared to the example ones.

#include "srsgnb/adt/span.h"
#include "srsgnb/phy/upper/channel_coding/ldpc.h"
#include "srsgnb/phy/upper/channel_coding/ldpc_decoder.h"
#include "srsgnb/phy/upper/channel_coding/ldpc_encoder.h"
#include <fstream>
#include <iostream>
#include <random>
#include <string>

/// Number of messages and codewords in each data set.
constexpr uint8_t NOF_MESSAGES = 10;

/// Converts a string into a vector of bits.
void read_bits(std::ifstream& example_file, std::vector<uint8_t>& out);

/// Given base graph \c bg and lifting size \c ls, retrieves test messages and codeblocks from a file.
void get_examples(unsigned                                        bg,
                  unsigned                                        ls,
                  std::array<std::vector<uint8_t>, NOF_MESSAGES>& msgs,
                  std::array<std::vector<uint8_t>, NOF_MESSAGES>& cblocks);

int main()
{
  std::unique_ptr<srsgnb::ldpc_encoder> my_encoder = srsgnb::create_ldpc_encoder("generic");
  std::unique_ptr<srsgnb::ldpc_decoder> my_decoder = srsgnb::create_ldpc_decoder("generic");

  for (const auto bg :
       std::array<srsgnb::ldpc::base_graph_t, 2>{srsgnb::ldpc::base_graph_t::BG1, srsgnb::ldpc::base_graph_t::BG2}) {
    for (const auto ls : srsgnb::ldpc::all_lifting_sizes) {
      srsgnb::ldpc_encoder::config_t cfg_enc{bg, ls};
      srsgnb::ldpc_decoder::config_t cfg_dec{bg, ls};

      std::cout << "Testing BG" << static_cast<unsigned>(bg) + 1 << ", LS: " << ls << std::endl;

      // compute message and codeblock lengths
      unsigned min_cb_length = 24 * ls;
      unsigned max_cb_length = 66 * ls;
      unsigned msg_length    = 22 * ls;
      if (bg == srsgnb::ldpc::base_graph_t::BG2) {
        min_cb_length = 12 * ls;
        max_cb_length = 50 * ls;
        msg_length    = 10 * ls;
      }

      // get example messages and corresponding codeblocks
      std::array<std::vector<uint8_t>, NOF_MESSAGES> msg{};
      msg.fill(std::vector<uint8_t>(msg_length));
      std::array<std::vector<uint8_t>, NOF_MESSAGES> cblock{};
      cblock.fill(std::vector<uint8_t>(max_cb_length));
      get_examples(static_cast<unsigned>(bg), ls, msg, cblock);

      // encode each message and compare with given codeblock
      const auto* cblock_i    = cblock.cbegin();
      unsigned    length_step = (max_cb_length - min_cb_length) / 10;
      for (const auto& msg_i : msg) {
        assert(cblock_i != cblock.cend());
        // check several shortened codeblocks
        for (unsigned length = min_cb_length; length < max_cb_length; length += length_step) {
          std::vector<uint8_t> encoded(length);
          my_encoder->encode(encoded, msg_i, cfg_enc);
          assert(std::equal(encoded.begin(), encoded.end(), cblock_i->begin()));

          std::vector<uint8_t> decoded(msg_length);
          std::vector<int8_t>  llrs(length);
          std::transform(cblock_i->begin(), cblock_i->begin() + length, llrs.begin(), [](uint8_t b) {
            return ((b == srsgnb::ldpc::filler_bit) ? 10 : 10 - 20 * b);
          });
          my_decoder->decode(decoded, llrs, cfg_dec);
          assert(std::equal(decoded.begin(), decoded.end(), msg_i.begin(), [](uint8_t a, uint8_t b) {
            return ((a == b) || ((a == 0) && (b == srsgnb::ldpc::filler_bit)));
          }));
        }
        // check full-length codeblock
        std::vector<uint8_t> encoded(max_cb_length);
        my_encoder->encode(encoded, msg_i, cfg_enc);
        assert(std::equal(encoded.begin(), encoded.end(), cblock_i->begin()));

        std::vector<uint8_t> decoded(msg_length);
        std::vector<int8_t>  llrs(max_cb_length);
        std::transform(cblock_i->begin(), cblock_i->end(), llrs.begin(), [](uint8_t b) { return 10 - 20 * b; });
        my_decoder->decode(decoded, llrs, cfg_dec);
        assert(std::equal(decoded.begin(), decoded.end(), msg_i.begin(), [](uint8_t a, uint8_t b) {
          return ((a == b) || ((a == 0) && (b == srsgnb::ldpc::filler_bit)));
        }));

        ++cblock_i;
      }
    }
  }

  std::cout << "it works!" << std::endl;
}

void read_bits(std::ifstream& example_file, std::vector<uint8_t>& out)
{
  for (auto& bit : out) {
    int tmp = example_file.get();
    switch (tmp) {
      case '0':
      case '1':
        bit = tmp - '0';
        break;
      case '-':
        bit = srsgnb::ldpc::filler_bit;
        break;
      default:
        assert(false);
    }
  }
  // we should be at the end of the string
  assert(example_file.get() == '\n');
}

void get_examples(unsigned                                        bg,
                  unsigned                                        ls,
                  std::array<std::vector<uint8_t>, NOF_MESSAGES>& msgs,
                  std::array<std::vector<uint8_t>, NOF_MESSAGES>& cblocks)
{
  std::string filename{"examples"};
  filename += "BG" + std::to_string(bg + 1) + ".dat";
  std::ifstream example_file(filename);
  if (!example_file.is_open()) {
    assert(false);
  }
  std::string msg_string = "ls" + std::to_string(ls) + "msgs";

  std::string id_string;
  for (std::getline(example_file, id_string); id_string != msg_string; std::getline(example_file, id_string)) {
    // If not msg_string, we can skip the following 2 * NOF_MESSAGES + 1 lines (NOF_MESSAGES messages, NOF_MESSAGES
    // codeblocks and one separating text line).
    for (int i = 0; i != 2 * NOF_MESSAGES + 1; ++i) {
      example_file.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
  }

  // The latest read line corresponds to the LS value we are interested in. Read the messages.
  for (auto& msg_i : msgs) {
    read_bits(example_file, msg_i);
  }
  // The following line should mark the start of the corresponding codeblocks.
  std::string cwd_string = "ls" + std::to_string(ls) + "cwds";
  std::getline(example_file, id_string);
  assert(id_string == cwd_string);

  // Read the codeblocks.
  for (auto& cblock_i : cblocks) {
    read_bits(example_file, cblock_i);
  }
}
