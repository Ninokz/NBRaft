#pragma once
#include <thread>
#include <memory>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "nocopyable.h"
#include "ceventhandler.h"
#include "session.h"

namespace Nano {
	namespace Communication {
		class BaseClient
		{
		public:
			typedef std::shared_ptr<BaseClient> Ptr;
		public:
			BaseClient();
			virtual ~BaseClient();
			bool connect(const std::string& ip, short port);
			void disconnect();
			void send(const char* data, int dataLength);
		public:
			std::shared_ptr<CEventHandler> m_ceventHandler;
		protected:
			bool m_running;
			boost::asio::ip::tcp::endpoint m_endpoint;
			std::thread m_clientThread;
			boost::asio::io_context m_ioc;
			std::shared_ptr<Session> m_session;
		};
	}
}