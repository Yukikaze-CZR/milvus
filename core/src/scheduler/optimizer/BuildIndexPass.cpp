// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#include "scheduler/optimizer/BuildIndexPass.h"
#include "scheduler/SchedInst.h"
#include "scheduler/Utils.h"
#include "scheduler/tasklabel/SpecResLabel.h"
#ifdef MILVUS_GPU_VERSION
namespace milvus {
namespace scheduler {

void
BuildIndexPass::Init() {
    server::Config& config = server::Config::GetInstance();
    Status s = config.GetGpuResourceConfigBuildIndexResources(build_gpu_ids_);
    if (!s.ok()) {
        throw;
    }
}

bool
BuildIndexPass::Run(const TaskPtr& task) {
    if (task->Type() != TaskType::BuildIndexTask)
        return false;

    if (build_gpu_ids_.empty()) {
        SERVER_LOG_WARNING << "BuildIndexPass cannot get build index gpu!";
        return false;
    }

    ResourcePtr res_ptr;
    res_ptr = ResMgrInst::GetInstance()->GetResource(ResourceType::GPU, build_gpu_ids_[specified_gpu_id_]);
    auto label = std::make_shared<SpecResLabel>(std::weak_ptr<Resource>(res_ptr));
    task->label() = label;
    SERVER_LOG_DEBUG << "Specify gpu" << specified_gpu_id_ << " to build index!";

    specified_gpu_id_ = (specified_gpu_id_ + 1) % build_gpu_ids_.size();
    return true;
}

}  // namespace scheduler
}  // namespace milvus
#endif
