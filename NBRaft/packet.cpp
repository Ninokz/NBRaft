#include "packet.h"

namespace Nano
{
	namespace Communication
	{
		Packet::Packet() : m_data(nullptr), m_size(0)
		{
		}

		Packet::Packet(unsigned short size) : m_data(new char[size]()), m_size(size)
		{
		}

		Packet::~Packet()
		{
			if (m_data)
			{
				delete[] m_data;
				m_data = nullptr;
			}
		}

		Packet::Packet(const Packet& other)
		{
			m_size = other.m_size;
			m_data = new char[m_size];
			memcpy(m_data, other.m_data, m_size);
		}

		Packet& Packet::operator=(const Packet& other)
		{
			if (this != &other)
			{
				if (m_data)
				{
					delete[] m_data;
					m_data = nullptr;
				}
				m_size = other.m_size;
				m_data = new char[m_size];
				memcpy(m_data, other.m_data, m_size);
			}
			return *this;
		}

		std::string Packet::toString() const
		{
			return std::string(m_data, m_size);
		}

		SendPacket::SendPacket(const char* data, unsigned short size) : Packet(size + Packet::PACKET_HEAD_SIZE)
		{
			unsigned short s_size = boost::asio::detail::socket_ops::host_to_network_short(size);
			memcpy(m_data, &s_size, Packet::PACKET_HEAD_SIZE);
			memcpy(m_data + Packet::PACKET_HEAD_SIZE, data, size);
		}

		RecvPacket::RecvPacket(unsigned short size) :Packet(size)
		{
		}

		void RecvPacket::clear()
		{
			memset(m_data, 0, m_size);
		}
	}
}