/*
Minetest
Copyright (C) 2013 celeron55, Perttu Ahola <celeron55@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation; either version 2.1 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#pragma once
#include <functional>
#include "cpp_api/s_base.h"
#include "cpp_api/s_entity.h"
#include "cpp_api/s_env.h"
#include "cpp_api/s_inventory.h"
#include "cpp_api/s_modchannels.h"
#include "cpp_api/s_node.h"
#include "cpp_api/s_player.h"
#include "cpp_api/s_server.h"
#include "cpp_api/s_security.h"
#include "threading/mutex_auto_lock.h"
#include "threading/thread.h"

typedef std::function<void()> luacpp_void;

/*****************************************************************************/
/* Scripting <-> Server Game Interface                                       */
/*****************************************************************************/

class ServerScripting :
		virtual public ScriptApiBase,
		public ScriptApiDetached,
		public ScriptApiEntity,
		public ScriptApiEnv,
		public ScriptApiModChannels,
		public ScriptApiNode,
		public ScriptApiPlayer,
		public ScriptApiServer,
		public ScriptApiSecurity,
		public Thread
{
public:
	ServerScripting(Server* server);

	void *run();
	void signal();

	template<typename... Args>
	void runAsync(Args... args)
	{
		RecursiveMutexAutoLock lock(m_luastackmutex);
		m_queue_void.push(std::bind(args...));
	}

	template<typename... Args>
	void runSync(Args... args)
	{
		RecursiveMutexAutoLock lock(m_luastackmutex);
		std::bind(args...)();
	}

	// use ScriptApiBase::loadMod() to load mods

private:
	void InitializeModApi(lua_State *L, int top);

	std::queue<luacpp_void> m_queue_void;
};

void log_deprecated(const std::string &message);
