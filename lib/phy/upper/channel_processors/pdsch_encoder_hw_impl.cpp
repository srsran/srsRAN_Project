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

#include "pdsch_encoder_hw_impl.h"
#include "srsran/phy/upper/channel_coding/crc_calculator.h"
#include "srsran/support/units.h"
#include <climits>

using namespace srsran;
using namespace srsran::ldpc;

/// Length of the CRC checksum added to the segments.
static constexpr units::bits SEG_CRC_LENGTH{24};

void pdsch_encoder_hw_impl::encode(span<uint8_t>        codeword,
                                   span<const uint8_t>  transport_block,
                                   const configuration& config)
{
  // CB mode will be forced if TB mode is requested for a TB larger than the maximum supported size.
  if (!cb_mode && transport_block.size_bytes() > max_tb_size) {
    cb_mode = true;
  }

  // Set the TB encoding parameters (common to all CBs) as required by the hardware-accelerated PDSCH encoder.
  hal::hw_pdsch_encoder_configuration hw_cfg = {};

  // Segmentation is only required in CB mode.
  unsigned nof_ops = 1;

  segmenter_config segmenter_cfg;
  segmenter_cfg.base_graph     = config.base_graph;
  segmenter_cfg.rv             = config.rv;
  segmenter_cfg.mod            = config.mod;
  segmenter_cfg.Nref           = config.Nref;
  segmenter_cfg.nof_layers     = config.nof_layers;
  segmenter_cfg.nof_ch_symbols = config.nof_ch_symbols;

  if (cb_mode) {
    // Clear the buffer.
    d_segments.clear();

    // Segmentation (it includes CRC attachment for the entire transport block and each individual segment).
    segmenter->segment(d_segments, transport_block, segmenter_cfg);
  }
  set_hw_enc_tb_configuration(hw_cfg, transport_block, d_segments, segmenter_cfg);
  if (cb_mode) {
    nof_ops = hw_cfg.nof_segments;
  }

  // Validate that all CBs have been succesfully enqueued and dequeued.
  unsigned last_enqueued_cb_id = 0, last_dequeued_cb_id = 0, offset = 0;
  bool     all_enqueued = false, all_dequeued = false;
  while (!all_enqueued || !all_dequeued) {
    // Enqueue each CB in the TB
    bool enqueued = false;
    for (unsigned cb_id = last_enqueued_cb_id; cb_id != nof_ops; ++cb_id) {
      // Update the pointer to the CB to be currently enqueued.
      last_enqueued_cb_id = cb_id;

      // Get the data to be encoded.
      span<const uint8_t> data;
      if (cb_mode) {
        // Retrieve the CB.
        described_segment& descr_seg = d_segments[cb_id];

        // Set the encoding parameters of the CB as required by the hardware-accelerated PDSCH encoder.
        set_hw_enc_cb_configuration(hw_cfg, descr_seg.get_metadata(), cb_id);

        // Retrieve the current CB to be encoded.
        unsigned segment_length       = static_cast<unsigned>(descr_seg.get_data().size()) - hw_cfg.nof_filler_bits;
        unsigned segment_length_bytes = static_cast<unsigned>(ceil(static_cast<float>(segment_length) / 8.0));
        data                          = descr_seg.get_data().get_buffer().first(segment_length_bytes);
      } else {
        // Set the encoding parameters of the CB as required by the hardware-accelerated PDSCH encoder.
        set_hw_enc_cb_configuration(hw_cfg, {}, cb_id);

        // Encode the full TB at once.
        data = transport_block;
      }
      // Enqueue the hardware-accelerated PDSCH encoding operation.
      enqueued = encoder->enqueue_operation(data, {}, cb_id);

      // Exit the enqueing loop in case the operation couldn't be enqueued.
      if (!enqueued) {
        break;
      }
    }

    // The loop is exited when all CBs in the TB have been enqueued, the queue is full or because enqueuing failed for a
    // given CB.
    if (enqueued) {
      // Update the enqueuing point to ensure that each CB is only enqueued once.
      if (last_enqueued_cb_id == nof_ops - 1) {
        ++last_enqueued_cb_id;
        all_enqueued = true;
      }
    }

    // Reset the dequeuing control variables.
    unsigned num_dequeued = 0;
    bool     dequeued     = false;

    // Dequeue each CB in the TB
    for (unsigned cb_id = last_dequeued_cb_id; cb_id != last_enqueued_cb_id; ++cb_id) {
      // Update the pointer to the CB to be currently dequeued.
      last_dequeued_cb_id = cb_id;

      // In CB mode, select the correct chunk of the output codeword.
      unsigned      rm_length = 0;
      span<uint8_t> codeblock;
      if (cb_mode) {
        rm_length = d_segments[cb_id].get_metadata().cb_specific.rm_length;
        srsran_assert(offset + rm_length <= codeword.size(), "Wrong codeword length.");

        codeblock = span<uint8_t>(codeword).subspan(offset, rm_length);
        codeblock_packed.resize(rm_length);
      } else {
        codeblock = codeword;
        codeblock_packed.resize(codeword.size());
      }

      // Make sure at least one operation is dequeued.
      dequeued = false;
      while (!dequeued) {
        // Dequeue the hardware-accelerated PUSCH decoding operation (updates the softbuffer too).
        dequeued = encoder->dequeue_operation(codeblock, codeblock_packed, cb_id);
        if (!dequeued) {
          if (num_dequeued > 0) {
            break;
          }
        } else {
          ++num_dequeued;
        }
      }

      // Exit the dequeueing loop in case no more operations can be dequeued.
      if (!dequeued) {
        break;
      }

      // Update the dequeuing offset within the TB.
      offset += rm_length;
    }
    // The loop is exited when all enqueued CBs have been dequeued or because dequeuing failed for a given CB.
    if (dequeued) {
      // Dequeueing of a given CB only has to take place once.
      ++last_dequeued_cb_id;
      if (last_dequeued_cb_id == nof_ops) {
        all_dequeued = true;
      }
    }
  }
}

/// \brief Computes the length of the rate-matched codeblock corresponding to each segment,
///  as per TS38.212 Section 5.4.2.1.
/// \param[in] i_seg                 Index of the segment.
/// \param[in] nof_segments          Number of segments in the transport block.
/// \param[in] nof_short_segments    Number of segments that will have a short rate-matched length.
/// \param[in] nof_symbols_per_layer Number of channel symbols assigned to a transmission layer.
/// \param[in] modulation_scheme     Modulation scheme.
/// \param[in] nof_layers            Number of transmission layers the transport block is mapped onto.
/// \return The length of the rate-matched codeblock corresponding to the indicated segment.
static unsigned compute_rm_length(unsigned          i_seg,
                                  unsigned          nof_segments,
                                  unsigned          nof_short_segments,
                                  unsigned          nof_symbols_per_layer,
                                  modulation_scheme mod,
                                  unsigned          nof_layers)
{
  unsigned tmp = 0;
  if (i_seg < nof_short_segments) {
    // For unsigned, division then floor is the same as integer division.
    tmp = nof_symbols_per_layer / nof_segments;
  } else {
    tmp = divide_ceil(nof_symbols_per_layer, nof_segments);
  }
  return tmp * nof_layers * get_bits_per_symbol(mod);
}

void pdsch_encoder_hw_impl::set_hw_enc_tb_configuration(
    hal::hw_pdsch_encoder_configuration&                      hw_cfg,
    span<const uint8_t>                                       transport_block,
    const static_vector<described_segment, MAX_NOF_SEGMENTS>& d_segs,
    const segmenter_config&                                   cfg)
{
  using namespace units::literals;

  // Operation mode.
  hw_cfg.cb_mode = cb_mode;

  // Modulation.
  hw_cfg.modulation = cfg.mod;

  // Redundancy version.
  hw_cfg.rv = cfg.rv;

  // Each transport_block entry is a byte, and TBS can always be expressed as an integer number of bytes (see, e.g.,
  // TS38.214 Section 5.1.3.2).
  units::bits nof_tb_bits     = units::bytes(transport_block.size()).to_bits();
  units::bits nof_tb_crc_bits = ldpc::compute_tb_crc_size(nof_tb_bits);
  hw_cfg.nof_tb_bits          = static_cast<unsigned>(nof_tb_bits.value());
  hw_cfg.nof_tb_crc_bits      = static_cast<unsigned>(nof_tb_crc_bits.value());

  // Number of segments.
  unsigned nof_segments = 0;
  if (cb_mode) {
    nof_segments = static_cast<unsigned>(d_segments.size());
  } else {
    nof_segments = ldpc::compute_nof_codeblocks(nof_tb_bits, cfg.base_graph);
  }
  hw_cfg.nof_segments = nof_segments;

  // TB size in bits, accounting for CRC bits.
  units::bits nof_tb_bits_out = nof_tb_bits + nof_tb_crc_bits;
  uint8_t     crc_byte_0      = 0;
  uint8_t     crc_byte_1      = 0;
  uint8_t     crc_byte_2      = 0;
  if (nof_segments > 1) {
    // Internally the CB CRC bits are also accounted for.
    nof_tb_bits_out += units::bits(nof_segments * SEG_CRC_LENGTH.value());
  }

  // Compute the TB CRC bits (needed for both FPGA and bbdev).
  crc_calculator_checksum_t tb_crc = compute_tb_crc(transport_block, hw_cfg.nof_tb_crc_bits);
  crc_byte_0                       = (static_cast<uint32_t>(tb_crc) >> 16) & 0xff;
  crc_byte_1                       = (static_cast<uint32_t>(tb_crc) >> 8) & 0xff;
  crc_byte_2                       = static_cast<uint32_t>(tb_crc) & 0xff;
  if (hw_cfg.nof_tb_crc_bits > 16) {
    hw_cfg.tb_crc = {crc_byte_0, crc_byte_1, crc_byte_2};
  } else {
    hw_cfg.tb_crc.resize(2);
    hw_cfg.tb_crc = {crc_byte_1, crc_byte_2};
  }

  // Number of channel symbols assigned to a transmission layer.
  unsigned nof_symbols_per_layer = cfg.nof_ch_symbols / cfg.nof_layers;
  // Number of segments that will have a short rate-matched length. In TS38.212 Section 5.4.2.1, these correspond to
  // codeblocks whose length E_r is computed by rounding down - floor. For the remaining codewords, the length is
  // rounded up.
  unsigned nof_short_segments = nof_segments - (nof_symbols_per_layer % nof_segments);
  hw_cfg.nof_short_segments   = nof_short_segments;

  // Codeword length for short rate-matched segments in bits (Ea). The FPGA will insert the CB CRC, thus the CB size
  // needs to be adjusted.
  unsigned           cw_length_a = 0;
  codeblock_metadata common_cfg;
  if (cb_mode) {
    common_cfg  = d_segments[0].get_metadata();
    cw_length_a = common_cfg.cb_specific.rm_length;
  } else {
    cw_length_a =
        compute_rm_length(0, nof_segments, nof_short_segments, nof_symbols_per_layer, cfg.mod, cfg.nof_layers);
  }
  hw_cfg.cw_length_a = cw_length_a;
  // Codeword length for long rate-matched segments in bits (Eb). The FPGA will insert the CB CRC, thus the CB size
  // needs to be adjusted.
  unsigned cw_length_b = cw_length_a;
  if (nof_segments > nof_short_segments) {
    if (cb_mode) {
      cw_length_b = d_segments[nof_short_segments].get_metadata().cb_specific.rm_length;
    } else {
      cw_length_b = compute_rm_length(
          nof_short_segments, nof_segments, nof_short_segments, nof_symbols_per_layer, cfg.mod, cfg.nof_layers);
    }
  }
  hw_cfg.cw_length_b = cw_length_b;

  // LDPC lifting size.
  unsigned lifting_size = 0;
  if (cb_mode) {
    lifting_size = common_cfg.tb_common.lifting_size;
  } else {
    lifting_size = ldpc::compute_lifting_size(nof_tb_bits, cfg.base_graph, nof_segments);
  }
  hw_cfg.lifting_size = lifting_size;

  // Base graph index and length of the circular buffer in bits, as described in TS38.212 Section 5.4.2.1.
  unsigned N              = 0;
  hw_cfg.base_graph_index = cfg.base_graph;
  if (cfg.base_graph == ldpc_base_graph_type::BG1) {
    N = lifting_size * 66;
  } else {
    N = lifting_size * 50;
  }
  hw_cfg.Ncb  = N;
  hw_cfg.Nref = cfg.Nref;

  // Number of CRC bits per segment.
  units::bits nof_crc_bits = (nof_segments > 1) ? SEG_CRC_LENGTH : 0_bits;

  // Number of information bits that is assigned to a segment.
  units::bits cb_info_bits = units::bits(divide_ceil(nof_tb_bits_out.value(), nof_segments)) - nof_crc_bits;
  hw_cfg.nof_segment_bits  = static_cast<unsigned>(cb_info_bits.value());

  if (!cb_mode) {
    // Number of filler bits.
    units::bits segment_length  = ldpc::compute_codeblock_size(cfg.base_graph, lifting_size);
    units::bits nof_filler_bits = segment_length - cb_info_bits - nof_crc_bits;
    hw_cfg.nof_filler_bits      = static_cast<unsigned>(nof_filler_bits.value());
  }
}

void pdsch_encoder_hw_impl::set_hw_enc_cb_configuration(hal::hw_pdsch_encoder_configuration& hw_cfg,
                                                        const codeblock_metadata&            cb_cfg,
                                                        unsigned                             cb_index)
{
  if (cb_mode) {
    // Number of filler bits in this segment.
    hw_cfg.nof_filler_bits = cb_cfg.cb_specific.nof_filler_bits;

    // Rate-matched length of this segment in bits.
    hw_cfg.rm_length = cb_cfg.cb_specific.rm_length;
  }

  // Set configuration in the HW accelerated encoder.
  encoder->configure_operation(hw_cfg, cb_index);
}

crc_calculator_checksum_t pdsch_encoder_hw_impl::compute_tb_crc(span<const uint8_t> transport_block,
                                                                unsigned            nof_tb_crc_bits)
{
  crc_calculator& tb_crc_calculator = (nof_tb_crc_bits == 16) ? *crc_set.crc16 : *crc_set.crc24A;
  return tb_crc_calculator.calculate_byte(transport_block);
}
