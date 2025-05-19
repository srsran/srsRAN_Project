/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "dmrs_helper.h"

using namespace srsran;

static const re_prb_mask RE_PATTERN_TYPE1_DELTA0 =
    {true, false, true, false, true, false, true, false, true, false, true, false};
static const re_prb_mask RE_PATTERN_TYPE1_DELTA1 =
    {false, true, false, true, false, true, false, true, false, true, false, true};
static const re_prb_mask RE_PATTERN_TYPE2_DELTA0 =
    {true, true, false, false, false, false, true, true, false, false, false, false};
static const re_prb_mask RE_PATTERN_TYPE2_DELTA2 =
    {false, false, true, true, false, false, false, false, true, true, false, false};
static const re_prb_mask RE_PATTERN_TYPE2_DELTA4 =
    {false, false, false, false, true, true, false, false, false, false, true, true};

static const std::array<dmrs_pxsch_parameters, dmrs_type::DMRS_MAX_PORTS_TYPE1> params_type1 = {{
    /* Port 1000 */ {RE_PATTERN_TYPE1_DELTA0, {+1.0F, +1.0F}, {+1.0F, +1.0F}},
    /* Port 1001 */ {RE_PATTERN_TYPE1_DELTA0, {+1.0F, -1.0F}, {+1.0F, +1.0F}},
    /* Port 1002 */ {RE_PATTERN_TYPE1_DELTA1, {+1.0F, +1.0F}, {+1.0F, +1.0F}},
    /* Port 1003 */ {RE_PATTERN_TYPE1_DELTA1, {+1.0F, -1.0F}, {+1.0F, +1.0F}},
    /* Port 1004 */ {RE_PATTERN_TYPE1_DELTA0, {+1.0F, +1.0F}, {+1.0F, -1.0F}},
    /* Port 1005 */ {RE_PATTERN_TYPE1_DELTA0, {+1.0F, -1.0F}, {+1.0F, -1.0F}},
    /* Port 1006 */ {RE_PATTERN_TYPE1_DELTA1, {+1.0F, +1.0F}, {+1.0F, -1.0F}},
    /* Port 1007 */ {RE_PATTERN_TYPE1_DELTA1, {+1.0F, -1.0F}, {+1.0F, -1.0F}},
}};

static const std::array<dmrs_pxsch_parameters, dmrs_type::DMRS_MAX_PORTS_TYPE2> params_type2 = {{
    /* Port 1000 */ {RE_PATTERN_TYPE2_DELTA0, {+1.0F, +1.0F}, {+1.0F, +1.0F}},
    /* Port 1001 */ {RE_PATTERN_TYPE2_DELTA0, {+1.0F, -1.0F}, {+1.0F, +1.0F}},
    /* Port 1002 */ {RE_PATTERN_TYPE2_DELTA2, {+1.0F, +1.0F}, {+1.0F, +1.0F}},
    /* Port 1003 */ {RE_PATTERN_TYPE2_DELTA2, {+1.0F, -1.0F}, {+1.0F, +1.0F}},
    /* Port 1004 */ {RE_PATTERN_TYPE2_DELTA4, {+1.0F, +1.0F}, {+1.0F, +1.0F}},
    /* Port 1005 */ {RE_PATTERN_TYPE2_DELTA4, {+1.0F, -1.0F}, {+1.0F, +1.0F}},
    /* Port 1006 */ {RE_PATTERN_TYPE2_DELTA0, {+1.0F, +1.0F}, {+1.0F, -1.0F}},
    /* Port 1007 */ {RE_PATTERN_TYPE2_DELTA0, {+1.0F, -1.0F}, {+1.0F, -1.0F}},
    /* Port 1008 */ {RE_PATTERN_TYPE2_DELTA2, {+1.0F, +1.0F}, {+1.0F, -1.0F}},
    /* Port 1009 */ {RE_PATTERN_TYPE2_DELTA2, {+1.0F, -1.0F}, {+1.0F, -1.0F}},
    /* Port 1010 */ {RE_PATTERN_TYPE2_DELTA4, {+1.0F, +1.0F}, {+1.0F, -1.0F}},
    /* Port 1011 */ {RE_PATTERN_TYPE2_DELTA4, {+1.0F, -1.0F}, {+1.0F, -1.0F}},
}};

void srsran::dmrs_sequence_generate(span<srsran::cf_t>       sequence,
                                    pseudo_random_generator& prg,
                                    float                    amplitude,
                                    unsigned                 reference_point_k_rb,
                                    unsigned                 nof_dmrs_per_rb,
                                    const prb_bitmap&        rb_mask)
{
  unsigned nof_dmrs_symbols = nof_dmrs_per_rb * rb_mask.count();
  srsran_assert(sequence.size() == nof_dmrs_symbols,
                "DM-RS sequence size, i.e., {} does not match the number of RE allocated for DM-RS, i.e., {}.",
                sequence.size(),
                nof_dmrs_symbols);

  for_each_interval(
      rb_mask, [&sequence, &prg, amplitude, &reference_point_k_rb, nof_dmrs_per_rb](size_t rb_begin, size_t rb_end) {
        // Skip DM-RS in between the current reference and the first RB of the interval.
        unsigned nof_rb_skip = rb_begin - reference_point_k_rb;
        prg.advance(nof_rb_skip * nof_dmrs_per_rb * 2);

        // Count the number of resource blocks.
        unsigned rb_count = rb_end - rb_begin;

        // Generate DM-RS for the interval of resource blocks.
        prg.generate(sequence.first(rb_count * nof_dmrs_per_rb), amplitude);

        // Advance sequence buffer.
        sequence = sequence.last(sequence.size() - rb_count * nof_dmrs_per_rb);

        // Update reference to the end RB.
        reference_point_k_rb = rb_end;
      });
}

dmrs_pxsch_parameters srsran::get_pxsch_dmrs_params(dmrs_type type, unsigned i_dmrs_port)
{
  // Select view of the selected table.
  span<const dmrs_pxsch_parameters> params = (type == dmrs_type::options::TYPE1)
                                                 ? span<const dmrs_pxsch_parameters>(params_type1)
                                                 : span<const dmrs_pxsch_parameters>(params_type2);

  // Verify the layer index is within the range.
  [[maybe_unused]] interval<unsigned> i_dmrs_port_range(0, params.size());
  srsran_assert(i_dmrs_port_range.contains(i_dmrs_port),
                "The DM-RS port index (i.e., {}) for DM-RS {} is out of the range {}.",
                i_dmrs_port,
                type.to_string(),
                i_dmrs_port_range);

  // Return layer parameters.
  return params[i_dmrs_port];
}
