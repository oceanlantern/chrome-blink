// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// authors: tim adams oceanlantern@gmail.com

#ifndef THIRD_PARTY_BLINK_RENDERER_MODULES_SYSTEM_INFO_H_
#define THIRD_PARTY_BLINK_RENDERER_MODULES_SYSTEM_INFO_H_

#include <memory>
#include "third_party/blink/renderer/bindings/core/v8/active_script_wrappable.h"
#include "third_party/blink/renderer/core/dom/events/event_target.h"
#include "third_party/blink/renderer/core/execution_context/context_lifecycle_observer.h"
#include "third_party/blink/renderer/modules/event_target_modules.h"
#include "third_party/blink/renderer/modules/systeminfo/system_info_options.h"
#include "third_party/blink/renderer/modules/modules_export.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "third_party/blink/public/mojom/systeminfo/system_info.mojom-blink.h"


namespace blink {

class ExceptionState;
class LocalDOMWindow;
class ScriptPromise;
class ScriptState;

class MODULES_EXPORT SystemInfo
    : public EventTargetWithInlineData,
      public ActiveScriptWrappable<SystemInfo>,
      public ContextLifecycleObserver {
  USING_GARBAGE_COLLECTED_MIXIN(SystemInfo);
  DEFINE_WRAPPERTYPEINFO();

 public:
  enum class State { kInactive = 0, kActive };

  static SystemInfo* Create(ExecutionContext* context,
                               ExceptionState& exception_state);
  static SystemInfo* Create(ExecutionContext* context,
                               const SystemInfoOptions* options,
                               ExceptionState& exception_state);

  SystemInfo(ExecutionContext* context,
                const SystemInfoOptions* options,
                ExceptionState& exception_state);
  ~SystemInfo() override;

  DEFINE_ATTRIBUTE_EVENT_LISTENER(start, kStart)
  DEFINE_ATTRIBUTE_EVENT_LISTENER(stop, kStop)

  ScriptPromise start(ScriptState*, ExceptionState& exception_state);
  ScriptPromise stop(ScriptState*, ExceptionState& exception_state);
  ScriptPromise getOperatingSystemName(ScriptState* script_state, ExceptionState& exception_state);

  // EventTarget
  const AtomicString& InterfaceName() const override;
  ExecutionContext* GetExecutionContext() const override;

  // ContextLifecycleObserver
  void ContextDestroyed(ExecutionContext* context) override;

  // ScriptWrappable
  bool HasPendingActivity() const final;

  void Trace(blink::Visitor* visitor) override;

 private:

  void Shutdown();
  void ScheduleDispatchEvent(Event* event);
  void DispatchScheduledEvent();

  State state_;
  mojo::Remote<mojom::blink::SystemInfo> system_info_;

  HeapVector<Member<Event>> scheduled_events_;
};

}  // namespace blink

#endif  // THIRD_PARTY_BLINK_RENDERER_MODULES_SYSTEM_INFO_H_
