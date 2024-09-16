#include "ceventhandler.h"

namespace Nano {
	namespace Communication {
		CEventHandler::~CEventHandler() {
			this->m_connectHandlers.clear();
			this->m_closeHandlers.clear();
			this->m_dataReadyHandlers.clear();
		}

		void CEventHandler::addCloseHandler(std::weak_ptr<ICloseEventHandler> handler) {
			if (!handler.expired())
			{
				this->m_closeHandlers.push_back(handler);
			}
			else
			{
				throw std::runtime_error("handler is expired");
			}
		}

		void CEventHandler::addConnectHandler(std::weak_ptr<IConnectEventHandler> handler) {
			if (!handler.expired())
			{
				this->m_connectHandlers.push_back(handler);
			}
			else
			{
				throw std::runtime_error("handler is expired");
			}
		}

		void CEventHandler::addDataReadyHandler(std::weak_ptr<IDataReadyEventHandler> handler) {
			if (!handler.expired())
			{
				this->m_dataReadyHandlers.push_back(handler);
			}
			else
			{
				throw std::runtime_error("handler is expired");
			}
		}

		void CEventHandler::onClosed(std::shared_ptr<Session> sender) {
			std::lock_guard<std::mutex> lock(this->m_mutex);
			for (auto it = m_closeHandlers.begin(); it != m_closeHandlers.end();) {
				if (auto handler = it->lock()) {
					handler->onClosed(sender);
					++it;
				}
				else {
					it = m_closeHandlers.erase(it);
				}
			}
		}

		void CEventHandler::onConnected(std::shared_ptr<Session> sender) {
			std::lock_guard<std::mutex> lock(this->m_mutex);
			for (auto it = m_connectHandlers.begin(); it != m_connectHandlers.end();) {
				if (auto handler = it->lock()) {
					handler->onConnected(sender);
					++it;
				}
				else {
					it = m_connectHandlers.erase(it);
				}
			}
		}

		void CEventHandler::onDataReady(std::shared_ptr<Session> sender, std::shared_ptr<RecvPacket> packet) {
			std::lock_guard<std::mutex> lock(this->m_mutex);
			for (auto it = m_dataReadyHandlers.begin(); it != m_dataReadyHandlers.end();) {
				if (auto handler = it->lock()) {
					handler->onDataReady(sender, packet);
					++it;
				}
				else {
					it = m_dataReadyHandlers.erase(it);
				}
			}
		}
	}
}