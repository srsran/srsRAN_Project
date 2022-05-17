/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#ifndef SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_CODE_IMPL_H
#define SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_CODE_IMPL_H

#include "srsgnb/phy/upper/channel_coding/polar/polar_code.h"
#include "srsgnb/srsvec/aligned_vec.h"

namespace srsgnb {

class polar_code_impl : public polar_code
{
private:
  /*!
   * \brief Look-up table for the mother code with code_size_log = 5.
   */
  static const std::array<uint16_t, 32> mother_code_5;

  /*!
   * \brief Look-up table for the mother code with code_size_log = 6.
   */
  static const std::array<uint16_t, 64> mother_code_6;

  /*!
   * \brief Look-up table for the mother code with code_size_log = 7.
   */
  static const std::array<uint16_t, 128> mother_code_7;

  /*!
   * \brief Look-up table for the mother code with code_size_log = 8.
   */
  static const std::array<uint16_t, 256> mother_code_8;

  /*!
   * \brief Look-up table for the mother code with code_size_log = 9.
   */
  static const std::array<uint16_t, 512> mother_code_9;

  /*!
   * \brief Look-up table for the mother code with code_size_log = 10.
   */
  static const std::array<uint16_t, 1024> mother_code_10;

  /*!
   * \brief Look-up table for the block interleaver for code_size_log = 5.
   */
  static const std::array<uint16_t, 32> blk_interleaver_5;

  /*!
   * \brief Look-up table for the block interleaver for code_size_log = 6.
   */
  static const std::array<uint16_t, 64> blk_interleaver_6;

  /*!
   * \brief Look-up table for the block interleaver for code_size_log = 7.
   */
  static const std::array<uint16_t, 128> blk_interleaver_7;

  /*!
   * \brief Look-up table for the block interleaver for code_size_log = 8.
   */
  static const std::array<uint16_t, 256> blk_interleaver_8;

  /*!
   * \brief Look-up table for the block interleaver for code_size_log = 9.
   */
  static const std::array<uint16_t, 512> blk_interleaver_9;

  /*!
   * \brief Look-up table for the block interleaver for code_size_log = 10.
   */
  static const std::array<uint16_t, 1024> blk_interleaver_10;

  /*!
   * Returns a pointer to the desired mother code.
   */
  static span<const uint16_t> get_mother_code(uint8_t n);

  /*!
   * Returns a pointer to the desired blk_interleaver.
   */
  static span<const uint16_t> get_blk_interleaver(uint8_t n);

  /*!
   * Extracts the elements in x that are smaller than T or are in y.
   * Returns the length of the output vector z.
   */
  static uint16_t setdiff_stable(const uint16_t* x,
                                 const uint16_t* y,
                                 uint16_t*       z,
                                 const int       T,
                                 const uint16_t  len1,
                                 const uint16_t  len2);

  /*!
   * Gets the codeword length N, nPC and nWmPC depending on the code parameters.
   * Returns -1 if not supported configuration, otherwise returns 0.
   */
  void set_code_params(const uint16_t K, const uint16_t E, const uint8_t nMax);

  uint16_t       N;          /*!< \brief Number of coded bits (N). */
  uint8_t        n;          /*!< \brief \f$ log_2(N)\f$.*/
  uint16_t       K;          /*!< \brief Number of message bits (data and CRC). */
  uint16_t       E;          /*!< \brief Number of encoded bits at the rate matching input. */
  uint16_t       nPC;        /*!< \brief Number of parity check bits. */
  uint16_t       nWmPC;      /*!< \brief Number of parity bits of minimum bandwidth type. */
  uint16_t       F_set_size; /*!< \brief Number of frozen bits. */
  span<uint16_t> K_set; /*!< \brief Pointer to the indices of the encoder input vector containing data and CRC bits. */
  srsvec::aligned_vec<uint16_t> tmp_K_set; /*!< \brief Temporal Pointer. */
  std::array<uint16_t, 4>
      PC_set; /*!< \brief Pointer to the indices of the encoder input vector containing the parity bits.*/
  srsvec::aligned_vec<uint16_t>
                  F_set; /*!< \brief Pointer to the indices of the encoder input vector containing frozen bits.*/
  polar_code_ibil ibil;

public:
  polar_code_impl();
  ~polar_code_impl() = default;

  uint16_t             get_n() const override;
  uint16_t             get_N() const override;
  uint16_t             get_K() const override;
  uint16_t             get_E() const override;
  uint16_t             get_nPC() const override;
  span<const uint16_t> get_K_set() const override;
  span<const uint16_t> get_PC_set() const override;
  span<const uint16_t> get_F_set() const override;
  span<const uint16_t> get_mother_code() const override;
  span<const uint16_t> get_blk_interleaver() const override;
  polar_code_ibil      get_ibil() const override;

  void set(uint16_t K, uint16_t E, uint8_t nMax, polar_code_ibil ibil) override;
};

} // namespace srsgnb

#endif // SRSGNB_LIB_PHY_UPPER_CHANNEL_CODING_POLAR_POLAR_CODE_IMPL_H
