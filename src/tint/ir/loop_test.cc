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

#include "src/tint/ir/loop.h"
#include "gtest/gtest-spi.h"
#include "src/tint/ir/ir_test_helper.h"

namespace tint::ir {
namespace {

using namespace tint::number_suffixes;  // NOLINT
using IR_LoopTest = IRTestHelper;

TEST_F(IR_LoopTest, Parent) {
    auto* loop = b.CreateLoop();
    EXPECT_EQ(loop->Initializer()->Parent(), loop);
    EXPECT_EQ(loop->Body()->Parent(), loop);
    EXPECT_EQ(loop->Continuing()->Parent(), loop);
    EXPECT_EQ(loop->Merge()->Parent(), loop);
}

TEST_F(IR_LoopTest, Fail_NullInitializerBlock) {
    EXPECT_FATAL_FAILURE(
        {
            Module mod;
            Builder b{mod};
            Loop loop(nullptr, b.CreateBlock(), b.CreateBlock(), b.CreateBlock());
        },
        "");
}

TEST_F(IR_LoopTest, Fail_NullBodyBlock) {
    EXPECT_FATAL_FAILURE(
        {
            Module mod;
            Builder b{mod};
            Loop loop(b.CreateBlock(), nullptr, b.CreateBlock(), b.CreateBlock());
        },
        "");
}

TEST_F(IR_LoopTest, Fail_NullContinuingBlock) {
    EXPECT_FATAL_FAILURE(
        {
            Module mod;
            Builder b{mod};
            Loop loop(b.CreateBlock(), b.CreateBlock(), nullptr, b.CreateBlock());
        },
        "");
}

TEST_F(IR_LoopTest, Fail_NullMergeBlock) {
    EXPECT_FATAL_FAILURE(
        {
            Module mod;
            Builder b{mod};
            Loop loop(b.CreateBlock(), b.CreateBlock(), b.CreateBlock(), nullptr);
        },
        "");
}

}  // namespace
}  // namespace tint::ir