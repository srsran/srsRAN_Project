/*
 *
 * Copyright 2021-2023 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/srslog/srslog.h"
#include "srsran/support/event_tracing.h"
#include <fstream>
#include <gtest/gtest.h>

using namespace srsran;

TEST(event_tracing_test, null_tracer)
{
  null_event_tracer tracer;

  trace_point tp = tracer.now();

  tracer << trace_event("test_event", tp);
}

TEST(event_tracing_test, event_trace_formatting)
{
  test_event_tracer tracer;

  trace_point tp = tracer.now();

  tracer << trace_event("test_event", tp);

  std::string event_out = tracer.get_last_event();

  fmt::print("event: {}\n", event_out);

  ASSERT_EQ(event_out[0], '{');
  ASSERT_EQ(event_out.back(), '}');
  ASSERT_TRUE(event_out.find("\"name\": \"test_event\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"ph\": \"X\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"cat\": \"process\"") != std::string::npos);
}

TEST(event_tracing_test, instant_event_trace_formatting)
{
  test_event_tracer tracer;

  trace_point tp = tracer.now();

  tracer << instant_trace_event("test_event", tp, instant_trace_event::cpu_scope::global);

  std::string event_out = tracer.get_last_event();

  fmt::print("event: {}\n", event_out);

  ASSERT_EQ(event_out[0], '{');
  ASSERT_EQ(event_out.back(), '}');
  ASSERT_TRUE(event_out.find("\"name\": \"test_event\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"ph\": \"i\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"cat\": \"process\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"s\": \"g\"") != std::string::npos);
}

TEST(event_tracing_test, file_event_tracer)
{
  open_trace_file("/tmp/event_tracing_test.json");

  file_event_tracer<> tracer;

  trace_point tp = tracer.now();

  tracer << trace_event("test_event1", tp);
  tracer << instant_trace_event("test_event2", tp, instant_trace_event::cpu_scope::global);
  tracer << instant_trace_event("test_event3", tp, instant_trace_event::cpu_scope::thread);
  tracer << instant_trace_event("test_event4", tp, instant_trace_event::cpu_scope::process);

  close_trace_file();

  std::ifstream fptr("/tmp/event_tracing_test.json");
  ASSERT_TRUE(fptr.is_open());
  std::string line;

  ASSERT_TRUE(std::getline(fptr, line));
  ASSERT_EQ(line, "[");
  ASSERT_TRUE(std::getline(fptr, line));
  ASSERT_EQ(line[0], '{');
  ASSERT_EQ(line.back(), ',');
  ASSERT_NE(line.find("\"name\": \"test_event1\""), std::string::npos);
  ASSERT_NE(line.find("\"ph\": \"X\""), std::string::npos);
  ASSERT_TRUE(std::getline(fptr, line));
  ASSERT_EQ(line[0], '{');
  ASSERT_EQ(line.back(), ',');
  ASSERT_NE(line.find("\"name\": \"test_event2\""), std::string::npos);
  ASSERT_NE(line.find("\"ph\": \"i\""), std::string::npos);
  ASSERT_NE(line.find("\"s\": \"g\""), std::string::npos);
  ASSERT_TRUE(std::getline(fptr, line));
  ASSERT_NE(line.find("\"name\": \"test_event3\""), std::string::npos);
  ASSERT_NE(line.find("\"s\": \"t\""), std::string::npos);
  ASSERT_TRUE(std::getline(fptr, line));
  ASSERT_NE(line.find("\"name\": \"test_event4\""), std::string::npos);
  ASSERT_NE(line.find("\"s\": \"p\""), std::string::npos);
}

TEST(event_tracing_test, deactivated_file_event_tracer)
{
  open_trace_file("/tmp/event_tracing_test2.json");

  file_event_tracer<false> tracer;

  trace_point tp = tracer.now();

  tracer << trace_event("test_event1", tp);
  tracer << instant_trace_event("test_event2", tp, instant_trace_event::cpu_scope::global);
  tracer << instant_trace_event("test_event3", tp, instant_trace_event::cpu_scope::thread);
  tracer << instant_trace_event("test_event4", tp, instant_trace_event::cpu_scope::process);

  close_trace_file();

  std::ifstream fptr("/tmp/event_tracing_test2.json");
  ASSERT_TRUE(fptr.is_open());
  std::string line;

  ASSERT_TRUE(std::getline(fptr, line));
  ASSERT_EQ(line, "[");
  ASSERT_TRUE(std::getline(fptr, line));
  ASSERT_EQ(line, "]");
}
