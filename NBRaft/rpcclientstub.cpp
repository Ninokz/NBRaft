#include "rpcclientstub.h"

namespace Nano {
	namespace Rpc {
		RpcClientStub::RpcClientStub() : m_rpcClient(Rpc::RpcClientFactory::create()), m_connected(false)
		{
		}

		RpcClientStub::~RpcClientStub()
		{
		}

		bool RpcClientStub::connect(std::string ip, short port)
		{
			if (m_rpcClient->connect(ip, port))
			{
				m_connected = true;
				return true;
			}
			else
			{
				m_connected = false;
				return false;
			}
		}

		void RpcClientStub::disconnect()
		{
			m_rpcClient->disconnect();
			m_connected = false;
		}

		CallRecord::Ptr RpcClientStub::rpcReturnCall(std::string id, std::string methodName, std::unordered_map<std::string, Json::Value> params, const ProcedureDoneCallback callback, int milliseconds_timeout)
		{
			if (m_connected)
			{
				Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createReturnCallRequest("2.0", methodName, id, params);
				this->m_rpcClient->callReturnProcedure(request, callback);
				std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
				return this->m_rpcClient->getReturnCallRecord(id);
			}
			else
			{
				return nullptr;
			}
		}

		std::future<CallRecord::Ptr> RpcClientStub::asyncRpcReturnCall(std::string id, std::string methodName, std::unordered_map<std::string, Json::Value> params, const ProcedureDoneCallback callback, int milliseconds_timeout)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createReturnCallRequest("2.0", methodName, id, params);
			// Create a promise to hold the CallRecord::Ptr result
			auto promise = std::make_shared<std::promise<CallRecord::Ptr>>();
			// Get the future associated with the promise
			std::future<CallRecord::Ptr> future = promise->get_future();
			if (m_connected)
			{
				// Submit the async task to the thread pool
				Nano::Concurrency::SimpleThreadPool::GetInstance()->submit([this, request, callback, milliseconds_timeout, id, promise]() {
					this->m_rpcClient->callReturnProcedure(request, callback);
					std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
					promise->set_value(this->m_rpcClient->getReturnCallRecord(id));
					});
				return future;
			}
			else
			{
				// Set an empty result in the promise in case of connection failure
				promise->set_value(nullptr);
				return future;
			}
		}

		bool RpcClientStub::rpcNotifyCall(std::string methodName, std::unordered_map<std::string, Json::Value> params)
		{
			if (m_connected)
			{
				Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createNotifyCallRequest("2.0", methodName, params);
				this->m_rpcClient->callNotifyProcedure(request);
				return true;
			}
			else
			{
				return false;
			}
		}

		std::future<bool> RpcClientStub::asyncRpcNotifyCall(std::string methodName, std::unordered_map<std::string, Json::Value> params)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createNotifyCallRequest("2.0", methodName, params);
			// Create a promise to hold the bool result
			auto promise = std::make_shared<std::promise<bool>>();
			// Get the future associated with the promise
			std::future<bool> future = promise->get_future();
			if (m_connected)
			{
				// Submit the async task to the thread pool
				Nano::Concurrency::SimpleThreadPool::GetInstance()->submit([this, request, promise]() {
					this->m_rpcClient->callNotifyProcedure(request);
					promise->set_value(true);
					});
				return future;
			}
			else
			{
				// Set an empty result in the promise in case of connection failure
				promise->set_value(false);
				return future;
			}
		}


		CallRecord::Ptr RpcClientStub::getReturnCallRecord(const std::string& id) {
			return m_rpcClient->getReturnCallRecord(id);
		}

		CallRecord::Ptr RpcClientStub::rpcReturnCallOnce(std::string ip, short port, std::string id, std::string methodName,
			std::unordered_map<std::string, Json::Value> params,
			const ProcedureDoneCallback callback, 
			int milliseconds_timeout)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createReturnCallRequest("2.0", methodName, id, params);
			auto tmp_rpcClient = Rpc::RpcClientFactory::create();
			if (tmp_rpcClient->connect(ip, port))
			{
				tmp_rpcClient->callReturnProcedure(request, callback);
				std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
				tmp_rpcClient->disconnect();
				return tmp_rpcClient->getReturnCallRecord(id);
			}
			else
			{
				return nullptr;
			}
		}

		std::future<CallRecord::Ptr> RpcClientStub::asyncRpcReturnCallOnce(std::string ip, short port, std::string id, std::string methodName,
			std::unordered_map<std::string, Json::Value> params,
			const ProcedureDoneCallback callback, 
			int milliseconds_timeout)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createReturnCallRequest("2.0", methodName, id, params);
			auto tmp_rpcClient = Rpc::RpcClientFactory::create();
			// Create a promise to hold the CallRecord::Ptr result
			auto promise = std::make_shared<std::promise<CallRecord::Ptr>>();
			// Get the future associated with the promise
			std::future<CallRecord::Ptr> future = promise->get_future();
			if (tmp_rpcClient->connect(ip, port))
			{
				// Submit the async task to the thread pool
				Nano::Concurrency::SimpleThreadPool::GetInstance()->submit([tmp_rpcClient, request, callback, milliseconds_timeout, id, promise]() {
					tmp_rpcClient->callReturnProcedure(request, callback);
					std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
					tmp_rpcClient->disconnect();

					// Set the result in the promise
					promise->set_value(tmp_rpcClient->getReturnCallRecord(id));
					});

				return future;
			}
			else
			{
				// Set an empty result in the promise in case of connection failure
				promise->set_value(nullptr);
				return future;
			}
		}

		bool RpcClientStub::rpcNotifyCallOnce(std::string ip, short port, std::string methodName, 
			std::unordered_map<std::string, Json::Value> params)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createNotifyCallRequest("2.0", methodName, params);
			auto tmp_rpcClient = Rpc::RpcClientFactory::create();
			if (tmp_rpcClient->connect(ip, port))
			{
				tmp_rpcClient->callNotifyProcedure(request);
				tmp_rpcClient->disconnect();
				return true;
			}
			else
			{
				return false;
			}
		}

		std::future<bool> RpcClientStub::asyncRpcNotifyCallOnce(std::string ip, short port, std::string methodName, 
			std::unordered_map<std::string, Json::Value> params)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createNotifyCallRequest("2.0", methodName, params);
			auto tmp_rpcClient = Rpc::RpcClientFactory::create();
			// Create a promise to hold the bool result
			auto promise = std::make_shared <std::promise<bool>>();
			// Get the future associated with the promise
			std::future<bool> future = promise->get_future();
			if (tmp_rpcClient->connect(ip, port))
			{
				// Submit the async task to the thread pool
				Nano::Concurrency::SimpleThreadPool::GetInstance()->submit([tmp_rpcClient, request, promise]() {
					tmp_rpcClient->callNotifyProcedure(request);
					tmp_rpcClient->disconnect();
					promise->set_value(true);
					});
				return future;
			}
			else
			{
				// Set an empty result in the promise in case of connection failure
				promise->set_value(false);
				return future;
			}
		}
	}
}