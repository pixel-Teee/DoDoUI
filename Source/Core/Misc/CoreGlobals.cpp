#include "CoreGlobals.h"

namespace DoDo {
	/*indicates that mainloop() should be exited at the end of the current iteration*/
	bool g_is_requesting_exit = false;
	void DoDo::request_engine_exit()
	{
		g_is_requesting_exit = true;
	}

	bool is_engine_exit_requested()
	{
		return g_is_requesting_exit;
	}
	
}