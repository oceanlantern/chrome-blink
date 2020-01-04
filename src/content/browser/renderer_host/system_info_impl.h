// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
// authors: tim adams oceanlantern@gmail.com

#ifndef CONTENT_BROWSER_RENDERER_HOST_SYSTEM_INFO_IMPL_H_
#define CONTENT_BROWSER_RENDERER_HOST_SYSTEM_INFO_IMPL_H_

#include <memory>
#include <string>
#include "base/macros.h"
#include "build/build_config.h"
#include "content/common/content_export.h"
#include "mojo/public/cpp/base/big_buffer.h"
#include "mojo/public/cpp/bindings/receiver.h"
#include "third_party/blink/public/mojom/systeminfo/system_info.mojom.h"

namespace content {

class CONTENT_EXPORT SystemInfoImpl : public blink::mojom::SystemInfo {
public:
    SystemInfoImpl();
    ~SystemInfoImpl() override;

    static void Create(mojo::PendingReceiver<blink::mojom::SystemInfo> receiver);

private:

    void Start(StartCallback callback) override;
    void Stop(StopCallback callback) override;
    void GetOperatingSystemName(GetOperatingSystemNameCallback callback) override;
};


}  // namespace content

#endif  // CONTENT_BROWSER_RENDERER_HOST_SYSTEM_INFO_IMPL_H_
