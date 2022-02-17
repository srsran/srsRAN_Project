/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2021 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */
#ifndef SRSGNB_CHANNEL_MODULATION_MODULATION_MAPPER_IMPL_H_
#define SRSGNB_CHANNEL_MODULATION_MODULATION_MAPPER_IMPL_H_

#include "srsgnb/phy/upper/channel_modulation/modulation_mapper.h"
#include <map>

namespace srsgnb {
class modulation_mapper_impl : public modulation_mapper
{
private:
  /**
   * @brief Defines a base structure to store the modulation tables
   */
  struct modulator_table_s {
    unsigned          Qm;
    std::vector<cf_t> table;
  };

  /**
   * @brief Modulation table for BPSK
   */
  struct bpsk_table_s : public modulator_table_s {
    bpsk_table_s();
  };

  /**
   * @brief Modulation table for QPSK
   */
  struct qpsk_table_s : public modulator_table_s {
    qpsk_table_s();
  };

  /**
   * @brief Modulation table for QAM16
   */
  struct qam16_table_s : public modulator_table_s {
    qam16_table_s();
  };

  /**
   * @brief Modulation table for QAM64
   */
  struct qam64_table_s : public modulator_table_s {
    qam64_table_s();
  };

  /**
   * @brief Modulation table for QAM256
   */
  struct qam256_table_s : public modulator_table_s {
    qam256_table_s();
  };

  /**
   * @brief Maps the modulation schemes to modulation tables
   */
  static const std::map<modulation_scheme, modulator_table_s> modulation_tables;

public:
  void modulate(span<const uint8_t> input, span<cf_t> symbols, modulation_scheme scheme) override;
};

} // namespace srsgnb
#endif // SRSGNB_CHANNEL_MODULATION_MODULATION_MAPPER_IMPL_H_
