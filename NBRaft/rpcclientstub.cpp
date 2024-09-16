#include "rpcclientstub.h"

namespace Nano {
	namespace Rpc {
		RpcClientStub::RpcClientStub() : m_rpcClient(Rpc::RpcClientFactory::create())
		{
		}

		RpcClientStub::~RpcClientStub()
		{
		}

		void RpcClientStub::rpcReturnCall(std::string ip, short port, std::string id, std::string methodName,
			std::unordered_map<std::string, Json::Value> params,
			const ProcedureDoneCallback callback, int milliseconds_timeout)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createReturnCallRequest("2.0", methodName, id, params);
			m_rpcClient->connect(ip, port);
			this->m_rpcClient->callReturnProcedure(request, callback);
			std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
			m_rpcClient->disconnect();
		}

		void RpcClientStub::asyncRpcReturnCall(std::string ip, short port, std::string id, std::string methodName,
			std::unordered_map<std::string, Json::Value> params,
			const ProcedureDoneCallback callback, int milliseconds_timeout)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createReturnCallRequest("2.0", methodName, id, params);
			m_rpcClient->connect(ip, port);
			Nano::Concurrency::SimpleThreadPool::GetInstance()->submit([this, request, callback, milliseconds_timeout]() {
				this->m_rpcClient->callReturnProcedure(request, callback);
				std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
				m_rpcClient->disconnect();
				});
		}

		void RpcClientStub::rpcNotifyCall(std::string ip, short port, std::string methodName, std::unordered_map<std::string, Json::Value> params)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createNotifyCallRequest("2.0", methodName, params);
			m_rpcClient->connect(ip, port);
			this->m_rpcClient->callNotifyProcedure(request);
			m_rpcClient->disconnect();
		}

		void RpcClientStub::asyncRpcNotifyCall(std::string ip, short port, std::string methodName, std::unordered_map<std::string, Json::Value> params)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createNotifyCallRequest("2.0", methodName, params);
			m_rpcClient->connect(ip, port);
			Nano::Concurrency::SimpleThreadPool::GetInstance()->submit([this, request]() {
				this->m_rpcClient->callNotifyProcedure(request);
				m_rpcClient->disconnect();
				});
		}

		CallRecord::Ptr RpcClientStub::getReturnCallRecord(const std::string& id) {
			return m_rpcClient->getReturnCallRecord(id);
		}
	}
}