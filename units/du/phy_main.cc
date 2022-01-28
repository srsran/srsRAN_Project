#include "srsgnb/phy/channel_coding/crc_calculator.h"
#include "srsgnb/phy/channel_coding/ldpc_codeblock_segmentation.h"
#include "srsgnb/phy/channel_coding/ldpc_encoder.h"
#include "srsgnb/phy/channel_coding/ldpc_rate_matching.h"
#include "srsgnb/phy/channel_modulation/modulation_mapper.h"
#include "srsgnb/phy/channel_modulation/scrambler.h"
#include "srsgnb/phy/channel_processors/pdsch_encoder.h"
#include "srsgnb/phy/channel_processors/pdsch_modulator.h"
#include "srsgnb/phy/channel_processors/pdsch_processor.h"
#include "srsgnb/phy/channel_processors/pdsch_resource_mapper.h"
#include "srsgnb/phy/downlink_processor.h"
#include "srsgnb/phy/phy.h"
#include "srsgnb/phy/resource_mapping/layer_mapping.h"
#include "srsgnb/phy/resource_mapping/resource_element_mapping.h"
#include <cstdio>

using namespace srsgnb;

namespace {

class dummy_gateway : public phy_output_gateway
{
public:
  bool send(const std::vector<std::complex<float> >& samples) override
  {
    printf("[PHY-OUT-GW] Sending %u IQ samples to the outside world\n", (unsigned)samples.size());
    return true;
  }

  bool send(const resource_grid& grid) override
  {
    printf("[PHY-OUT-GW] Sending resource grid to the outside world\n");
    return true;
  }
};

///
/// ENCODING ENTITIES
///
class crc_calculator_impl : public crc_calculator
{
public:
  crc_calculator_checksum_t calculate(const byte_buffer& data) override
  {
    printf("[CRC] Calculating CRC for input of %u bytes\n", (unsigned)data.size());
    return data[0] ^ 0xAAU;
  }
};

class ldpc_codeblock_segmentation_impl : public ldpc_codeblock_segmentation
{
public:
  std::vector<byte_buffer> segment(const byte_buffer& input, ldpc_base_graph base_graph) override
  {
    printf("[CB_SEGM] Received TB of %u bytes\n", (unsigned)input.size());
    if (input.size() > 4) {
      printf("[CB_SEGM] TB block needs to be segmented into two blocks\n");
      // In the real implementation this class will also have a dependency with the CRC calc.
      return {{input[0], input[1]}, {input[2], input[3]}};
    }
    printf("[CB_SEGM] TB block does not need segmentation\n");
    return {input};
  }
};

class ldpc_encoder_impl : public ldpc_encoder
{
public:
  byte_buffer encode(const byte_buffer& input, ldpc_base_graph base_graph) override
  {
    printf("[LDPC_ENCODER] Received codeblock of %u bytes\n", (unsigned)input.size());
    auto encoded_cb = input;
    for (auto& x : encoded_cb) {
      ++x;
    }

    // fill the end with zeros
    encoded_cb.insert(encoded_cb.end(), 10, 0);
    return encoded_cb;
  }
};

class ldpc_rate_matching_impl : public ldpc_rate_matching
{
public:
  byte_buffer rate_match(const byte_buffer& input) override
  {
    printf("[LDPC_RM] Received codeblock of %u bytes\n", (unsigned)input.size());
    auto rate_matched_cb = input;
    for (auto& x : rate_matched_cb) {
      ++x;
    }

    return rate_matched_cb;
  }
};

class scrambler_impl : public scrambler
{
public:
  byte_buffer scramble(const byte_buffer& input, const byte_buffer& sequence) override
  {
    printf("[SCRAMBLER] Scrambling codeword of %u bytes\n", (unsigned)input.size());

    byte_buffer scrambled_data;
    for (unsigned i = 0, e = input.size(); i != e; ++i) {
      scrambled_data.push_back(input[i] ^ sequence[i % sequence.size()]);
    }
    return scrambled_data;
  }
};

class modulator_impl : public modulation_mapper
{
public:
  std::vector<std::complex<float> > modulate(const byte_buffer& input, modulation_scheme scheme) override
  {
    printf("[MODULATOR] Modulating codeword of %u bytes\n", (unsigned)input.size());

    std::vector<std::complex<float> > symbols;
    for (auto i : input) {
      symbols.emplace_back(i * 7.F, 0);
    }
    return symbols;
  }
};

class layer_mapping_impl : public layer_mapping
{
public:
  std::vector<std::vector<std::complex<float> > > map(const std::vector<std::complex<float> >& symbols,
                                                      unsigned                                 num_layers) override
  {
    printf("[LAYER_MAPPING] Mapping symbols to one layer\n");
    return {symbols};
  }
};

class resource_element_mapping_impl : public resource_element_mapping
{
public:
  resource_grid map(const std::vector<std::complex<float> >& input, const std::vector<unsigned int>& indices) override
  {
    printf("[RE_MAPPER] Mapping symbols to virtual RBs\n");
    printf("[RE_MAPPER] Mapping virtual RBs to physical RBs\n");
    return {};
  }
};

///
/// PDSCH UCs
///
class pdsch_encoder_impl : public pdsch_encoder
{
  // This class will hold references to abstract interfaces, not real implementations like in here.
  // Done this way for now to avoid adding more code for dep injection.
  crc_calculator_impl              crc_calc;
  ldpc_codeblock_segmentation_impl cbsegm;
  ldpc_encoder_impl                encoder;
  ldpc_rate_matching_impl          rm;

public:
  std::vector<byte_buffer> encode(const byte_buffer& input) override
  {
    printf("[PDSCH_ENCODER] Received TB of size %u bytes\n", (unsigned)input.size());

    // As this is a stub implementation i'm skipping CRC polynom or LDPC BG selection.
    unsigned crc         = crc_calc.calculate(input);
    auto     tb_with_crc = input;
    tb_with_crc.push_back(crc);

    auto cbs = cbsegm.segment(tb_with_crc, ldpc_base_graph::BG1);

    std::vector<byte_buffer> code_blocks;
    for (const auto& cb : cbs) {
      auto encoded_cb      = encoder.encode(cb, ldpc_base_graph::BG1);
      auto rate_matched_cb = rm.rate_match(encoded_cb);
      code_blocks.push_back(rate_matched_cb);
    }

    return code_blocks;
  }
};

class pdsch_modulator_impl : public pdsch_modulator
{
  scrambler_impl scrambler;
  modulator_impl modulator;

public:
  std::vector<std::complex<float> > modulate(const byte_buffer& input) override
  {
    printf("[PDSCH_MODULATOR] Received codeword of size %u bytes\n", (unsigned)input.size());
    auto scrambled_cw       = scrambler.scramble(input, {1, 2, 3, 4});
    auto modulation_symbols = modulator.modulate(scrambled_cw, modulation_scheme::QPSK);
    return modulation_symbols;
  }
};

class pdsch_resource_mapper_impl : public pdsch_resource_mapper
{
  layer_mapping_impl            layer_mapper;
  resource_element_mapping_impl re_mapper;

public:
  std::vector<resource_grid> map(const std::vector<std::complex<float> >& input,
                                 const std::vector<unsigned int>&         reserved_indices) override
  {
    printf("[PDSCH_RESOURCE_MAPPER] Received %u modulation symbols\n", (unsigned)input.size());

    std::vector<resource_grid> grids;
    auto                       layers = layer_mapper.map(input, 1);
    for (const auto& layer : layers) {
      grids.push_back(re_mapper.map(layer, {1, 2, 3}));
    }
    return grids;
  }
};

class pdsch_processor_impl : public pdsch_processor
{
  phy_output_gateway&        gw;
  pdsch_encoder_impl         encoder;
  pdsch_modulator_impl       modulator;
  pdsch_resource_mapper_impl re_mapper;

public:
  explicit pdsch_processor_impl(phy_output_gateway& gw) : gw(gw) {}

  bool process(const byte_buffer& tb) override
  {
    printf("[PDSCH_PROCESSOR] Received TB to process of size %u bytes\n", (unsigned)tb.size());

    auto code_blocks = encoder.encode(tb);
    for (const auto cb : code_blocks) {
      auto symbols = modulator.modulate(cb);
      auto grids   = re_mapper.map(symbols, {1, 2, 3});
      for (const auto& grid : grids) {
        gw.send(grid);
      }
    }

    return true;
  }
};

class downlink_processor_impl : public downlink_processor
{
  pdsch_processor& pdsch;

public:
  explicit downlink_processor_impl(pdsch_processor& pdsch) : pdsch(pdsch) {}

  bool process(const dl_sched& sched) override
  {
    printf("[DL_PROCESSOR] Processing new slot data\n");

    return pdsch.process(sched.pdu);
  }
};

} // namespace

void generate_dummy_dlsch()
{
  dummy_gateway           gw;
  pdsch_processor_impl    pdsch(gw);
  downlink_processor_impl proc(pdsch);

  byte_buffer pdu = {1, 2, 3, 4};
  proc.process({pdu});
}

int main()
{
  generate_dummy_dlsch();
  return 0;
}
