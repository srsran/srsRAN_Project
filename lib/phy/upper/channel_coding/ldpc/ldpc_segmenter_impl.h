/// \file
/// \brief LDPC codeblock segmentation declaration.

#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_IMPL_H

#include "ldpc_graph_impl.h"
#include "srsgnb/phy/upper/channel_coding/crc_calculator.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_segmenter.h"
#include <iostream>

namespace srsgnb {
namespace ldpc {
constexpr unsigned max_BG1_block_length = (BG1_N_full - BG1_M) * max_lifting_size;
constexpr unsigned max_BG2_block_length = (BG2_N_full - BG2_M) * max_lifting_size;
} // namespace ldpc

class ldpc_segmenter_impl : public ldpc_segmenter
{
public:
  explicit ldpc_segmenter_impl(std::unique_ptr<crc_calculator> c);

  void segment(segmented_codeblocks&   segments,
               tb_segment_description& segment_descriptions,
               span<const uint8_t>     transport_block,
               const config_t&         cfg) override;

private:
  void        compute_nof_segments();
  void        compute_lifting_size();
  void        compute_segment_length();
  unsigned    compute_rm_length(unsigned i_seg, modulation_scheme mod, unsigned nof_layers) const;
  static void not_implemented(const std::string& str)
  {
    std::cout << "LDPC segmenter -> " << str << ": not implemented yet.";
  }

  // Data members.
  /// line 1
  unsigned                        max_segment_length{0};
  unsigned                        segment_length{0};
  unsigned                        nof_tb_bits_in{0};
  unsigned                        nof_tb_bits_out{0};
  unsigned                        nof_segments{0};
  ldpc::base_graph_t              base_graph{ldpc::base_graph_t::BG1};
  unsigned                        lifting_size{0};
  unsigned                        nof_available_coded_bits{0};
  unsigned                        symbols_per_layer{0};
  unsigned                        nof_short_segments{0};
  std::unique_ptr<crc_calculator> crc{};
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_LDPC_LDPC_SEGMENTER_IMPL_H
