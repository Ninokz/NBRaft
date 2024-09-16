#include "session.h"
#include "ceventHandler.h"

namespace Nano {
	namespace Communication {
		Session::Session(boost::asio::io_context& ioContext, CEventHandler& ceventHandler) :
			m_socket(ioContext),
			m_ceventHandler(ceventHandler),
			m_uid(boost::uuids::to_string(boost::uuids::random_generator()())),
			m_recvHead(std::make_shared<RecvPacket>(Packet::PACKET_HEAD_SIZE))
		{
		}

		Session::~Session()
		{
		}

		void Session::start()
		{
			try {
				boost::asio::async_read(
					m_socket,
					boost::asio::buffer(m_recvHead->m_data, Packet::PACKET_HEAD_SIZE),
					boost::bind(&Session::handleHeadRead, shared_from_this(),
						std::placeholders::_1, std::placeholders::_2)
				);
				m_ceventHandler.onConnected(shared_from_this());
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				this->close();
			}
		}

		void Session::close()
		{
			m_ceventHandler.onClosed(shared_from_this());
			m_socket.close();
		}

		void Session::send(const char* data, int dataLength)
		{
			try {
				bool queuePending = false;
				std::lock_guard<std::mutex> lock(m_sendMutex);
				if (this->m_sendQueue.size() >= Session::MAX_SESSION_SEND_QUEUE_PENDING_SIZE) {
					return;
				}
				if (!this->m_sendQueue.empty()) {
					queuePending = true;
				}
				this->m_sendQueue.push(std::make_shared<SendPacket>(data, dataLength));
				if (queuePending)
				{
					return;
				}
				auto& nxtPacket = this->m_sendQueue.front();
				boost::asio::async_write(
					this->m_socket,
					boost::asio::buffer(nxtPacket->m_data, nxtPacket->m_size),
					boost::bind(&Session::handleWrite, shared_from_this(), std::placeholders::_1)
				);
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				this->close();
			}
		}

		std::string Session::getUid() const {
			return this->m_uid;
		}

		boost::asio::ip::tcp::socket& Session::getSocket() {
			return m_socket;
		}

		void Session::handleHeadRead(const boost::system::error_code& ec, size_t bytes_tramsferred)
		{
			try {
				if (!ec)
				{
					if (bytes_tramsferred < Packet::PACKET_HEAD_SIZE || bytes_tramsferred > Packet::PACKET_HEAD_SIZE)
					{
						this->close();
						return;
					}
					unsigned short dataSize = 0;
					memcpy(&dataSize, m_recvHead->m_data, Packet::PACKET_HEAD_SIZE);
					dataSize = boost::asio::detail::socket_ops::network_to_host_short(dataSize);

					if (dataSize > Packet::PACKET_MAX_SIZE)
					{
						this->close();
						return;
					}
					m_recvPacket = std::make_shared<RecvPacket>(dataSize);

					boost::asio::async_read(
						m_socket,
						boost::asio::buffer(m_recvPacket->m_data, m_recvPacket->m_size),
						boost::bind(&Session::handleBodyRead, shared_from_this(),
							std::placeholders::_1, std::placeholders::_2)
					);
				}
				else {
					this->close();
				}
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				this->close();
			}
		}

		void Session::handleBodyRead(const boost::system::error_code& ec, size_t bytes_tramsferred)
		{
			try {
				if (!ec)
				{
					/// copy m_recvPacket
					auto newPacket = std::make_shared<RecvPacket>(*m_recvPacket);
					/// notify data ready
					this->m_ceventHandler.onDataReady(shared_from_this(), newPacket);
					/// read next packet
					this->m_recvPacket->clear();
					boost::asio::async_read(
						m_socket,
						boost::asio::buffer(m_recvHead->m_data, Packet::PACKET_HEAD_SIZE),
						boost::bind(&Session::handleHeadRead, shared_from_this(),
							std::placeholders::_1, std::placeholders::_2)
					);
				}
				else
				{
					this->close();
				}
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				this->close();
			}
		}

		void Session::handleWrite(const boost::system::error_code& ec)
		{
			try {
				if (!ec) {
					std::lock_guard<std::mutex> lock(m_sendMutex);
					this->m_sendQueue.pop();
					if (!this->m_sendQueue.empty())
					{
						auto& nxtPacket = this->m_sendQueue.front();
						boost::asio::async_write(
							this->m_socket,
							boost::asio::buffer(nxtPacket->m_data, nxtPacket->m_size),
							boost::bind(&Session::handleWrite, shared_from_this(), std::placeholders::_1)
						);
					}
				}
				else {
					close();
				}
			}
			catch (std::exception& e) {
				std::cerr << e.what() << std::endl;
				this->close();
			}
		}
	}
}