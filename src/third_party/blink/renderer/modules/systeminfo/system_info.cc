// Copyright 2020 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// authors: tim adams oceanlantern@gmail.com

#include "third_party/blink/renderer/modules/systeminfo/system_info.h"

#include <algorithm>
#include <limits>
#include "third_party/blink/public/platform/platform.h"
#include "third_party/blink/public/platform/task_type.h"
#include "third_party/blink/renderer/bindings/core/v8/dictionary.h"
#include "third_party/blink/renderer/core/dom/events/event.h"
#include "third_party/blink/renderer/core/inspector/console_message.h"
#include "third_party/blink/renderer/modules/event_target_modules.h"
#include "third_party/blink/renderer/platform/heap/heap.h"
#include "third_party/blink/renderer/platform/network/mime/content_type.h"
#include "third_party/blink/renderer/platform/wtf/functional.h"
#include "third_party/blink/renderer/bindings/core/v8/script_promise_resolver.h"
#include "third_party/blink/renderer/core/dom/document.h"
#include "third_party/blink/renderer/core/dom/dom_exception.h"
#include "third_party/blink/renderer/core/frame/local_dom_window.h"
#include "third_party/blink/renderer/core/frame/local_frame.h"
#include "third_party/blink/public/common/browser_interface_broker_proxy.h"
#include "mojo/public/cpp/system/platform_handle.h"
#include "third_party/blink/public/common/thread_safe_browser_interface_broker_proxy.h"

namespace blink
{

// ============================================================================
//
// Create
//
//=============================================================================

SystemInfo *SystemInfo::Create(ExecutionContext *context,
                               ExceptionState &exception_state)
{
  return MakeGarbageCollected<SystemInfo>(
      context, SystemInfoOptions::Create(), exception_state);
}

// ============================================================================
//
// Create
//
//=============================================================================

SystemInfo *SystemInfo::Create(ExecutionContext *context,
                               const SystemInfoOptions *options,
                               ExceptionState &exception_state)
{
  return MakeGarbageCollected<SystemInfo>(context, options,
                                          exception_state);
}

// ============================================================================
//
// SystemInfo::SystemInfo
//
//=============================================================================

SystemInfo::SystemInfo(ExecutionContext *context,
                       const SystemInfoOptions *options,
                       ExceptionState &exception_state)
    : ContextLifecycleObserver(context),
      state_(State::kInactive)
{

  Platform::Current()->GetBrowserInterfaceBroker()->GetInterface(
      system_info_.BindNewPipeAndPassReceiver());

  if (context->IsContextDestroyed())
  {
    exception_state.ThrowDOMException(DOMExceptionCode::kNotAllowedError,
                                      "Execution context is detached.");
    return;
  }
}

// ============================================================================
//
// SystemInfo::~SystemInfo
//
//=============================================================================

SystemInfo::~SystemInfo() = default;

// ============================================================================
//
// start:
//
// C++ implementation of start function callable from JavaScript
// and declered in system_info.idl.
//
// Function makes asynchronous call to Browser process using Mojo IPC where
// processing function is implemented.
//
// When asynchronous function completes the result is returned to JavaScript by
// completing Promise or throwing an exception.
//
// See content\browser\renderer_host\system_info_impl.cc for browser side
// implementation.
//
//=============================================================================

ScriptPromise SystemInfo::start(ScriptState *script_state, ExceptionState &exception_state)
{
  if (!GetExecutionContext() || GetExecutionContext()->IsContextDestroyed())
  {
    return ScriptPromise::RejectWithDOMException(
        script_state,
        MakeGarbageCollected<DOMException>(DOMExceptionCode::kNotAllowedError));
  }

  Document *document = To<Document>(GetExecutionContext());
  if (!document)
  {
    return ScriptPromise::RejectWithDOMException(
        script_state,
        MakeGarbageCollected<DOMException>(DOMExceptionCode::kAbortError));
  }

  if (state_ != State::kInactive)
  {
    return ScriptPromise::RejectWithDOMException(
        script_state,
        MakeGarbageCollected<DOMException>(DOMExceptionCode::kInvalidStateError));
  }

  auto *resolver = MakeGarbageCollected<ScriptPromiseResolver>(script_state);
  ScriptPromise resolver_result = resolver->Promise();

  //
  // Make call to implementation function in Browser process using Mojo IPC with lamda callback
  //
  system_info_->Start(WTF::Bind([](
                                    SystemInfo *systemInfo,
                                    ScriptPromiseResolver *resolver,
                                    bool result) {
    if (result)
    {
      VLOG(1) << "SystemInfo::start succeeded";

      systemInfo->state_ = State::kActive;

      systemInfo->ScheduleDispatchEvent(Event::Create(event_type_names::kStart));

      resolver->Resolve(true);
    }
    else
    {
      VLOG(0) << "SystemInfo::start failed " << result;

      resolver->Reject(
          MakeGarbageCollected<DOMException>(DOMExceptionCode::kNotSupportedError, (char *)"SystemInfo::start failed"));
    }
  }, WrapPersistent(this), WrapPersistent(resolver)));

  return resolver_result;
}

// ============================================================================
//
// stop:
//
// C++ implementation of stop function callable from JavaScript
// and declared in system_info.idl.
//
// Function makes asynchronous call to Browser process using Mojo IPC where
// processing function is implemented.
//
// When asynchronous function completes the resultis returned to JavaScript by
// completing Promise or throwing an exception.
//
// See content\browser\renderer_host\system_info_impl.cc for browser side
// implementation.
//
//=============================================================================

ScriptPromise SystemInfo::stop(ScriptState *script_state, ExceptionState &exception_state)
{

  if (!GetExecutionContext() || GetExecutionContext()->IsContextDestroyed())
  {
    return ScriptPromise::RejectWithDOMException(
        script_state,
        MakeGarbageCollected<DOMException>(DOMExceptionCode::kNotAllowedError));
  }

  Document *document = To<Document>(GetExecutionContext());
  if (!document)
  {
    return ScriptPromise::RejectWithDOMException(
        script_state,
        MakeGarbageCollected<DOMException>(DOMExceptionCode::kAbortError));
  }

  if (state_ != State::kActive)
  {
    return ScriptPromise::RejectWithDOMException(
        script_state,
        MakeGarbageCollected<DOMException>(DOMExceptionCode::kInvalidStateError));
  }

  auto *resolver = MakeGarbageCollected<ScriptPromiseResolver>(script_state);
  ScriptPromise resolver_result = resolver->Promise();

  //
  // Make call to implementation function in Browser process using Mojo IPC with lamda callback
  //
  system_info_->Stop(WTF::Bind([](
                                   SystemInfo *SystemInfo,
                                   ScriptPromiseResolver *resolver) {
    VLOG(1) << "SystemInfo::stop succeeded";

    SystemInfo->state_ = State::kInactive;

    SystemInfo->ScheduleDispatchEvent(Event::Create(event_type_names::kStop));

    resolver->Resolve(true);
  }, WrapPersistent(this), WrapPersistent(resolver)));

  return resolver_result;
}

// ============================================================================
//
// getOperatingSystemName:
//
// C++ implementation of getOperatingSystemName function callable from JavaScript
// and declared in system_info.idl.
//
// Function makes asynchronous call to Browser process using Mojo IPC where
// processing function is implemented.
//
// When asynchronous function completes the resultis returned to JavaScript by
// completing Promise or throwing an exception.
//
// See content\browser\renderer_host\system_info_impl.cc for browser side
// implementation.
//
//=============================================================================

ScriptPromise SystemInfo::getOperatingSystemName(ScriptState *script_state, ExceptionState &exception_state)
{
  if (!GetExecutionContext() || GetExecutionContext()->IsContextDestroyed())
  {
    return ScriptPromise::RejectWithDOMException(
        script_state,
        MakeGarbageCollected<DOMException>(DOMExceptionCode::kNotAllowedError));
  }

  Document *document = To<Document>(GetExecutionContext());
  if (!document)
  {
    return ScriptPromise::RejectWithDOMException(
        script_state,
        MakeGarbageCollected<DOMException>(DOMExceptionCode::kAbortError));
  }

  if (state_ != State::kActive)
  {
    return ScriptPromise::RejectWithDOMException(
        script_state,
        MakeGarbageCollected<DOMException>(DOMExceptionCode::kInvalidStateError));
  }

  auto *resolver = MakeGarbageCollected<ScriptPromiseResolver>(script_state);
  ScriptPromise resolver_result = resolver->Promise();

  //
  // Make call to implementation function in Browser process using Mojo IPC with lamda callback
  //
  system_info_->GetOperatingSystemName(WTF::Bind([](
                                                     SystemInfo *systemInfo,
                                                     ScriptPromiseResolver *resolver,
                                                     const WTF::String &result) {
    if (result.length())
    {
      VLOG(1) << "SystemInfo::GetOperatingSystemName succeeded";

      resolver->Resolve(result);
    }
    else
    {
      VLOG(0) << "SystemInfo::GetOperatingSystemName failed " << result;

      resolver->Reject(
          MakeGarbageCollected<DOMException>(DOMExceptionCode::kNotSupportedError, (char *)"SystemInfo::start failed"));
    }
  }, WrapPersistent(this), WrapPersistent(resolver)));

  return resolver_result;
}

const AtomicString &SystemInfo::InterfaceName() const
{
  return event_target_names::kSystemInfo;
}

ExecutionContext *SystemInfo::GetExecutionContext() const
{
  return ContextLifecycleObserver::GetExecutionContext();
}

void SystemInfo::Shutdown()
{
  if (state_ != State::kInactive)
  {
    ScheduleDispatchEvent(Event::Create(event_type_names::kStop));
  }

  state_ = State::kInactive;
}

bool SystemInfo::HasPendingActivity() const
{
  return (state_ != State::kInactive);
}

void SystemInfo::ContextDestroyed(ExecutionContext *)
{
  Shutdown();
}

void SystemInfo::ScheduleDispatchEvent(Event *event)
{
  scheduled_events_.push_back(event);
  // Only schedule a post if we are placing the first item in the queue.
  if (scheduled_events_.size() == 1)
  {
    if (auto *context = GetExecutionContext())
    {
      context->GetTaskRunner(TaskType::kDOMManipulation)
          ->PostTask(FROM_HERE,
                     WTF::Bind(&SystemInfo::DispatchScheduledEvent,
                               WrapPersistent(this)));
    }
  }
}

void SystemInfo::DispatchScheduledEvent()
{
  HeapVector<Member<Event>> events;
  events.swap(scheduled_events_);

  for (const auto &event : events)
    DispatchEvent(*event);
}

void SystemInfo::Trace(blink::Visitor *visitor)
{
  visitor->Trace(scheduled_events_);
  EventTargetWithInlineData::Trace(visitor);
  ContextLifecycleObserver::Trace(visitor);
}

} // namespace blink
