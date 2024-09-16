#pragma once
#include <iostream>
#include <memory>
#include <string>
#include <queue>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "packet.h"
#include "nocopyable.h"

namespace Nano {
	namespace Communication {
		class CEventHandler;
		class Session : public std::enable_shared_from_this<Session>
		{
		public:
			typedef std::shared_ptr<Session> Ptr;
			static const int MAX_SESSION_SEND_QUEUE_PENDING_SIZE = 64;
		public:
			Session(boost::asio::io_context& ioContext, CEventHandler& ceventHandler);
			virtual ~Session();
			void start();
			void close();
			void send(const char* data, int dataLength);
			std::string getUid() const;
			boost::asio::ip::tcp::socket& getSocket();
		private:
			void handleHeadRead(const boost::system::error_code& ec, size_t bytes_tramsferred);
			void handleBodyRead(const boost::system::error_code& ec, size_t bytes_tramsferred);
			void handleWrite(const boost::system::error_code& ec);
		private:
			std::string m_uid;
			boost::asio::ip::tcp::socket m_socket;

			std::shared_ptr<RecvPacket> m_recvHead;
			std::shared_ptr<RecvPacket> m_recvPacket;
			std::mutex m_sendMutex;

			std::queue<std::shared_ptr<SendPacket>> m_sendQueue;
			CEventHandler& m_ceventHandler;
		};
	}
}
