// authors: tim adams oceanlantern@gmail.com 

let systemInfo;

async function startSystemInfo() {

  try {
    systemInfo = null;

    systemInfo = new SystemInfo();

    if (systemInfo){        
      systemInfo.onstart = onStartSystemInfo; // Setup events
      systemInfo.onstop = onStopSystemInfo;

      await systemInfo.start();

      var name = await systemInfo.getOperatingSystemName();

      consoleMsg('Operating System Name: ' + name);

      stopSystemInfo();

    }
  }
  catch (e) {
    consoleMsg(e);
    
  }     
}

async function stopSystemInfo() {
  if (systemInfo){
    systemInfo.stop();
    systemInfo = null;
  }
  else{
  }
}

function onStartSystemInfo() {
    consoleMsg("onStartSystemInfo - Event");
}

function onStopSystemInfo() {
    consoleMsg("onStopSystemInfo - Event");
}


