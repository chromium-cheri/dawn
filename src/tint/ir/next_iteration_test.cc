// Copyright 2023 The Tint Authors.
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

#include "src/tint/ir/next_iteration.h"
#include "gtest/gtest-spi.h"
#include "src/tint/ir/ir_test_helper.h"

namespace tint::ir {
namespace {

using namespace tint::number_suffixes;  // NOLINT
using IR_NextIterationTest = IRTestHelper;

TEST_F(IR_NextIterationTest, Fail_NullLoop) {
    EXPECT_FATAL_FAILURE(
        {
            Module mod;
            Builder b{mod};
            b.NextIteration(nullptr);
        },
        "");
}

TEST_F(IR_NextIterationTest, Fail_NullValue) {
    EXPECT_FATAL_FAILURE(
        {
            Module mod;
            Builder b{mod};
            b.NextIteration(b.Loop(), utils::Vector<Value*, 1>{nullptr});
        },
        "");
}

}  // namespace
}  // namespace tint::ir
