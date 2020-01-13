// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// authors: tim adams oceanlantern@gmail.com

#include "content/browser/renderer_host/system_info_impl.h"

#include <utility>
#include "base/bind.h"
#include "base/location.h"
#include "base/macros.h"
#include "base/pickle.h"
#include "base/sequenced_task_runner.h"
#include "base/strings/utf_string_conversions.h"
#include "base/threading/sequenced_task_runner_handle.h"
#include "build/build_config.h"
#include "mojo/public/cpp/system/platform_handle.h"
#include "mojo/public/cpp/bindings/callback_helpers.h"
#include "mojo/public/cpp/bindings/self_owned_receiver.h"
#include "base/system/sys_info.h"

namespace content
{

SystemInfoImpl::SystemInfoImpl()
{
}
SystemInfoImpl::~SystemInfoImpl() = default;

void SystemInfoImpl::Create(
    mojo::PendingReceiver<blink::mojom::SystemInfo> receiver)
{

  mojo::MakeSelfOwnedReceiver(std::make_unique<SystemInfoImpl>(),
                              std::move(receiver));
}

// ============================================================================
//
// GetOperatingSystemName:
//
// C++ implementation of GetOperatingSystemName function executing in Browser
// process called from Blink/Renderer process
//
// This function queries the operating system name using the abstracted base APIs.
// The callback is run and the lambda callback function is executed in Blink/Renderer 
// process
//
//=============================================================================

void SystemInfoImpl::GetOperatingSystemName(GetOperatingSystemNameCallback callback)
{
  std::move(callback).Run(base::SysInfo::OperatingSystemName());
}

// ============================================================================
//
// Start:
//
// C++ implementation of Start function executing in Browser
// process called from Blink/Renderer process
//
// For this example function the callback is run and the lambda callback
// function is executed in Blink/Renderer process
//
//=============================================================================

void SystemInfoImpl::Start(StartCallback callback)
{

  std::move(callback).Run(true);
}

// ============================================================================
//
// Stop:
//
// C++ implementation of Stop function executing in Browser
// process called from Blink/Renderer process
//
// For this example function the callback is run and the lambda callback
// function is executed in Blink/Renderer process
//
//=============================================================================

void SystemInfoImpl::Stop(StopCallback callback)
{
  std::move(callback).Run();
}

} // namespace content
