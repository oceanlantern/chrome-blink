// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stddef.h>
#include <string>
#include "base/macros.h"
#include "base/run_loop.h"
#include "base/time/time.h"
#include "third_party/blink/renderer/modules/systeminfo/system_info.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/blink/public/platform/scheduler/test/renderer_scheduler_test_support.h"
#include "third_party/blink/public/web/web_heap.h"
#include "third_party/blink/renderer/bindings/core/v8/v8_binding_for_testing.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"
#include "third_party/blink/renderer/platform/heap/heap.h"
#include "third_party/blink/renderer/platform/heap/thread_state.h"
#include "third_party/blink/renderer/platform/testing/io_task_runner_testing_platform_support.h"
#include "third_party/blink/renderer/platform/wtf/text/string_builder.h"


namespace blink {

  class MockSystemInfo : public SystemInfo {
  public:
    MockSystemInfo(V8TestingScope& scope)
      : SystemInfo(scope.GetExecutionContext(),
        SystemInfoOptions::Create(),
        scope.GetExceptionState()) {}
    ~MockSystemInfo() final = default;
  };

  TEST(SystemInfoTest, Create) {
    V8TestingScope scope;
    auto* systeminfo = MakeGarbageCollected<MockSystemInfo>(scope);

    ASSERT_TRUE(systeminfo);  

    
}


}  // namespace blink
