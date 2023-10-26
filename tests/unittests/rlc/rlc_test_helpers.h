/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include "srsran/adt/byte_buffer.h"
#include "srsran/pcap/pcap_rlc.h"
#include <gtest/gtest.h>
#include <list>

using namespace srsran;

/// Fixture base class for all RLC {Tx,Rx} {AM,UM,TM} tests which contains commonly uses helper methods
class rlc_trx_test
{
public:
  /// \brief Creates a byte_buffer serving as SDU for RLC
  ///
  /// The produced SDU contains an incremental sequence of bytes starting with the value given by first_byte,
  /// i.e. if first_byte = 0xfc, the SDU will be 0xfc 0xfe 0xff 0x00 0x01 ...
  /// \param sdu_size Size of the SDU
  /// \param first_byte Value of the first byte
  /// \return the produced SDU as a byte_buffer
  byte_buffer create_sdu(uint32_t sdu_size, uint8_t first_byte = 0) const
  {
    byte_buffer sdu_buf;
    for (uint32_t k = 0; k < sdu_size; ++k) {
      sdu_buf.append(first_byte + k);
    }
    return sdu_buf;
  }
};

/// Dummy pcap writer.
class pcap_rlc_dummy : public pcap_rlc
{
public:
  ~pcap_rlc_dummy() = default;
  pcap_rlc_dummy()  = default;

  void open(const std::string& filename_) override {}
  void close() override {}
  bool is_write_enabled() override { return false; }
  void push_pdu(const pcap_rlc_pdu_context& context, const_span<uint8_t> pdu) override {}
  void push_pdu(const pcap_rlc_pdu_context& context, const byte_buffer& pdu) override {}
};
