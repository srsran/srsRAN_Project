
#include "pdcch_encoder_impl.h"
#include "srsgnb/adt/static_vector.h"
#include "srsgnb/srsvec/binary.h"
#include "srsgnb/srsvec/bit.h"
#include "srsgnb/srsvec/copy.h"
#include "srsgnb/support/srsran_assert.h"

using namespace srsgnb;
using namespace pdcch_constants;

pdcch_encoder_impl::pdcch_encoder_impl() :
  crc24c(create_crc_calculator(crc_generator_poly::CRC24C)),
  interleaver(create_polar_interleaver()),
  alloc(create_polar_allocator()),
  code(create_polar_code()),
  encoder(create_polar_encoder_pipelined(polar_code::NMAX_LOG)),
  rm(create_polar_rate_matcher())
{}

void pdcch_encoder_impl::crc_attach(span<uint8_t>& c, span<const uint8_t> a, unsigned rnti)
{
  std::array<uint8_t, RNTI_LEN> unpacked_rnti = {};

  // Unpack RNTI bits
  span<uint8_t> rnti_bits{unpacked_rnti};
  srsvec::bit_unpack(rnti_bits, rnti, RNTI_LEN);

  // Set first L bits to 1s
  std::fill(c.begin(), c.begin() + CRC_LEN, 1U);

  // Copy original payload to an offset of 24 bits
  srsvec::copy(c.subspan(CRC_LEN, a.size()), a);

  // Calculate checksum
  span<uint8_t> a_prime  = c.first(CRC_LEN + a.size());
  unsigned      checksum = crc24c->calculate_bit(a_prime);

  // Unpack and attach checksum
  span<uint8_t> p = srsvec::bit_unpack(c.last(CRC_LEN), checksum, CRC_LEN);

  // Scramble CRC parity bits with RNTI
  p = c.last(RNTI_LEN);
  srsvec::binary_xor(span<uint8_t>{unpacked_rnti}, p, p);

  // Skip first L 1s added for CRC calculation
  c = c.subspan(CRC_LEN, c.size() - CRC_LEN);
}

void pdcch_encoder_impl::channel_coding(span<uint8_t> d, span<const uint8_t> c)
{
  // 5.3.1.1 Interleaving
  static_vector<uint8_t, MAX_K> c_prime(c.size());
  interleaver->interleave(c, c_prime, polar_interleaver_direction::tx);

  // Channel allocation
  static_vector<uint8_t, polar_code::NMAX> allocated(code->get_N());
  alloc->allocate(c_prime, allocated, *code);

  // Polar encoding
  encoder->encode(allocated, code->get_n(), d);
}

void pdcch_encoder_impl::rate_matching(span<uint8_t> f, span<const uint8_t> d)
{
  rm->rate_match(d, f, *code);
}

void pdcch_encoder_impl::encode(span<uint8_t> encoded, span<const uint8_t> data, const config_t& config)
{
  srsran_assert(encoded.size() >= config.E, "Output data vector is too small to store encoded bits");

  // Configure Polar encoder
  uint16_t K = data.size() + CRC_LEN;
  code->set(K, config.E, polar_code::NMAX_LOG - 1, polar_code_ibil::not_present);

  // Attach CRC
  // Allocate extra L bits for leading 1s used in CRC calculation according to TS 38.312 section 7.3.2
  static_vector<uint8_t, MAX_K + CRC_LEN> c(K + CRC_LEN);
  span<uint8_t>                           c_span{c};
  crc_attach(c_span, data, config.rnti);

  // Encode
  static_vector<uint8_t, polar_code::NMAX> d(code->get_N());
  channel_coding(d, c_span);

  // Rate match
  rate_matching(encoded, d);
}

std::unique_ptr<pdcch_encoder> srsgnb::create_pdcch_encoder()
{
  return std::make_unique<pdcch_encoder_impl>();
}