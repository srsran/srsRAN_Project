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

#include "srsgnb/phy/upper/channel_coding/crc_calculator.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_decoder.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_encoder.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_rate_dematcher.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_rate_matcher.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_segmenter_rx.h"
#include "srsgnb/phy/upper/channel_coding/ldpc/ldpc_segmenter_tx.h"
#include <memory>

namespace srsgnb {

class crc_calculator_factory
{
public:
  virtual ~crc_calculator_factory()                                       = default;
  virtual std::unique_ptr<crc_calculator> create(crc_generator_poly poly) = 0;
};

std::shared_ptr<crc_calculator_factory> create_crc_calculator_factory_sw();

class ldpc_decoder_factory
{
public:
  virtual ~ldpc_decoder_factory()                = default;
  virtual std::unique_ptr<ldpc_decoder> create() = 0;
};

std::shared_ptr<ldpc_decoder_factory> create_ldpc_decoder_factory_sw(std::string dec_type);

class ldpc_encoder_factory
{
public:
  virtual ~ldpc_encoder_factory()                = default;
  virtual std::unique_ptr<ldpc_encoder> create() = 0;
};

std::shared_ptr<ldpc_encoder_factory> create_ldpc_encoder_factory_sw(std::string enc_type);

class ldpc_rate_dematcher_factory
{
public:
  virtual ~ldpc_rate_dematcher_factory()                = default;
  virtual std::unique_ptr<ldpc_rate_dematcher> create() = 0;
};

std::shared_ptr<ldpc_rate_dematcher_factory> create_ldpc_rate_dematcher_factory_sw();

class ldpc_rate_matcher_factory
{
public:
  virtual ~ldpc_rate_matcher_factory()                = default;
  virtual std::unique_ptr<ldpc_rate_matcher> create() = 0;
};

std::shared_ptr<ldpc_rate_matcher_factory> create_ldpc_rate_matcher_factory_sw();

class ldpc_segmenter_rx_factory
{
public:
  virtual ~ldpc_segmenter_rx_factory()                = default;
  virtual std::unique_ptr<ldpc_segmenter_rx> create() = 0;
};

std::shared_ptr<ldpc_segmenter_rx_factory> create_ldpc_segmenter_rx_factory_sw();

class ldpc_segmenter_tx_factory
{
public:
  virtual ~ldpc_segmenter_tx_factory()                = default;
  virtual std::unique_ptr<ldpc_segmenter_tx> create() = 0;
};

struct ldpc_segmenter_tx_factory_sw_configuration {
  std::shared_ptr<crc_calculator_factory> crc_factory;
};

std::shared_ptr<ldpc_segmenter_tx_factory>
create_ldpc_segmenter_tx_factory_sw(ldpc_segmenter_tx_factory_sw_configuration& config);

} // namespace srsgnb
