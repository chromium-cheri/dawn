// Copyright 2021 The Tint Authors.
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

#include "src/resolver/resolver.h"

#include "gmock/gmock.h"
#include "src/resolver/resolver_test_helper.h"

namespace tint {
namespace resolver {
namespace {

using ResolverIsHostShareable = ResolverTest;

TEST_F(ResolverIsHostShareable, Void) {
  EXPECT_FALSE(r()->IsHostShareable(ty.void_()));
}

TEST_F(ResolverIsHostShareable, Bool) {
  EXPECT_FALSE(r()->IsHostShareable(ty.bool_()));
}

TEST_F(ResolverIsHostShareable, NumericScalar) {
  EXPECT_TRUE(r()->IsHostShareable(ty.i32()));
  EXPECT_TRUE(r()->IsHostShareable(create<sem::U32>()));
  EXPECT_TRUE(r()->IsHostShareable(ty.f32()));
}

TEST_F(ResolverIsHostShareable, NumericVector) {
  EXPECT_TRUE(r()->IsHostShareable(ty.vec2<i32>()));
  EXPECT_TRUE(r()->IsHostShareable(ty.vec3<i32>()));
  EXPECT_TRUE(r()->IsHostShareable(ty.vec4<i32>()));
  EXPECT_TRUE(r()->IsHostShareable(create<sem::Vector>(create<sem::U32>(), 2)));
  EXPECT_TRUE(r()->IsHostShareable(create<sem::Vector>(create<sem::U32>(), 3)));
  EXPECT_TRUE(r()->IsHostShareable(create<sem::Vector>(create<sem::U32>(), 4)));
  EXPECT_TRUE(r()->IsHostShareable(ty.vec2<f32>()));
  EXPECT_TRUE(r()->IsHostShareable(ty.vec3<f32>()));
  EXPECT_TRUE(r()->IsHostShareable(ty.vec4<f32>()));
}

TEST_F(ResolverIsHostShareable, BoolVector) {
  EXPECT_FALSE(r()->IsHostShareable(ty.vec2<bool>()));
  EXPECT_FALSE(r()->IsHostShareable(ty.vec3<bool>()));
  EXPECT_FALSE(r()->IsHostShareable(ty.vec4<bool>()));
  EXPECT_FALSE(r()->IsHostShareable(ty.vec2<bool>()));
  EXPECT_FALSE(r()->IsHostShareable(ty.vec3<bool>()));
  EXPECT_FALSE(r()->IsHostShareable(ty.vec4<bool>()));
  EXPECT_FALSE(r()->IsHostShareable(ty.vec2<bool>()));
  EXPECT_FALSE(r()->IsHostShareable(ty.vec3<bool>()));
  EXPECT_FALSE(r()->IsHostShareable(ty.vec4<bool>()));
}

TEST_F(ResolverIsHostShareable, Matrix) {
  EXPECT_TRUE(r()->IsHostShareable(ty.mat2x2<f32>()));
  EXPECT_TRUE(r()->IsHostShareable(ty.mat2x3<f32>()));
  EXPECT_TRUE(r()->IsHostShareable(ty.mat2x4<f32>()));
  EXPECT_TRUE(r()->IsHostShareable(ty.mat3x2<f32>()));
  EXPECT_TRUE(r()->IsHostShareable(ty.mat3x3<f32>()));
  EXPECT_TRUE(r()->IsHostShareable(ty.mat3x4<f32>()));
  EXPECT_TRUE(r()->IsHostShareable(ty.mat4x2<f32>()));
  EXPECT_TRUE(r()->IsHostShareable(ty.mat4x3<f32>()));
  EXPECT_TRUE(r()->IsHostShareable(ty.mat4x4<f32>()));
}

TEST_F(ResolverIsHostShareable, Pointer) {
  EXPECT_FALSE(
      r()->IsHostShareable(ty.pointer<i32>(ast::StorageClass::kPrivate)));
}

TEST_F(ResolverIsHostShareable, ArraySizedOfHostShareable) {
  auto* arr = create<sem::Array>(create<sem::I32>(), 5, 4, 20, 4, true);
  EXPECT_TRUE(r()->IsHostShareable(arr));
}

TEST_F(ResolverIsHostShareable, ArrayUnsizedOfHostShareable) {
  auto* arr = create<sem::Array>(create<sem::I32>(), 0, 4, 4, 4, true);
  EXPECT_TRUE(r()->IsHostShareable(arr));
}

// Note: Structure tests covered in host_shareable_validation_test.cc

}  // namespace
}  // namespace resolver
}  // namespace tint
