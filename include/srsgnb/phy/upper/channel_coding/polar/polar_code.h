/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsgnb/adt/span.h"
#include <cstdint>
#include <memory>

namespace srsgnb {

enum class polar_code_ibil { not_present, present };

class polar_code
{
public:
  /*!
   * \brief Maximum rate-matched codeword length
   */
  static const uint16_t EMAX = 8192;

  /*!
   * \brief Maximum codeword length
   */
  static const uint16_t NMAX = 1024;

  /*!
   * \brief Base 2 logarithm of maximum codeword length
   */
  static const unsigned NMAX_LOG = 10U;

  /*!
   * \brief \f$log_2(EMAX)\f$
   */
  static const uint16_t eMAX = 13; // log2(EMAX);

  virtual ~polar_code()                                    = default;
  virtual uint16_t             get_n() const               = 0;
  virtual uint16_t             get_N() const               = 0;
  virtual uint16_t             get_K() const               = 0;
  virtual uint16_t             get_E() const               = 0;
  virtual uint16_t             get_nPC() const             = 0;
  virtual span<const uint16_t> get_K_set() const           = 0;
  virtual span<const uint16_t> get_PC_set() const          = 0;
  virtual span<const uint16_t> get_F_set() const           = 0;
  virtual span<const uint16_t> get_mother_code() const     = 0;
  virtual span<const uint16_t> get_blk_interleaver() const = 0;
  virtual polar_code_ibil      get_ibil() const            = 0;

  /*!
   * \brief Initializes the different index sets as needed by the subchannel allocation block and/or by the polar
   * decoder.
   *
   * \param[in] K Number of data + CRC bits.
   * \param[in] E Number of bits of the codeword after rate matching.
   * \param[in] nMax Maximum \f$log_2(N)\f$, where \f$N\f$ is the codeword size, nMax = 9 for downlink and nMax = 10,
   * for uplink. \return An integer: 0 if the function executes correctly, -1 otherwise.
   * \param[in] ibil Indicator of bit interleaver (set to not_present to disable).
   */
  virtual void set(uint16_t K, uint16_t E, uint8_t nMax, polar_code_ibil ibil) = 0;
};

std::unique_ptr<polar_code> create_polar_code();

} // namespace srsgnb
