// Copyright 2011 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef NINJA_METRICS_H_
#define NINJA_METRICS_H_

#include <string>
#include <vector>

#include "util.h"  // For int64_t.

/// The Metrics module is used for the debug mode that dumps timing stats of
/// various actions.  To use, see METRIC_RECORD below.

/// A single metrics we're tracking, like "depfile load time".
struct Metric {
  std::string name;
  /// Number of times we've hit the code path.
  int count;
  /// Total time (in platform-dependent units) we've spent on the code path.
  int64_t sum;
};

/// A scoped object for recording a metric across the body of a function.
/// Used by the METRIC_RECORD macro.
struct ScopedMetric {
  explicit ScopedMetric(Metric* metric);
  ~ScopedMetric();

private:
  Metric* metric_;
  /// Timestamp when the measurement started.
  /// Value is platform-dependent.
  int64_t start_;
};

/// The singleton that stores metrics and prints the report.
struct Metrics {
  Metric* NewMetric(const std::string& name);

  /// Print a summary report to stdout.
  void Report();

private:
  std::vector<Metric*> metrics_;
};

/// Get the current time as relative to some epoch.
/// Epoch varies between platforms; only useful for measuring elapsed time.
int64_t GetTimeMillis();

/// A simple stopwatch which returns the time
/// in seconds since Restart() was called.
struct Stopwatch {
 public:
  Stopwatch() : started_(0) {}

  /// Seconds since Restart() call.
  double Elapsed() const;

  void Restart() { started_ = NowRaw(); }

 private:
  uint64_t started_;
  // Return the current time using the native frequency of the high resolution
  // timer.
  uint64_t NowRaw() const;
};

/// The primary interface to metrics. Each invocation expands to exactly one
/// declaration with a unique identifier, so multiple records in one scope (or
/// on one source line) do not collide. The caller supplies the semicolon.
///
/// A lambda keeps the per-call-site Metric* static and preserves its existing
/// one-time, thread-safe initialization without introducing names into the
/// calling scope. The scoped timer still runs until that scope exits.
#define NINJA_METRIC_JOIN_IMPL(a, b) a##b
#define NINJA_METRIC_JOIN(a, b) NINJA_METRIC_JOIN_IMPL(a, b)
#define NINJA_METRIC_RECORD_IMPL(metric_name, enabled, id)                   \
  ScopedMetric NINJA_METRIC_JOIN(ninja_scoped_metric_, id)([&]() -> Metric* { \
    static Metric* metric =                                                \
        g_metrics ? g_metrics->NewMetric(metric_name) : NULL;              \
    return (enabled) ? metric : NULL;                                       \
  }())

/// Record the elapsed time spent in this scope.
#define METRIC_RECORD(name) NINJA_METRIC_RECORD_IMPL(name, true, __COUNTER__)

/// Record elapsed time only when |condition| is true. The expression is
/// evaluated once, after the call site's metric has been initialized.
#define METRIC_RECORD_IF(name, condition) \
  NINJA_METRIC_RECORD_IMPL(name, condition, __COUNTER__)

extern Metrics* g_metrics;

#endif // NINJA_METRICS_H_
