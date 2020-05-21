# chrome-blink
This project creates a new WebIDL and native C++ interface in Chromium's Blink component that is callable by JavaScript.  The client implementation is in the Renderer process and the Host implementation is in the Browser process. Inter-process communication is implemented using Mojo IPC and only the Browser process code calls abstracted native APIs - following Chromium’s security and implementation model.  

The project is cross-platform and has been tested on Windows, Macintosh and Linux operating systems.
    
## Background
Blink is the layout and rendering engine used by Chrome.  It shares the DOM and works in conjunction with the JavaScript V8 engine to produce a visual that is rasterized or output by the GPU component.

In order for JavaScript to be able to call Blink an interface is needed.  This interface is implemented using WebIDL which provides the  declaration for the functions to be called.

The interface can contain many types of declarations including functions, attributes, types and others.  For each WebIDL declaration there must be a corresponding C++ implementation header and C file.  A WebIDL interface definition corresponds to a C++ implementation class.

See WebIDL draft here: https://heycam.github.io/webidl/#introduction

## Building
Follow instructions below to build on your system it requires Chrome tag 81.0.4010.0.

- Build Chrome by following the Chrome build instructions for your operating system.
- Change directory to src directory
- git fetch --tags
- git checkout -b br_81.0.4010.0 81.0.4010.0
- copy patch file from patch/systeminfo.patch to your src/systeminfo.patch
- git apply systeminfo.patch
- Windows / Mac:
    - gclient sync --with_branch_heads --with_tags 
- Linux:
    - gclient sync
- Build chrome e.g. autoninja -C out\dbg64 chrome
- Launch chrome
- Open test web page web/index.html
- Select startSystemInfo button  

## Blink API Implementation Diagram

![Blink API Diagram](/images/blink_api_diagram.png)

## Blink / Renderer Process
Blink code runs in the sandboxed Renderer process which means it is restricted in what it can access using operating system APIs.  To gain access to needed functionality Blink must communicate with the Browser process that runs at the interactive users security level this done using Mojo IPC.

### New Blink APIs: src/third_party/blink/renderer/modules/systeminfo  
https://github.com/oceanlantern/chrome-blink/tree/master/src/third_party/blink/renderer/modules/systeminfo

To create a new WebIDL interface the first step is to create a new directory under third_party/blink/renderer\modules.  For this project the new module is called SystemInfo.  So a new directory named SystemInfo is created under third_party/blink/renderer/modules/systeminfo.

The fist file to be created is the SystemInfo.idl which describes the new interface and its APIs using WebIDL.  If you open the Systeminfo.idl file you'll see the declaration of its interface and its function and eventhandlers.  These are the functions that will be called from JavaScript or in the case of events be called by native code to JavaScript.

The next step is to create a corresponding C++ and header file.  This is the native interface implementation code.  If you open Systeminfo.cpp and Systeminfo.h you'll see corresponding native functions that we declared in SystemInfo.idl.  

### The functions are:  
- start
- stop  
- getOperatingSystemName  

### The events are:  
- start  
- stop  

Each of the native functions that are implemented follows the same model.  
On entry sanity checks are done - the first is to test for Execution context and its validity and the second is to check for an active document. If either of the tests fail a DOM exception is thrown in JavaScript.

Then a GarbageCollector ScriptPromiseResolver object is created.  This is the native implementation of the JavaScript Promise and is the return value from the function.  Then the actual implementation function is asynchronously called in the Browser process via a Mojo IPC interface.  The function being called has a callback parameter that is called when the function execution is completed.  The callback parameter is implemented as a C++ lambda function with bound parameters.  The bound parameters passed to the callback function along with the return value from the called function if any.  In our case the bound parameters are the ScriptPromiseResolver and the class object.

When the function call completes the ScriptPromiseResolver is used to either complete the JavaScript Promise successfully and returns any result to JavaScript or fails it by throwing a DOM exception which the JavaScript catches. 

### Example WebIDL function:  
[CallWith = ScriptState, RaisesException, Measure] Promise<DomString> getOperatingSystemName()  

### Example C++ declaration of WebIDL function in Blink:   
ScriptPromise SystemInfo::getOperatingSystemName(ScriptState* script_state, ExceptionState& exception_state) 

### Example Blink calling implementation function in Browser process using Bind and asynchronous lambda callback function:  

    system_info_->GetOperatingSystemName(WTF::Bind([](
        SystemInfo* systemInfo,
        ScriptPromiseResolver* resolver,
        const WTF::String& result) {

        if (result.length()) {

          VLOG(1) << "SystemInfo::GetOperatingSystemName succeeded";
          resolver->Resolve(result);
        }
        else {

          VLOG(0) << "SystemInfo::GetOperatingSystemName failed " << result;

          resolver->Reject(
            MakeGarbageCollected<DOMException>(DOMExceptionCode::kNotSupportedError,					            (char*)"SystemInfo::start failed"));
        }

      }, WrapPersistent(this), WrapPersistent(resolver)));


### Mojo Apis: src/third_party/blink/public/mojom/systeminfo/system_info.mojom
In order for Blink to communicate with Browser process a Mojo IPC must be defined.  For our project a new Mojo IPC file is created under third_party/blink/public/mojom/systeminfo with the name of the interface which is SystemInfo in our case.  The actual mojo file is named system_info.mojom and contains the specific function definitions.  Mojo IPCs have client part that operates in the Renderer process and an implementation part that is in the Browser process.  When a mojo file is compiled it generates the necessary header files for the client and implementation.  The implementation must fill all the functions since they are defined as pure virtual or compiler error will occur.

= Example of Mojo Api:
 GetOperatingSystemName() =>
      (string result);

## Content / Browser Process: src/content//browser/renderer_host/systeminfo.cc
https://github.com/oceanlantern/chrome-blink/tree/master/src/content/browser/renderer_host

The implementation end of the Mojo IPC lives in the Browser process and implements the functionality required and then executes the callback which returns control to the renderer process and executes the lambda callback function.  

- Example Mojo implementation function in Browser process retrieving operating system name and running callback:  

void SystemInfoImpl::GetOperatingSystemName(GetOperatingSystemNameCallback callback) {
  std::move(callback).Run(base::SysInfo::OperatingSystemName());
}

## Unittest
Use target blink_unittests to build SystemInfo unittest.

## Generated Files
When an IDL and or Mojom file is compiled code is generated that binds implementations together.  
These bindings are accessed via header files and are placed under out\<build dir>\gen directory.  
They are included using the same relative paths from the location of the component.

For example on the Blink side the SystemInfo Mojo interface is included using this generated file:
#include "third_party/blink/public/mojom/systeminfo/system_info.mojom-blink.h"

And on the Browser implementation side the SystemInfo Mojo interface is included using this generated file:
#include "third_party/blink/public/mojom/systeminfo/system_info.mojom.h"

## Windows Screenshot

![Windows Screenshot](/images/windows_systeminfo.png)

## macOS Screenshot

![Macintosh Screenshot](/images/mac_systeminfo.png)

## Linux Screenshot

![Linux Screenshot](/images/linux_systeminfo.png)

