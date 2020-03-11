// Copyright 2017 The Dawn Authors
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

#include "dawn_native/metal/PipelineLayoutMTL.h"

#include "common/BitSetIterator.h"
#include "dawn_native/BindGroupLayout.h"
#include "dawn_native/metal/DeviceMTL.h"

namespace dawn_native { namespace metal {

    PipelineLayout::PipelineLayout(Device* device, const PipelineLayoutDescriptor* descriptor)
        : PipelineLayoutBase(device, descriptor) {
        // Each stage has its own numbering namespace in CompilerMSL.
        for (auto stage : IterateStages(kAllStages)) {
            uint32_t bufferIndex = 0;
            uint32_t samplerIndex = 0;
            uint32_t textureIndex = 0;

            for (uint32_t group : IterateBitSet(GetBindGroupLayoutsMask())) {
                const auto& groupInfo = GetBindGroupLayout(group)->GetBindingInfo();
                for (size_t binding = 0; binding < kMaxBindingsPerGroup; ++binding) {
                    if (!(groupInfo.visibilities[binding] & StageBit(stage))) {
                        continue;
                    }
                    if (!groupInfo.mask[binding]) {
                        continue;
                    }

                    switch (groupInfo.types[binding]) {
                        case wgpu::BindingType::UniformBuffer:
                        case wgpu::BindingType::StorageBuffer:
                        case wgpu::BindingType::ReadonlyStorageBuffer:
                            mIndexInfo[stage][group][binding] = bufferIndex;
                            bufferIndex++;
                            break;
                        case wgpu::BindingType::Sampler:
                            mIndexInfo[stage][group][binding] = samplerIndex;
                            samplerIndex++;
                            break;
                        case wgpu::BindingType::SampledTexture:
                            mIndexInfo[stage][group][binding] = textureIndex;
                            textureIndex++;
                            break;
                        case wgpu::BindingType::StorageTexture:
                        case wgpu::BindingType::ReadonlyStorageTexture:
                        case wgpu::BindingType::WriteonlyStorageTexture:
                            UNREACHABLE();
                            break;
                    }
                }
            }

            mBufferBindingCount[stage] = bufferIndex;
        }
    }

    const PipelineLayout::BindingIndexInfo& PipelineLayout::GetBindingIndexInfo(
        SingleShaderStage stage) const {
        return mIndexInfo[stage];
    }

    uint32_t PipelineLayout::GetBufferBindingCount(SingleShaderStage stage) {
        return mBufferBindingCount[stage];
    }

}}  // namespace dawn_native::metal
