#pragma once
#include <iostream>
#include <memory>
#include <vector>

#include "packet.h"
#include "session.h"

namespace Nano {
	namespace Communication {
		class ICloseEventHandler {
		public:
			virtual ~ICloseEventHandler() = default;
			virtual void onClosed(std::shared_ptr<Session> sender) = 0;
		};

		class IConnectEventHandler
		{
		public:
			virtual ~IConnectEventHandler() = default;
			virtual void onConnected(std::shared_ptr<Session> sender) = 0;
		};

		class IDataReadyEventHandler
		{
		public:
			virtual ~IDataReadyEventHandler() = default;
			virtual void onDataReady(std::shared_ptr<Session> sender, std::shared_ptr<RecvPacket> packet) = 0;
		};

		class CEventHandler : public std::enable_shared_from_this<CEventHandler>
		{
		public:
			typedef std::shared_ptr<CEventHandler> Ptr;
		public:
			CEventHandler() = default;
			virtual ~CEventHandler();

			void addCloseHandler(std::weak_ptr<ICloseEventHandler> handler);
			void addConnectHandler(std::weak_ptr<IConnectEventHandler> handler);
			void addDataReadyHandler(std::weak_ptr<IDataReadyEventHandler> handler);

			void onClosed(std::shared_ptr<Session> sender);
			void onConnected(std::shared_ptr<Session> sender);
			void onDataReady(std::shared_ptr<Session> sender, std::shared_ptr<RecvPacket> packet);
		protected:
			std::vector<std::weak_ptr<ICloseEventHandler>> m_closeHandlers;
			std::vector<std::weak_ptr<IConnectEventHandler>> m_connectHandlers;
			std::vector<std::weak_ptr<IDataReadyEventHandler>> m_dataReadyHandlers;
			std::mutex m_mutex;
		};
	}
}