// Copyright 2022 The Tint Authors.
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

#include "src/tint/ir/block.h"

TINT_INSTANTIATE_TYPEINFO(tint::ir::Block);

namespace tint::ir {

Block::Block() : Base() {}

Block::~Block() = default;

void Block::BranchTo(FlowNode* to, utils::VectorRef<Value*> args) {
    TINT_ASSERT(IR, to);
    branch_.target = to;
    branch_.args = args;
    if (to) {
        to->AddInboundBranch(this);
    }
}

}  // namespace tint::ir
