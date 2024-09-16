#pragma once
#include <iostream>
#include <memory>

#include <boost/asio.hpp>

namespace Nano
{
	namespace Communication
	{
		static inline bool encode(const std::string& str, char** buffer, int* len) {
			size_t length = str.size();
			if (length > INT32_MAX)
				throw std::runtime_error("buffer size is too large");
			*len = static_cast<int>(length);
			*buffer = new char[length];
			memcpy(*buffer, str.c_str(), length);
			return true;
		}

		static inline std::string decode(const char* buffer, int len) {
			std::string str(buffer, len);
			return std::move(str);
		}

		class Packet
		{
		public:
			typedef std::shared_ptr<Packet> Ptr;
			typedef unsigned short SizeType;

			static const int PACKET_HEAD_SIZE = 4;
			static const int PACKET_MAX_SIZE = 1024;
		public:
			Packet();
			Packet(unsigned short size);
			virtual ~Packet();

			Packet(const Packet& other);
			Packet& operator=(const Packet& other);

			std::string toString() const;
		public:
			char* m_data;
			unsigned short m_size;
		};

		class RecvPacket : public Packet
		{
		public:
			typedef std::shared_ptr<RecvPacket> Ptr;
		public:
			RecvPacket() = delete;
			RecvPacket(unsigned short size);
			void clear();
		};

		class SendPacket : public Packet
		{
		public:
			typedef std::shared_ptr<SendPacket> Ptr;
		public:
			SendPacket() = delete;
			SendPacket(unsigned short size) = delete;
			SendPacket(const char* data, unsigned short size);
		};
	}
}