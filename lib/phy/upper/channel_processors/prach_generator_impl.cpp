/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "prach_generator_impl.h"
#include "prach_preamble_helpers.h"
#include "srsgnb/phy/constants.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/srsvec/sc_prod.h"
#include "srsgnb/srsvec/zero.h"
#include "srsgnb/support/math_utils.h"

using namespace srsgnb;

template <unsigned L>
class prach_generator_cexp_table : public std::array<cf_t, 2 * L>
{
public:
  prach_generator_cexp_table()
  {
    std::generate(this->begin(), this->end(), [n = 0]() mutable {
      return std::exp(-COMPLEX_I * static_cast<float>(M_PI) * static_cast<float>(n++) / static_cast<float>(L));
    });
  }
};

static prach_generator_cexp_table<839U> prach_generator_cexp_table_l839;

unsigned prach_generator_impl::get_nof_cyclic_shifts(unsigned              prach_scs,
                                                     restricted_set_config restricted_set,
                                                     unsigned              zero_correlation_zone)
{
  // TS38.211 Table 6.3.3.1-5 First column.
  static const std::array<unsigned, 16> NOF_CYCLIC_SHIFTS_1_DOT_25_UNRESTRICTED = {
      0, 13, 15, 18, 22, 26, 32, 38, 46, 59, 76, 93, 119, 167, 279, 419};
  // TS38.211 Table 6.3.3.1-5 Second column.
  static const std::array<unsigned, 16> NOF_CYCLIC_SHIFTS_1_DOT_25_TYPE_A = {
      15, 18, 22, 26, 32, 38, 46, 55, 68, 82, 100, 128, 158, 202, 237, RESERVED};
  // TS38.211 Table 6.3.3.1-5 Third column.
  static const std::array<unsigned, 16> NOF_CYCLIC_SHIFTS_1_DOT_25_TYPE_B = {
      15, 18, 22, 26, 32, 38, 46, 55, 68, 82, 100, 118, 137, RESERVED, RESERVED, RESERVED};

  // TS38.211 Table 6.3.3.1-6 First column.
  static const std::array<unsigned, 16> NOF_CYCLIC_SHIFTS_5_UNRESTRICTED = {
      0, 13, 26, 33, 38, 41, 49, 55, 64, 76, 93, 119, 139, 209, 279, 419};
  // TS38.211 Table 6.3.3.1-6 Second column.
  static const std::array<unsigned, 16> NOF_CYCLIC_SHIFTS_5_TYPE_A = {
      36, 57, 72, 81, 89, 94, 103, 112, 121, 132, 137, 152, 173, 195, 216, 237};
  // TS38.211 Table 6.3.3.1-6 Third column.
  static const std::array<unsigned, 16> NOF_CYCLIC_SHIFTS_5_TYPE_B = {
      36, 57, 60, 63, 65, 68, 71, 77, 81, 85, 97, 109, 122, 137, RESERVED, RESERVED};

  if (prach_scs == 1250) {
    switch (restricted_set) {
      case restricted_set_config::UNRESTRICTED:
        return NOF_CYCLIC_SHIFTS_1_DOT_25_UNRESTRICTED[zero_correlation_zone];
      case restricted_set_config::TYPE_A:
        return NOF_CYCLIC_SHIFTS_1_DOT_25_TYPE_A[zero_correlation_zone];
      case restricted_set_config::TYPE_B:
        return NOF_CYCLIC_SHIFTS_1_DOT_25_TYPE_B[zero_correlation_zone];
    }
  }

  if (prach_scs == 5000) {
    switch (restricted_set) {
      case restricted_set_config::UNRESTRICTED:
        return NOF_CYCLIC_SHIFTS_5_UNRESTRICTED[zero_correlation_zone];
      case restricted_set_config::TYPE_A:
        return NOF_CYCLIC_SHIFTS_5_TYPE_A[zero_correlation_zone];
      case restricted_set_config::TYPE_B:
        return NOF_CYCLIC_SHIFTS_5_TYPE_B[zero_correlation_zone];
    }
  }

  return RESERVED;
}

unsigned prach_generator_impl::get_sequence_length(preamble_format format)
{
  srsran_assert(format.is_long_preamble(), "Short format not implemented.");

  return 839;
}

unsigned prach_generator_impl::get_sequence_number_long(unsigned root_sequence_index)
{
  static const std::array<uint16_t, 838> lut = {
      129, 710, 140, 699, 120, 719, 210, 629, 168, 671, 84,  755, 105, 734, 93,  746, 70,  769, 60,  779, 2,   837, 1,
      838, 56,  783, 112, 727, 148, 691, 80,  759, 42,  797, 40,  799, 35,  804, 73,  766, 146, 693, 31,  808, 28,  811,
      30,  809, 27,  812, 29,  810, 24,  815, 48,  791, 68,  771, 74,  765, 178, 661, 136, 703, 86,  753, 78,  761, 43,
      796, 39,  800, 20,  819, 21,  818, 95,  744, 202, 637, 190, 649, 181, 658, 137, 702, 125, 714, 151, 688, 217, 622,
      128, 711, 142, 697, 122, 717, 203, 636, 118, 721, 110, 729, 89,  750, 103, 736, 61,  778, 55,  784, 15,  824, 14,
      825, 12,  827, 23,  816, 34,  805, 37,  802, 46,  793, 207, 632, 179, 660, 145, 694, 130, 709, 223, 616, 228, 611,
      227, 612, 132, 707, 133, 706, 143, 696, 135, 704, 161, 678, 201, 638, 173, 666, 106, 733, 83,  756, 91,  748, 66,
      773, 53,  786, 10,  829, 9,   830, 7,   832, 8,   831, 16,  823, 47,  792, 64,  775, 57,  782, 104, 735, 101, 738,
      108, 731, 208, 631, 184, 655, 197, 642, 191, 648, 121, 718, 141, 698, 149, 690, 216, 623, 218, 621, 152, 687, 144,
      695, 134, 705, 138, 701, 199, 640, 162, 677, 176, 663, 119, 720, 158, 681, 164, 675, 174, 665, 171, 668, 170, 669,
      87,  752, 169, 670, 88,  751, 107, 732, 81,  758, 82,  757, 100, 739, 98,  741, 71,  768, 59,  780, 65,  774, 50,
      789, 49,  790, 26,  813, 17,  822, 13,  826, 6,   833, 5,   834, 33,  806, 51,  788, 75,  764, 99,  740, 96,  743,
      97,  742, 166, 673, 172, 667, 175, 664, 187, 652, 163, 676, 185, 654, 200, 639, 114, 725, 189, 650, 115, 724, 194,
      645, 195, 644, 192, 647, 182, 657, 157, 682, 156, 683, 211, 628, 154, 685, 123, 716, 139, 700, 212, 627, 153, 686,
      213, 626, 215, 624, 150, 689, 225, 614, 224, 615, 221, 618, 220, 619, 127, 712, 147, 692, 124, 715, 193, 646, 205,
      634, 206, 633, 116, 723, 160, 679, 186, 653, 167, 672, 79,  760, 85,  754, 77,  762, 92,  747, 58,  781, 62,  777,
      69,  770, 54,  785, 36,  803, 32,  807, 25,  814, 18,  821, 11,  828, 4,   835, 3,   836, 19,  820, 22,  817, 41,
      798, 38,  801, 44,  795, 52,  787, 45,  794, 63,  776, 67,  772, 72,  767, 76,  763, 94,  745, 102, 737, 90,  749,
      109, 730, 165, 674, 111, 728, 209, 630, 204, 635, 117, 722, 188, 651, 159, 680, 198, 641, 113, 726, 183, 656, 180,
      659, 177, 662, 196, 643, 155, 684, 214, 625, 126, 713, 131, 708, 219, 620, 222, 617, 226, 613, 230, 609, 232, 607,
      262, 577, 252, 587, 418, 421, 416, 423, 413, 426, 411, 428, 376, 463, 395, 444, 283, 556, 285, 554, 379, 460, 390,
      449, 363, 476, 384, 455, 388, 451, 386, 453, 361, 478, 387, 452, 360, 479, 310, 529, 354, 485, 328, 511, 315, 524,
      337, 502, 349, 490, 335, 504, 324, 515, 323, 516, 320, 519, 334, 505, 359, 480, 295, 544, 385, 454, 292, 547, 291,
      548, 381, 458, 399, 440, 380, 459, 397, 442, 369, 470, 377, 462, 410, 429, 407, 432, 281, 558, 414, 425, 247, 592,
      277, 562, 271, 568, 272, 567, 264, 575, 259, 580, 237, 602, 239, 600, 244, 595, 243, 596, 275, 564, 278, 561, 250,
      589, 246, 593, 417, 422, 248, 591, 394, 445, 393, 446, 370, 469, 365, 474, 300, 539, 299, 540, 364, 475, 362, 477,
      298, 541, 312, 527, 313, 526, 314, 525, 353, 486, 352, 487, 343, 496, 327, 512, 350, 489, 326, 513, 319, 520, 332,
      507, 333, 506, 348, 491, 347, 492, 322, 517, 330, 509, 338, 501, 341, 498, 340, 499, 342, 497, 301, 538, 366, 473,
      401, 438, 371, 468, 408, 431, 375, 464, 249, 590, 269, 570, 238, 601, 234, 605, 257, 582, 273, 566, 255, 584, 254,
      585, 245, 594, 251, 588, 412, 427, 372, 467, 282, 557, 403, 436, 396, 443, 392, 447, 391, 448, 382, 457, 389, 450,
      294, 545, 297, 542, 311, 528, 344, 495, 345, 494, 318, 521, 331, 508, 325, 514, 321, 518, 346, 493, 339, 500, 351,
      488, 306, 533, 289, 550, 400, 439, 378, 461, 374, 465, 415, 424, 270, 569, 241, 598, 231, 608, 260, 579, 268, 571,
      276, 563, 409, 430, 398, 441, 290, 549, 304, 535, 308, 531, 358, 481, 316, 523, 293, 546, 288, 551, 284, 555, 368,
      471, 253, 586, 256, 583, 263, 576, 242, 597, 274, 565, 402, 437, 383, 456, 357, 482, 329, 510, 317, 522, 307, 532,
      286, 553, 287, 552, 266, 573, 261, 578, 236, 603, 303, 536, 356, 483, 355, 484, 405, 434, 404, 435, 406, 433, 235,
      604, 267, 572, 302, 537, 309, 530, 265, 574, 233, 606, 367, 472, 296, 543, 336, 503, 305, 534, 373, 466, 280, 559,
      279, 560, 419, 420, 240, 599, 258, 581, 229, 610};

  return lut[root_sequence_index % lut.size()];
}

span<const cf_t> prach_generator_impl::generate_y_u_v_long(unsigned sequence_number, unsigned cyclic_shift)
{
  span<cf_t> x_u_v = dft_l839->get_input();
  for (unsigned n = 0; n != 839; ++n) {
    uint64_t x_u_v_index = (sequence_number * n * (n + 1UL)) % prach_generator_cexp_table_l839.size();
    x_u_v[(n + (839 - cyclic_shift)) % 839] = prach_generator_cexp_table_l839[x_u_v_index];
  }

  return dft_l839->run();
}

unsigned prach_generator_impl::get_N_rb_ra(unsigned prach_scs_Hz, unsigned pusch_scs_Hz)
{
  if (prach_scs_Hz == 1250) {
    if (pusch_scs_Hz == 15000) {
      return 6;
    }
    if (pusch_scs_Hz == 30000) {
      return 3;
    }
    if (pusch_scs_Hz == 60000) {
      return 2;
    }
  }
  if (prach_scs_Hz == 5000) {
    if (pusch_scs_Hz == 15000) {
      return 24;
    }
    if (pusch_scs_Hz == 30000) {
      return 12;
    }
    if (pusch_scs_Hz == 60000) {
      return 6;
    }
  }
  return RESERVED;
}

unsigned prach_generator_impl::get_k_bar(unsigned prach_scs_Hz, unsigned pusch_scs_Hz)
{
  if (prach_scs_Hz == 1250) {
    if (pusch_scs_Hz == 15000) {
      return 7;
    }
    if (pusch_scs_Hz == 30000) {
      return 1;
    }
    if (pusch_scs_Hz == 60000) {
      return 133;
    }
  }
  if (prach_scs_Hz == 5000) {
    if (pusch_scs_Hz == 15000) {
      return 12;
    }
    if (pusch_scs_Hz == 30000) {
      return 10;
    }
    if (pusch_scs_Hz == 60000) {
      return 7;
    }
  }
  return RESERVED;
}

span<const cf_t> prach_generator_impl::modulate(span<const cf_t> y_u_v, const configuration& config)
{
  srsran_assert(config.format.is_long_preamble(), "Short preambles are not implemented.");
  prach_preamble_modulation_info info         = prach_preamble_modulation_info_get_long(config.format, dft_size_15kHz);
  unsigned                       prach_scs_Hz = info.scs.to_Hz();
  unsigned                       pusch_scs_Hz = scs_to_khz(config.pusch_scs) * 1000;
  unsigned                       L_ra         = get_sequence_length(config.format);

  // Select DFT to generate the time domain sequence.
  dft_processor* dft = dft_1_dot_25_kHz.get();
  if (prach_scs_Hz == 5000) {
    dft = dft_5_kHz.get();
  }
  span<cf_t> dft_input = dft->get_input();

  unsigned K               = pusch_scs_Hz / prach_scs_Hz;
  unsigned prach_grid_size = nof_prb_ul_grid * K * NRE;
  srsran_assert(dft->get_size() > prach_grid_size,
                "DFT size {} for PRACH SCS {} is not sufficient for K={}, N_RB={}. It requires {}.",
                dft->get_size(),
                prach_scs_Hz,
                K,
                nof_prb_ul_grid,
                prach_grid_size);

  unsigned k_bar = get_k_bar(prach_scs_Hz, pusch_scs_Hz);
  srsran_assert(k_bar != RESERVED, "Configuration leads to a reserved value.");

  unsigned k_start = K * NRE * config.rb_offset + k_bar;
  srsran_assert(k_start + L_ra < prach_grid_size,
                "Start subcarrier {} plus sequence length {} exceeds PRACH grid size {}.",
                k_start,
                L_ra,
                prach_grid_size);

  float gain = std::sqrt(1.0F / static_cast<float>(dft_input.size() * L_ra * dft_size_15kHz /
                                                   pow2(to_numerology_value(config.pusch_scs))));
  srsran_assert(std::isnormal(gain), "Invalid gain.");

  // Set DFT input to zero.
  srsvec::zero(dft_input);

  // Create views of the lower and upper grid.
  span<cf_t> lower_grid = dft_input.last(prach_grid_size / 2);
  span<cf_t> upper_grid = dft_input.first(prach_grid_size / 2);

  // If the sequence map starts at the lower half.
  if (k_start < prach_grid_size / 2) {
    // Number of subcarrier to fill in the lower half of the grid.
    unsigned N = std::min(prach_grid_size / 2 - k_start, L_ra);

    // Copy first N subcarriers of the sequence in the lower half grid.
    srsvec::copy(lower_grid.subspan(k_start, N), y_u_v.first(N));

    // Copy the remainder of the sequence in the upper half grid.
    srsvec::copy(upper_grid.first(L_ra - N), y_u_v.last(L_ra - N));
  } else {
    // Copy the sequence in the upper half grid.
    srsvec::copy(upper_grid.subspan(k_start - prach_grid_size / 2, L_ra), y_u_v);
  }
  srsvec::sc_prod(dft_input, gain, dft_input);

  // Return frequency domain signal.
  if (config.frequency_domain) {
    return dft_input;
  }

  // Calculate DFT.
  span<const cf_t> prach_symbol = dft->run();

  // Calculate total output sequence length. Rounds it to a subframe.
  unsigned output_length = info.N_u + info.N_cp_ra;
  output_length          = (15 * dft_size_15kHz) * divide_ceil(output_length, 15 * dft_size_15kHz);

  // Select view of the output sequence.
  span<cf_t> output = temp.first(output_length);

  // Copy cyclic prefix.
  srsvec::copy(output.first(info.N_cp_ra), prach_symbol.last(info.N_cp_ra));

  // Copy PRACH symbol.
  for (unsigned n = 0; n < info.N_u; n += prach_symbol.size()) {
    srsvec::copy(output.subspan(info.N_cp_ra + n, prach_symbol.size()), prach_symbol);
  }

  // Append zeros.
  srsvec::zero(output.last(output_length - info.N_u - info.N_cp_ra));

  return output;
}

span<const cf_t> prach_generator_impl::generate(const prach_generator::configuration& config)
{
  srsran_assert(config.format.is_long_preamble(), "Short preambles are not implemented.");
  prach_preamble_modulation_info info         = prach_preamble_modulation_info_get_long(config.format, dft_size_15kHz);
  unsigned                       prach_scs_Hz = info.scs.to_Hz();
  unsigned                       pusch_scs_Hz = scs_to_khz(config.pusch_scs) * 1000;
  unsigned                       L_ra         = get_sequence_length(config.format);

  unsigned N_cs = get_nof_cyclic_shifts(prach_scs_Hz, config.restricted_set, config.zero_correlation_zone);
  srsran_assert(N_cs != RESERVED, "Configuration leads to a reserved number of cyclic shifts.");

  unsigned N_rb_ra = get_N_rb_ra(prach_scs_Hz, pusch_scs_Hz);
  srsran_assert(N_rb_ra != RESERVED, "Configuration leads to a reserved number of resource blocks.");

  unsigned root_sequence_index = config.root_sequence_index + config.preamble_index;
  unsigned cyclic_shift        = 0;

  if (N_cs != 0) {
    unsigned nof_sequences_per_root = L_ra / N_cs;
    root_sequence_index             = config.root_sequence_index + config.preamble_index / nof_sequences_per_root;
    cyclic_shift                    = (config.preamble_index % nof_sequences_per_root) * N_cs;
  }

  unsigned         sequence_number = get_sequence_number_long(root_sequence_index);
  span<const cf_t> y_u_v           = generate_y_u_v_long(sequence_number, cyclic_shift);

  return modulate(y_u_v, config);
}
