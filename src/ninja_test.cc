// Copyright 2013 Google Inc. All Rights Reserved.
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

#include <gtest/gtest.h>

#include "metrics.h"

// Two metric sites must not introduce the same local variable, even when the
// invocations occupy the same source line.
TEST(MetricMacroTest, MultipleSitesInOneScope) {
  METRIC_RECORD("first"); METRIC_RECORD("second");
  METRIC_RECORD_IF("conditional", true);
}

// The macros must expand to a single declaration without swallowing 'else'.
// The condition must be evaluated exactly once.
TEST(MetricMacroTest, ConditionalControlFlow) {
  int evaluations = 0;
  METRIC_RECORD_IF("checked", ++evaluations == 1);
  EXPECT_EQ(1, evaluations);

  int alternate = 0;
  if (true)
    METRIC_RECORD("selected");
  else
    ++alternate;

  if (false)
    METRIC_RECORD_IF("unselected", ++evaluations == 2);
  else
    ++alternate;

  EXPECT_EQ(1, evaluations);
  EXPECT_EQ(1, alternate);
}

int main(int argc, char **argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
