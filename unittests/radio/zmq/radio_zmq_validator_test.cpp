/*
 *
 * Copyright 2013-2022 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsgnb/radio/radio_factory.h"
#include "srsgnb/support/executors/task_worker.h"
#include "fmt/ostream.h"
#include "gtest/gtest.h"

using namespace srsgnb;

namespace fmt {

template <>
struct formatter<srsgnb::radio_configuration::clock_sources::source> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::radio_configuration::clock_sources::source& source, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    switch (source) {
      case srsgnb::radio_configuration::clock_sources::source::DEFAULT:
        return fmt::format_to(ctx.out(), "default");
      case srsgnb::radio_configuration::clock_sources::source::INTERNAL:
        return fmt::format_to(ctx.out(), "internal");
      case srsgnb::radio_configuration::clock_sources::source::EXTERNAL:
        return fmt::format_to(ctx.out(), "external");
      case srsgnb::radio_configuration::clock_sources::source::GPSDO:
      default:
        return fmt::format_to(ctx.out(), "gpsdo");
    }
  }
};

template <>
struct formatter<srsgnb::radio_configuration::radio> {
  template <typename ParseContext>
  auto parse(ParseContext& ctx) -> decltype(ctx.begin())
  {
    return ctx.begin();
  }

  template <typename FormatContext>
  auto format(const srsgnb::radio_configuration::radio& config, FormatContext& ctx)
      -> decltype(std::declval<FormatContext>().out())
  {
    return fmt::format_to(ctx.out(), "clock_src={} sync_src={}", config.clock.clock, config.clock.sync);
  }
};
} // namespace fmt

namespace {

const radio_configuration::clock_sources base_clock_sources = {radio_configuration::clock_sources::source::DEFAULT,
                                                               radio_configuration::clock_sources::source::DEFAULT};

const radio_configuration::lo_frequency base_lo_frequency = {3.5e9, 0.0};

const radio_configuration::channel base_tx_channel = {base_lo_frequency, 20.0, "tcp://*:1234"};

const radio_configuration::channel base_rx_channel = {base_lo_frequency, 10.0, "tcp://localhost:1234"};

const radio_configuration::stream base_tx_stream = {{base_tx_channel}, ""};

const radio_configuration::stream base_rx_stream = {{base_rx_channel}, ""};

const radio_configuration::radio radio_base_config = {base_clock_sources,
                                                      {base_tx_stream},
                                                      {base_rx_stream},
                                                      1.92e6,
                                                      radio_configuration::over_the_wire_format::DEFAULT,
                                                      "none"};

struct test_case_t {
  std::function<radio_configuration::radio()> get_config;
  std::string                                 message;
};

std::ostream& operator<<(std::ostream& os, const test_case_t& test_case)
{
  fmt::print(os, "{}", test_case.get_config());
  return os;
}

const std::vector<test_case_t> radio_zmq_validator_test_data = {
    {[] {
       radio_configuration::radio config = radio_base_config;
       return config;
     },
     ""},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.clock.clock                = radio_configuration::clock_sources::source::INTERNAL;
       return config;
     },
     "Only 'default' clock source is available.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.clock.sync                 = radio_configuration::clock_sources::source::INTERNAL;
       return config;
     },
     "Only 'default' sync source is available.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.tx_streams.clear();
       return config;
     },
     "At least one transmit stream must be available.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.tx_streams.front().channels.clear();
       return config;
     },
     "Streams must contain at least one channel.\n"},
    {[] {
       radio_configuration::radio config                                   = radio_base_config;
       config.tx_streams.front().channels.front().freq.center_frequency_hz = 0.0;
       return config;
     },
     "The center frequency must be non-zero, NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config                               = radio_base_config;
       config.tx_streams.front().channels.front().freq.lo_frequency_hz = 1.0;
       return config;
     },
     "The custom LO frequency is not currently supported.\n"},
    {[] {
       radio_configuration::radio config                  = radio_base_config;
       config.tx_streams.front().channels.front().gain_dB = NAN;
       return config;
     },
     "Channel gain must not be NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config                  = radio_base_config;
       config.tx_streams.front().channels.front().gain_dB = INFINITY;
       return config;
     },
     "Channel gain must not be NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config               = radio_base_config;
       config.tx_streams.front().channels.front().args = "auto";
       return config;
     },
     "Channel arguments auto does not describe a valid TCP socket.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.tx_streams.front().args    = "some args";
       return config;
     },
     "Stream arguments are not currently supported.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.rx_streams.clear();
       return config;
     },
     "At least one receive stream must be available.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.rx_streams.front().channels.clear();
       return config;
     },
     "Streams must contain at least one channel.\n"},
    {[] {
       radio_configuration::radio config                                   = radio_base_config;
       config.rx_streams.front().channels.front().freq.center_frequency_hz = 0.0;
       return config;
     },
     "The center frequency must be non-zero, NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config                               = radio_base_config;
       config.rx_streams.front().channels.front().freq.lo_frequency_hz = 1.0;
       return config;
     },
     "The custom LO frequency is not currently supported.\n"},
    {[] {
       radio_configuration::radio config                  = radio_base_config;
       config.rx_streams.front().channels.front().gain_dB = NAN;
       return config;
     },
     "Channel gain must not be NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config                  = radio_base_config;
       config.rx_streams.front().channels.front().gain_dB = INFINITY;
       return config;
     },
     "Channel gain must not be NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config               = radio_base_config;
       config.rx_streams.front().channels.front().args = "auto";
       return config;
     },
     "Channel arguments auto does not describe a valid TCP socket.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.rx_streams.front().args    = "some args";
       return config;
     },
     "Stream arguments are not currently supported.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.sampling_rate_hz           = 0.0;
       return config;
     },
     "The sampling rate must be non-zero, NAN nor infinite.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.sampling_rate_hz           = -1.0;
       return config;
     },
     "The sampling rate must be greater than zero.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.otw_format                 = radio_configuration::over_the_wire_format::SC12;
       return config;
     },
     "Only default OTW format is currently supported.\n"},
    {[] {
       radio_configuration::radio config = radio_base_config;
       config.log_level                  = "some invalid log level";
       return config;
     },
     "Log level some invalid log level does not correspond to an actual logger level.\n"},
};

class RadioZmqValidatorFixture : public ::testing::TestWithParam<test_case_t>
{
protected:
  static std::unique_ptr<radio_factory> factory;

  static void SetUpTestSuite()
  {
    if (factory) {
      return;
    }

    // Create pseudo-random sequence generator.
    factory = create_radio_factory("zmq", "");
    ASSERT_NE(factory, nullptr);
  }
};

class radio_notifier_spy : public radio_notification_handler
{
public:
  void on_radio_rt_event(const event_description& description) override {}
};

std::unique_ptr<radio_factory> RadioZmqValidatorFixture::factory = nullptr;

TEST_P(RadioZmqValidatorFixture, RadioZmqValidatorTest)
{
  ASSERT_NE(factory, nullptr);

  const test_case_t& param = GetParam();

  // Create configuration.
  radio_configuration::radio config = param.get_config();

  // Redirect stdout to buffer.
  ::testing::internal::CaptureStdout();

  // Determine whether the configuration is valid.
  bool        is_valid = factory->get_configuration_validator().is_configuration_valid(config);
  std::string output   = ::testing::internal::GetCapturedStdout();

  // Asserts the validity of the configuration.
  ASSERT_EQ(param.message.empty(), is_valid);

  // Asserts the contents of the standard output.
  ASSERT_EQ(output, param.message);

  // Make sure the process of creation is valid.
  if (param.message.empty()) {
    // Asynchronous task executor.
    task_worker                    async_task_worker("async_thread", 2 * RADIO_MAX_NOF_PORTS);
    std::unique_ptr<task_executor> async_task_executor = make_task_executor(async_task_worker);

    // Notifier.
    radio_notifier_spy notifier;

    std::unique_ptr<radio_session> radio = factory->create(config, *async_task_executor, notifier);
    ASSERT_NE(radio, nullptr);
  }
}

// Creates test suite that combines all possible parameters.
INSTANTIATE_TEST_SUITE_P(RadioZmqValidatorTest,
                         RadioZmqValidatorFixture,
                         ::testing::ValuesIn(radio_zmq_validator_test_data));

} // namespace
