
#ifndef SRSGNB_UL_BSR_H
#define SRSGNB_UL_BSR_H

#include "srsgnb/adt/to_array.h"
#include "srsgnb/mac/sched_ue_feedback.h"
#include <stdint.h>

///
/// @brief  Buffer size format definition and levels according to 3GPP TS 38.321 version 15.3.0
///

namespace srsgnb {

/// TS 38.321, Table 6.1.3.1-1 Buffer size levels (in bytes) for 5-bit Buffer Size field, all values <= except marked
constexpr auto buffer_size_levels_5bit = to_array<uint32_t>(
    {/* == */ 0, 10,    14,    20,    28,    38,    53,    74,     102,    142,           198,
     276,        384,   535,   745,   1038,  1446,  2014,  2806,   3909,   5446,          7587,
     10570,      14726, 20516, 28581, 39818, 55474, 77284, 107669, 150000, /* > */ 150000});

/// TS 38.321, Table 6.1.3.1-2: Buffer size levels (in bytes) for 8-bit Buffer Size field, all values <= except marked
constexpr auto buffer_size_levels_8bit =
    to_array<uint32_t>({/* == */ 0, 10,       11,       12,       13,
                        14,         15,       16,       17,       18,
                        19,         20,       22,       23,       25,
                        26,         28,       30,       32,       34,
                        36,         38,       40,       43,       46,
                        49,         52,       55,       59,       62,
                        66,         71,       75,       80,       85,
                        91,         97,       103,      110,      117,
                        124,        132,      141,      150,      160,
                        170,        181,      193,      205,      218,
                        233,        248,      264,      281,      299,
                        318,        339,      361,      384,      409,
                        436,        464,      494,      526,      560,
                        597,        635,      677,      720,      767,
                        817,        870,      926,      987,      1051,
                        1119,       1191,     1269,     1351,     1439,
                        1532,       1631,     1737,     1850,     1970,
                        2098,       2234,     2379,     2533,     2698,
                        2873,       3059,     3258,     3469,     3694,
                        3934,       4189,     4461,     4751,     5059,
                        5387,       5737,     6109,     6506,     6928,
                        7378,       7857,     8367,     8910,     9488,
                        10104,      10760,    11458,    12202,    12994,
                        13838,      14736,    15692,    16711,    17795,
                        18951,      20181,    21491,    22885,    24371,
                        25953,      27638,    29431,    31342,    33376,
                        35543,      37850,    40307,    42923,    45709,
                        48676,      51836,    55200,    58784,    62599,
                        66663,      70990,    75598,    80505,    85730,
                        91295,      97221,    103532,   110252,   117409,
                        125030,     133146,   141789,   150992,   160793,
                        171231,     182345,   194182,   206786,   220209,
                        234503,     249725,   265935,   283197,   301579,
                        321155,     342002,   364202,   387842,   413018,
                        439827,     468377,   498780,   531156,   565634,
                        602350,     641449,   683087,   727427,   774645,
                        824928,     878475,   935498,   996222,   1060888,
                        1129752,    1203085,  1281179,  1364342,  1452903,
                        1547213,    1647644,  1754595,  1868488,  1989774,
                        2118933,    2256475,  2402946,  2558924,  2725027,
                        2901912,    3090279,  3290873,  3504487,  3731968,
                        3974215,    4232186,  4506902,  4799451,  5110989,
                        5442750,    5796046,  6172275,  6572925,  6999582,
                        7453933,    7937777,  8453028,  9001725,  9586039,
                        10208280,   10870913, 11576557, 12328006, 13128233,
                        13980403,   14887889, 15854280, 16883401, 17979324,
                        19146385,   20389201, 21712690, 23122088, 24622972,
                        26221280,   27923336, 29735875, 31666069, 33721553,
                        35910462,   38241455, 40723756, 43367187, 46182206,
                        49179951,   52372284, 55771835, 59392055, 63247269,
                        67352729,   71724679, 76380419, 81338368, /* > */ 81338368});

/// Report of Logical Channel Group UL BSR
struct lcg_bsr_report {
  lcg_id_t lcg_id;
  uint8_t  nof_bytes;
};

/// Decode Short BSR
inline lcg_bsr_report decode_sbsr(span<const uint8_t> payload)
{
  srsran_sanity_check(not payload.empty(), "Trying to decode SBSR but payload is empty.");

  lcg_bsr_report sbsr = {};
  sbsr.lcg_id         = (payload[0] & 0xe0U) >> 5;
  sbsr.nof_bytes      = payload[0] & 0x1f;
  return sbsr;
}

struct long_bsr_report {
  static const uint8_t max_num_lcg_lbsr = 8;

  uint8_t                                         bitmap; /// the first octet of LBSR and Long Trunc BSR
  static_vector<lcg_bsr_report, max_num_lcg_lbsr> list;   /// one entry for each reported LCG
};

/// Decode Long BSR
inline long_bsr_report decode_lbsr(bsr_format format, span<const uint8_t> payload)
{
  long_bsr_report lbsr = {};

  const uint8_t* ptr = payload.data();
  lbsr.bitmap        = *ptr; // read LCG bitmap
  ptr++;                     // skip LCG bitmap

  // early stop if LBSR is empty
  if (lbsr.bitmap == 0) {
    return lbsr;
  }

  unsigned bsr_cnt = 0;
  for (uint8_t i = 0; i < long_bsr_report::max_num_lcg_lbsr; i++) {
    // If LCGi bit is enabled, it means the next 8-bit BSR value corresponds to it
    if (lbsr.bitmap & (0x1U << i)) {
      lcg_bsr_report bsr = {};
      bsr.lcg_id         = i;
      // For the Long truncated, some BSR words can be not present, assume BSR > 0 in that case
      if (1 + bsr_cnt < payload.size()) {
        bsr.nof_bytes = ptr[bsr_cnt];
        bsr_cnt++;
      } else if (format == bsr_format::LONG_TRUNC_BSR) {
        bsr.nof_bytes = 63; // just assume it has 526 bytes to transmit
      } else {
        srslog::fetch_basic_logger("MAC-NR").error(
            "Error parsing LongBSR CE: sdu_length={} but there are {} active bsr\n", payload.size(), bsr_cnt);
      }
      lbsr.list.push_back(bsr);
    }
  }

  return lbsr;
}

} // namespace srsgnb

#endif // SRSGNB_UL_BSR_H
