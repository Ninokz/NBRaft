#pragma once
#include <memory>
#include <string>
#include <iostream>
#include <unordered_map>

#include "ceventhandler.h"
#include "session.h"

namespace Nano {
	namespace Communication {
		class SessionManager : public Noncopyable, public ICloseEventHandler, public IConnectEventHandler
		{
		public:
			typedef std::shared_ptr<SessionManager> Ptr;
			static const int MAS_SESSION_COUNT = 100;
		public:
			SessionManager(size_t maxSessions = MAS_SESSION_COUNT) : m_maxSessions(maxSessions) {}
			virtual ~SessionManager() {}
			virtual void onClosed(std::shared_ptr<Session> sender) override;
			virtual void onConnected(std::shared_ptr<Session> sender) override;
		private:
			inline bool addSession(std::shared_ptr<Session> session);
			inline void removeSession(std::string& uid);
		private:
			const size_t m_maxSessions;
			std::unordered_map<std::string, std::shared_ptr<Session>> m_sessions;
		};
	}
}