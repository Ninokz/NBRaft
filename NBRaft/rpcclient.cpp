#include "rpcclient.h"

namespace Nano {
	namespace Rpc {
		RpcClient::RpcClient()
		{
		}

		RpcClient::~RpcClient()
		{
			this->m_callRecords.clear();
		}

		void RpcClient::Init()
		{
			this->m_ceventHandler->addDataReadyHandler(weak_from_this());
		}

		bool RpcClient::callReturnProcedure(JrpcProto::JsonRpcRequest::Ptr request, const ProcedureDoneCallback callback)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_callRecords.find(request->getId()) != m_callRecords.end())
				return false;
			// 1. record
			CallRecord::Ptr reRecord = std::make_shared<CallRecord>(request);
			m_callRecords.emplace(request->getId(), std::make_pair(reRecord, callback));
			// 2. call
			std::string jsonStyleStr = request->toJsonStr();
			char* buffer = nullptr;
			int len = 0;
			if (Nano::Communication::encode(jsonStyleStr, &buffer, &len))
			{
				this->send(buffer, len);
				delete[] buffer;
				buffer = nullptr;
				return true;
			}
			else
			{
				return false;
			}
		}

		bool RpcClient::callNotifyProcedure(JrpcProto::JsonRpcRequest::Ptr request)
		{
			std::string jsonStyleStr = request->toJsonStr();
			char* buffer = nullptr;
			int len = 0;
			if (Nano::Communication::encode(jsonStyleStr, &buffer, &len))
			{
				this->send(buffer, len);
				delete[] buffer;
				buffer = nullptr;
				return true;
			}
			else
			{
				return false;
			}
		}

		CallRecord::Ptr RpcClient::getReturnCallRecord(const std::string& id)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			auto it = m_callRecords.find(id);
			if (it != m_callRecords.end())
				return it->second.first;
			return nullptr;
		}

		void RpcClient::clearCallRecords()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			m_callRecords.clear();
		}

		void RpcClient::removeCallRecord(const std::string& id)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			auto it = m_callRecords.find(id);
			if (it != m_callRecords.end())
				m_callRecords.erase(it);
		}

		void RpcClient::onDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet)
		{
			std::string responseJsonStr = Nano::Communication::decode(packet->m_data, packet->m_size);
			bool generateResult = false;
			Nano::JrpcProto::JsonRpcResponse::Ptr response = Nano::JrpcProto::JsonRpcResponseFactory::createResponseFromJsonStr(responseJsonStr, &generateResult);
			if (generateResult)
			{
				std::lock_guard<std::mutex> lock(m_mutex);
				auto it = m_callRecords.find(response->getId());
				if (it != m_callRecords.end())
				{
					it->second.first->response = response;
					it->second.second(response->getResult());
				}
			}
		}
	}
}