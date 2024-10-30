/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#include "srsran/srslog/srslog.h"
#include "srsran/support/executors/executor_tracer.h"
#include "srsran/support/executors/manual_task_worker.h"
#include "srsran/support/tracing/event_tracing.h"
#include <fstream>
#include <gtest/gtest.h>

using namespace srsran;

TEST(event_tracing_test, null_tracer)
{
  detail::null_event_tracer tracer;

  trace_point tp = tracer.now();

  tracer << trace_event("test_event", tp);
}

TEST(event_tracing_test, event_trace_formatting)
{
  test_event_tracer tracer;

  trace_point tp = tracer.now();

  tracer << trace_event("test_event", tp);
  tracer << trace_thres_event("test_event2", tp, trace_duration{1000000000});
  tracer << trace_thres_event("test_event3", tp, trace_duration{0});

  std::vector<std::string> events = tracer.pop_last_events();
  ASSERT_EQ(events.size(), 2);

  // First event.
  std::string event_out = events[0];
  fmt::print("event: {}\n", event_out);

  ASSERT_EQ(event_out[0], '{');
  ASSERT_EQ(event_out.back(), '}');
  ASSERT_TRUE(event_out.find("\"name\": \"test_event\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"ph\": \"X\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"cat\": \"process\"") != std::string::npos);

  // Third event.
  event_out = events[1];
  fmt::print("event2: {}\n", event_out);
  ASSERT_EQ(event_out[0], '{');
  ASSERT_EQ(event_out.back(), '}');
  ASSERT_TRUE(event_out.find("\"name\": \"test_event3\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"ph\": \"X\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"cat\": \"process\"") != std::string::npos);
}

TEST(event_tracing_test, instant_event_trace_formatting)
{
  test_event_tracer tracer;

  tracer << instant_trace_event("test_event", instant_trace_event::cpu_scope::global);

  std::string event_out = tracer.pop_last_events()[0];

  fmt::print("event: {}\n", event_out);

  ASSERT_EQ(event_out[0], '{');
  ASSERT_EQ(event_out.back(), '}');
  ASSERT_TRUE(event_out.find("\"name\": \"test_event\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"ph\": \"i\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"cat\": \"process\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"s\": \"g\"") != std::string::npos);
}

TEST(event_tracing_test, rusage_event_trace_formatting)
{
  test_event_tracer tracer;

  trace_point tp           = tracer.now();
  auto        rus_snapshot = tracer.rusage_now();
  tracer << rusage_thres_trace_event("test_event", std::chrono::microseconds{0}, tp, rus_snapshot);

  std::string event_out = tracer.pop_last_events()[0];
  fmt::print("event: {}\n", event_out);

  ASSERT_EQ(event_out[0], '{');
  ASSERT_EQ(event_out.back(), '}');
  ASSERT_TRUE(event_out.find("\"name\": \"test_event\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"cat\": \"process\"") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"start_tstamp\": ") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"vol_ctxt_switch\": ") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"invol_ctxt_switch\": ") != std::string::npos);
  ASSERT_TRUE(event_out.find("\"dur\": ") != std::string::npos);
}

TEST(event_tracing_test, file_event_tracer)
{
  open_trace_file("/tmp/event_tracing_test.json");

  file_event_tracer<> tracer;

  trace_point tp = tracer.now();

  tracer << trace_event("test_event1", tp);
  tracer << instant_trace_event("test_event2", instant_trace_event::cpu_scope::global);
  tracer << instant_trace_event("test_event3", instant_trace_event::cpu_scope::thread);
  tracer << instant_trace_event("test_event4", instant_trace_event::cpu_scope::process);
  tracer << trace_thres_event("test_event5", tp, trace_duration{1000000000});
  tracer << trace_thres_event("test_event6", tp, trace_duration{0});

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
  ASSERT_TRUE(std::getline(fptr, line));
  ASSERT_NE(line.find("\"name\": \"test_event6\""), std::string::npos);
  ASSERT_NE(line.find("\"ph\": \"X\""), std::string::npos);
  ASSERT_TRUE(std::getline(fptr, line));
  ASSERT_EQ(line, "]");
}

TEST(event_tracing_test, deactivated_file_event_tracer)
{
  open_trace_file("/tmp/event_tracing_test2.json");

  file_event_tracer<false> tracer;

  trace_point tp = tracer.now();

  tracer << trace_event("test_event1", tp);
  tracer << instant_trace_event("test_event2", instant_trace_event::cpu_scope::global);
  tracer << instant_trace_event("test_event3", instant_trace_event::cpu_scope::thread);
  tracer << instant_trace_event("test_event4", instant_trace_event::cpu_scope::process);

  close_trace_file();

  std::ifstream fptr("/tmp/event_tracing_test2.json");
  ASSERT_TRUE(fptr.is_open());
  std::string line;

  ASSERT_TRUE(std::getline(fptr, line));
  ASSERT_EQ(line, "[");
  ASSERT_TRUE(std::getline(fptr, line));
  ASSERT_EQ(line, "]");
}

TEST(trace_executor_test, enqueue_and_run_traces)
{
  manual_task_worker worker{128};
  task_executor&     exec = worker;
  test_event_tracer  test_tracer;

  auto trace_exec1 = make_trace_executor("testexec1", exec, test_tracer);
  auto trace_exec2 = make_trace_executor("testexec2", exec, test_tracer);

  int counter = 0;
  (void)trace_exec1.defer([&counter]() mutable { counter++; });
  (void)trace_exec2.execute([&counter]() mutable { counter++; });

  worker.run_pending_tasks();
  ASSERT_EQ(counter, 2);

  std::vector<std::string> events = test_tracer.pop_last_events();

  fmt::print("events: {}\n", fmt::join(events, ",\n"));
  ASSERT_EQ(events.size(), 4);
  ASSERT_NE(events[0].find("\"name\": \"testexec2_enqueue\""), std::string::npos);
  ASSERT_NE(events[1].find("\"name\": \"testexec2_run\""), std::string::npos); // called with execute(...)
  ASSERT_NE(events[2].find("\"name\": \"testexec1_enqueue\""), std::string::npos);
  ASSERT_NE(events[3].find("\"name\": \"testexec1_run\""), std::string::npos); // called with defer(...)
}
