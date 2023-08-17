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
#include "srsran/adt/optional.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_notifier.h"
#include "srsran/phy/upper/channel_processors/pusch/pusch_decoder_result.h"

namespace srsran {

class pusch_decoder_notifier_impl : public pusch_decoder_notifier
{
public:
  void on_sch_data(const pusch_decoder_result& result_) override { result.emplace(result_); }

  bool has_result() const { return result.has_value(); }

  const pusch_decoder_result& get_result() const
  {
    srsran_assert(has_result(), "Result is not present.");
    return result.value();
  }

private:
  optional<pusch_decoder_result> result;
};

} // namespace srsran
