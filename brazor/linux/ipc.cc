#include "brazor/brazor.h"

#include <functional>


//-----------------------------------------------------------------------------
bool
debugger_module::is_emulator_running()
{
	// TBI

	return false;
}

//-----------------------------------------------------------------------------
bool
debugger_module::connect()
{
	// TBI

	return false;
}

//-----------------------------------------------------------------------------
void
debugger_module::disconnect()
{
    if (connected) {
        msg_process.request_stop();
        cv_pause.notify_all();
    }

    g_ipc_manager.client_disconnect();

	// TBI

    connected = false;
    online_mode = false;
}
