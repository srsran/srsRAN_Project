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

#include "channel_precoder_neon.h"
#include <arm_neon.h>

using namespace srsran;

namespace {

// Size of a NEON register in complex numbers with 32-bit floating point precision.
static constexpr unsigned NEON_CF_SIZE = 4;

// Size of a NEON register in complex numbers with 8-bit fixed point precision.
static constexpr unsigned NEON_CI8_SIZE = 16;

// Representation of a set of complex numbers using a pair of NEON registers, for the real and imaginary parts.
struct simd_cf_t {
  float32x4_t re;
  float32x4_t im;

  // Sets the registers using a complex constant.
  void set1(const cf_t a)
  {
    re = vdupq_n_f32(a.real());
    im = vdupq_n_f32(a.imag());
  }
};

// Type to hold a set of complex numbers using a NEON register, with interleaved real and imaginary parts.
using simd_cf_interleaved = float32x4x2_t;

simd_cf_interleaved operator+(simd_cf_interleaved a, simd_cf_interleaved b)
{
  simd_cf_interleaved ret;
  ret.val[0] = vaddq_f32(a.val[0], b.val[0]);
  ret.val[1] = vaddq_f32(a.val[1], b.val[1]);
  return ret;
}

// Multiplication operator for the precoding weights.
simd_cf_interleaved operator*(const simd_cf_interleaved& re, const simd_cf_t& weight)
{
  simd_cf_interleaved ret;
  ret.val[0] = vmlsq_f32(vmulq_f32(re.val[0], weight.re), re.val[1], weight.im);
  ret.val[1] = vmlaq_f32(vmulq_f32(re.val[0], weight.im), re.val[1], weight.re);
  return ret;
}

simd_cf_interleaved add_mul(const simd_cf_interleaved& sum, const simd_cf_interleaved& value, const simd_cf_t& weight)
{
  simd_cf_interleaved ret;
  ret.val[0] = vmlsq_f32(vmlaq_f32(sum.val[0], value.val[0], weight.re), value.val[1], weight.im);
  ret.val[1] = vmlaq_f32(vmlaq_f32(sum.val[1], value.val[0], weight.im), value.val[1], weight.re);
  return ret;
}

} // namespace

void channel_precoder_neon::apply_precoding_port(span<cf_t>              port_re,
                                                 const re_buffer_reader& input_re,
                                                 span<const cf_t>        port_weights) const
{
  unsigned nof_re     = input_re.get_nof_re();
  unsigned nof_layers = input_re.get_nof_slices();

  // Create a list of all the input layer RE views.
  std::array<span<const cf_t>, precoding_constants::MAX_NOF_LAYERS> layer_re_view_list;

  // Array holding SIMD registers initialized with the precoding weights.
  std::array<simd_cf_t, precoding_constants::MAX_NOF_LAYERS> port_weights_simd;

  for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
    // Fill the RE view list.
    layer_re_view_list[i_layer] = input_re.get_slice(i_layer);

    // Set the SIMD registers with the port weights.
    port_weights_simd[i_layer].set1(port_weights[i_layer]);
  }

  unsigned i_re = 0;

  for (unsigned max_re = (nof_re / NEON_CF_SIZE) * NEON_CF_SIZE; i_re != max_re; i_re += NEON_CF_SIZE) {
    // Load layer 0 RE.
    simd_cf_interleaved re_in = vld2q_f32(reinterpret_cast<const float*>(&layer_re_view_list[0][i_re]));

    // Multiply the input REs by the precoding coefficient.
    simd_cf_interleaved re_out = re_in * port_weights_simd[0];

    for (unsigned i_layer = 1; i_layer != nof_layers; ++i_layer) {
      // Load layer RE.
      re_in = vld2q_f32(reinterpret_cast<const float*>(&layer_re_view_list[i_layer][i_re]));

      // Multiply the input REs by the coefficient and add to the contributions of other layers.
      re_out = add_mul(re_out, re_in, port_weights_simd[i_layer]);
    }

    // Store.
    vst2q_f32(reinterpret_cast<float*>(&port_re[i_re]), re_out);
  }

  for (; i_re != nof_re; ++i_re) {
    port_re[i_re] = layer_re_view_list[0][i_re] * port_weights[0];

    for (unsigned i_layer = 1; i_layer != nof_layers; ++i_layer) {
      port_re[i_re] += layer_re_view_list[i_layer][i_re] * port_weights[i_layer];
    }
  }
}

static inline void from_ci8_to_ci16(int16x8x2_t& out0, int16x8x2_t& out1, int8x16x2_t in)
{
  out0.val[0] = vmovl_s8(vget_low_s8(in.val[0]));
  out0.val[1] = vmovl_s8(vget_low_s8(in.val[1]));

  out1.val[0] = vmovl_high_s8(in.val[0]);
  out1.val[1] = vmovl_high_s8(in.val[1]);
};

static inline void from_ci16_to_ci32(int32x4x2_t& out0, int32x4x2_t& out1, int16x8x2_t in)
{
  out0.val[0] = vmovl_s16(vget_low_s16(in.val[0]));
  out0.val[1] = vmovl_s16(vget_low_s16(in.val[1]));

  out1.val[0] = vmovl_high_s16(in.val[0]);
  out1.val[1] = vmovl_high_s16(in.val[1]);
};

static inline void from_ci32_to_cf(float32x4x2_t& out, int32x4x2_t in)
{
  out.val[0] = vcvtq_f32_s32(in.val[0]);
  out.val[1] = vcvtq_f32_s32(in.val[1]);
};

// Converts ci8_t symbols into cf_t and stores them on four NEON registers.
static inline void from_ci8_to_cf(simd_cf_interleaved& out0,
                                  simd_cf_interleaved& out1,
                                  simd_cf_interleaved& out2,
                                  simd_cf_interleaved& out3,
                                  const int8x16x2_t    in)
{
  // Convert input to ci16.
  int16x8x2_t in_ci16_0, in_ci16_1;
  from_ci8_to_ci16(in_ci16_0, in_ci16_1, in);

  // Convert input to ci32.
  int32x4x2_t in_ci32_0, in_ci32_1, in_ci32_2, in_ci32_3;
  from_ci16_to_ci32(in_ci32_0, in_ci32_1, in_ci16_0);
  from_ci16_to_ci32(in_ci32_2, in_ci32_3, in_ci16_1);

  // Convert input to cf.
  from_ci32_to_cf(out0, in_ci32_0);
  from_ci32_to_cf(out1, in_ci32_1);
  from_ci32_to_cf(out2, in_ci32_2);
  from_ci32_to_cf(out3, in_ci32_3);
}

// Applies layer mapping for two layers and converts the symbols to cf_t.
static inline void layer2_map_and_ci8_to_cf(simd_cf_interleaved& out0_l0,
                                            simd_cf_interleaved& out0_l1,
                                            simd_cf_interleaved& out1_l0,
                                            simd_cf_interleaved& out1_l1,
                                            const int8x16x2_t    in)
{
  // The input is de-interleaved in real and imaginary, this step interleaves them again to produce the linear input in
  // temp in an array of words of 16bits.
  int16_t temp[NEON_CI8_SIZE];
  vst2q_s8(reinterpret_cast<int8_t*>(temp), in);

  // Load the temporary data with de-interleaving words of 16 bits and store them de-interleaved.
  int16x8x2_t deinterleaved0 = vld2q_s16(temp);
  vst1q_s16(&temp[0], deinterleaved0.val[0]);
  vst1q_s16(&temp[8], deinterleaved0.val[1]);

  // Load back to de-interleaved real and imaginary.
  int8x16x2_t out = vld2q_s8(reinterpret_cast<const int8_t*>(temp));

  from_ci8_to_cf(out0_l0, out1_l0, out0_l1, out1_l1, out);
}

// Applies layer mapping for three layers and converts the symbols to cf_t.
static inline void layer3_map_and_ci8_to_cf(simd_cf_interleaved& out_l0,
                                            simd_cf_interleaved& out_l1,
                                            simd_cf_interleaved& out_l2,
                                            const int8x16x2_t    in)
{
  simd_cf_interleaved unused;

  ci8_t temp_in[NEON_CI8_SIZE];
  ci8_t temp_out[NEON_CI8_SIZE];
  vst2q_s8(reinterpret_cast<int8_t*>(temp_in), in);
  unsigned k = 0;
  for (unsigned i_layer = 0; i_layer != 3; ++i_layer) {
    for (unsigned i_re = 0; i_re != NEON_CF_SIZE; ++i_re) {
      temp_out[k++] = temp_in[3 * i_re + i_layer];
    }
  }
  for (unsigned i_re = 0; i_re != NEON_CF_SIZE; ++i_re) {
    temp_out[k++] = 0;
  }
  int8x16x2_t out = vld2q_s8(reinterpret_cast<const int8_t*>(temp_out));

  from_ci8_to_cf(out_l0, out_l1, out_l2, unused, out);
}

// Applies layer mapping for four layers and converts the symbols to cf_t.
static inline void layer4_map_and_ci8_to_cf(simd_cf_interleaved& out_l0,
                                            simd_cf_interleaved& out_l1,
                                            simd_cf_interleaved& out_l2,
                                            simd_cf_interleaved& out_l3,
                                            const int8x16x2_t    in)
{
  // The input is de-interleaved in real and imaginary, this step interleaves them again to produce the linear input in
  // temp in an array of words of 16bits.
  int16_t temp[NEON_CI8_SIZE];
  vst2q_s8(reinterpret_cast<int8_t*>(temp), in);

  // Load the temporary data with de-interleaving words of 16 bits and store them de-interleaved.
  int16x8x2_t deinterleaved0 = vld2q_s16(temp);
  vst1q_s16(&temp[0], deinterleaved0.val[0]);
  vst1q_s16(&temp[8], deinterleaved0.val[1]);

  // Repeat de-interleaving.
  int16x8x2_t deinterleaved1 = vld2q_s16(temp);
  vst1q_s16(&temp[0], deinterleaved1.val[0]);
  vst1q_s16(&temp[8], deinterleaved1.val[1]);

  // Load back to de-interleaved real and imaginary.
  int8x16x2_t out = vld2q_s8(reinterpret_cast<const int8_t*>(temp));

  from_ci8_to_cf(out_l0, out_l1, out_l2, out_l3, out);
}

template <unsigned NofPorts>
void template_apply_layer_map_and_precoding(re_buffer_writer&              output,
                                            span<const ci8_t>              input,
                                            const precoding_weight_matrix& precoding)
{
  // Extract precoding parameters.
  unsigned nof_re     = output.get_nof_re();
  unsigned nof_layers = precoding.get_nof_layers();
  unsigned nof_ports  = precoding.get_nof_ports();

  // If the number of ports does not match, try with another one.
  if (nof_ports != NofPorts) {
    template_apply_layer_map_and_precoding<NofPorts - 1>(output, input, precoding);
    return;
  }

  // Registers to load the precoding weights.
  simd_cf_t weights[precoding_constants::MAX_NOF_PORTS][precoding_constants::MAX_NOF_LAYERS];

  // Views to store the precoded symbols.
  span<cf_t> outputs[precoding_constants::MAX_NOF_PORTS];

  // Convert weight matrix to SIMD types, and select output data views.
  for (unsigned i_port = 0; i_port != NofPorts; ++i_port) {
    span<const cf_t> port_coeff = precoding.get_port_coefficients(i_port);
    outputs[i_port]             = output.get_slice(i_port);
    for (unsigned i_layer = 0; i_layer != nof_layers; ++i_layer) {
      weights[i_port][i_layer].set1(port_coeff[i_layer]);
    }
  }

  unsigned i_re = 0;

  // Optimized case for one layer and more than one port.
  if (nof_layers == 1) {
    for (unsigned i_re_end = (nof_re / NEON_CI8_SIZE) * NEON_CI8_SIZE; i_re != i_re_end; i_re += NEON_CI8_SIZE) {
      // Load input.
      int8x16x2_t in8 = vld2q_s8(reinterpret_cast<const int8_t*>(input.data() + i_re));

      // Convert to complex float.
      simd_cf_interleaved infp_0, infp_1, infp_2, infp_3;
      from_ci8_to_cf(infp_0, infp_1, infp_2, infp_3, in8);

      for (unsigned i_port = 0; i_port != NofPorts; ++i_port) {
        // Apply precoding.
        simd_cf_interleaved result0 = infp_0 * weights[i_port][0];
        simd_cf_interleaved result1 = infp_1 * weights[i_port][0];
        simd_cf_interleaved result2 = infp_2 * weights[i_port][0];
        simd_cf_interleaved result3 = infp_3 * weights[i_port][0];

        // Store.
        vst2q_f32(reinterpret_cast<float*>(&outputs[i_port][i_re]), result0);
        vst2q_f32(reinterpret_cast<float*>(&outputs[i_port][i_re + NEON_CF_SIZE]), result1);
        vst2q_f32(reinterpret_cast<float*>(&outputs[i_port][i_re + (2 * NEON_CF_SIZE)]), result2);
        vst2q_f32(reinterpret_cast<float*>(&outputs[i_port][i_re + (3 * NEON_CF_SIZE)]), result3);
      }
    }
  }

  // Optimized case for two layer.
  if (nof_layers == 2) {
    static constexpr unsigned NEON_NOF_RE = NEON_CI8_SIZE / 2;
    for (unsigned i_re_end = (nof_re / NEON_NOF_RE) * NEON_NOF_RE; i_re != i_re_end; i_re += NEON_NOF_RE) {
      // Load input.
      int8x16x2_t in8 = vld2q_s8(reinterpret_cast<const int8_t*>(&input[2 * i_re]));

      // Layer map and convert to complex float.
      simd_cf_interleaved infp0_l0, infp0_l1, infp1_l0, infp1_l1;
      layer2_map_and_ci8_to_cf(infp0_l0, infp0_l1, infp1_l0, infp1_l1, in8);

      for (unsigned i_port = 0; i_port != NofPorts; ++i_port) {
        // Apply precoding.
        simd_cf_interleaved result0 = infp0_l0 * weights[i_port][0] + infp0_l1 * weights[i_port][1];
        simd_cf_interleaved result1 = infp1_l0 * weights[i_port][0] + infp1_l1 * weights[i_port][1];

        // Store.
        vst2q_f32(reinterpret_cast<float*>(&outputs[i_port][i_re]), result0);
        vst2q_f32(reinterpret_cast<float*>(&outputs[i_port][i_re + NEON_CF_SIZE]), result1);
      }
    }
  }

  // Optimized case for three layer.
  if (nof_layers == 3) {
    static constexpr unsigned NEON_NOF_RE = NEON_CI8_SIZE / 4;
    // For three layers, more input symbols are loaded than those actually used. The last loop iteration must be skipped
    // in order to avoid exceeding the input buffer boundaries.
    unsigned i_re_end = (nof_re > NEON_NOF_RE) ? (nof_re / NEON_NOF_RE - 1) * NEON_NOF_RE : 0U;
    for (; i_re != i_re_end; i_re += NEON_NOF_RE) {
      // Load input.
      int8x16x2_t in8 = vld2q_s8(reinterpret_cast<const int8_t*>(&input[3 * i_re]));

      // Layer map and convert to complex float.
      simd_cf_interleaved infp_0, infp_1, infp_2;
      layer3_map_and_ci8_to_cf(infp_0, infp_1, infp_2, in8);

      for (unsigned i_port = 0; i_port != NofPorts; ++i_port) {
        // Apply precoding.
        simd_cf_interleaved result = infp_0 * weights[i_port][0];
        result                     = add_mul(result, infp_1, weights[i_port][1]);
        result                     = add_mul(result, infp_2, weights[i_port][2]);

        // Store.
        vst2q_f32(reinterpret_cast<float*>(&outputs[i_port][i_re]), result);
      }
    }
  }

  // Optimized case for four layer.
  if (nof_layers == 4) {
    static constexpr unsigned NEON_NOF_RE = NEON_CI8_SIZE / 4;
    for (unsigned i_re_end = (nof_re / NEON_NOF_RE) * NEON_NOF_RE; i_re != i_re_end; i_re += NEON_NOF_RE) {
      // Load input.
      int8x16x2_t in8 = vld2q_s8(reinterpret_cast<const int8_t*>(&input[4 * i_re]));

      // Layer map and convert to complex float.
      simd_cf_interleaved infp_0, infp_1, infp_2, infp_3;
      layer4_map_and_ci8_to_cf(infp_0, infp_1, infp_2, infp_3, in8);

      for (unsigned i_port = 0; i_port != NofPorts; ++i_port) {
        // Apply precoding.
        simd_cf_interleaved result = infp_0 * weights[i_port][0];
        result                     = add_mul(result, infp_1, weights[i_port][1]);
        result                     = add_mul(result, infp_2, weights[i_port][2]);
        result                     = add_mul(result, infp_3, weights[i_port][3]);

        // Store.
        vst2q_f32(reinterpret_cast<float*>(&outputs[i_port][i_re]), result);
      }
    }
  }

  // Process remaining symbols.
  for (; i_re != nof_re; ++i_re) {
    for (unsigned i_port = 0; i_port != NofPorts; ++i_port) {
      span<const cf_t> port_weights = precoding.get_port_coefficients(i_port);
      span<cf_t>       port_re      = output.get_slice(i_port);

      cf_t sum = to_cf(input[nof_layers * i_re]) * port_weights[0];
      for (unsigned i_layer = 1; i_layer != nof_layers; ++i_layer) {
        sum += to_cf(input[nof_layers * i_re + i_layer]) * port_weights[i_layer];
      }
      port_re[i_re] = sum;
    }
  }
}

template <>
void template_apply_layer_map_and_precoding<1>(re_buffer_writer&              output,
                                               span<const ci8_t>              input,
                                               const precoding_weight_matrix& precoding)
{
  // Extract precoding parameters.
  unsigned nof_re     = output.get_nof_re();
  unsigned nof_layers = precoding.get_nof_layers();
  unsigned nof_ports  = precoding.get_nof_ports();

  // Make sure the number of ports is valid.
  srsran_assert(nof_ports == 1, "Invalid number of ports (i.e., {}).", nof_ports);

  simd_cf_t weight_neon;
  cf_t      weight_cft = precoding.get_coefficient(0, 0);
  weight_neon.set1(weight_cft);

  span<cf_t> out = output.get_slice(0);

  unsigned i_re = 0;

  // Optimized precoding for one layer and one port.
  for (unsigned i_re_end = (nof_re / NEON_CI8_SIZE) * NEON_CI8_SIZE; i_re != i_re_end; i_re += NEON_CI8_SIZE) {
    // Load input.
    int8x16x2_t in8 = vld2q_s8(reinterpret_cast<const int8_t*>(input.data() + i_re));

    // Convert to complex float.
    simd_cf_interleaved infp_0, infp_1, infp_2, infp_3;
    from_ci8_to_cf(infp_0, infp_1, infp_2, infp_3, in8);

    // Apply precoding.
    simd_cf_interleaved result0 = infp_0 * weight_neon;
    simd_cf_interleaved result1 = infp_1 * weight_neon;
    simd_cf_interleaved result2 = infp_2 * weight_neon;
    simd_cf_interleaved result3 = infp_3 * weight_neon;

    // Store.
    vst2q_f32(reinterpret_cast<float*>(&out[i_re]), result0);
    vst2q_f32(reinterpret_cast<float*>(&out[i_re + NEON_CF_SIZE]), result1);
    vst2q_f32(reinterpret_cast<float*>(&out[i_re + (2 * NEON_CF_SIZE)]), result2);
    vst2q_f32(reinterpret_cast<float*>(&out[i_re + (3 * NEON_CF_SIZE)]), result3);
  }

  // Process remaining symbols.
  for (; i_re != nof_re; ++i_re) {
    out[i_re] = to_cf(input[nof_layers * i_re]) * weight_cft;
  }
}

void channel_precoder_neon::apply_layer_map_and_precoding(re_buffer_writer&              output,
                                                          span<const ci8_t>              input,
                                                          const precoding_weight_matrix& precoding) const
{
  template_apply_layer_map_and_precoding<precoding_constants::MAX_NOF_PORTS>(output, input, precoding);
}
