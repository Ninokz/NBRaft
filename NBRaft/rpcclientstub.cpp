#include "rpcclientstub.h"

namespace Nano {
	namespace Rpc {
		RpcClientStub::RpcClientStub() : m_rpcClient(Rpc::RpcClientFactory::create()), m_connected(false)
		{
		}

		RpcClientStub::~RpcClientStub()
		{
			if (m_connected)
			{
				disconnect();
			}
		}

		bool RpcClientStub::connect(std::string ip, short port)
		{
			if (!m_connected)
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
			else
			{
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
				if (this->m_rpcClient->callReturnProcedure(request, callback))
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
					auto record =  this->m_rpcClient->getReturnCallRecord(id);
					if (!record->isError() && record->isDone())
					{
						this->m_rpcClient->removeCallRecord(id);
					}
					return record;
				}
			}
			return std::make_shared<CallRecord>();
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
					if (this->m_rpcClient->callReturnProcedure(request, callback))
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
						auto record = this->m_rpcClient->getReturnCallRecord(id);
						if (!record->isError() && record->isDone())
						{
							this->m_rpcClient->removeCallRecord(id);
						}
						promise->set_value(record);
					}
					else
					{
						promise->set_value(std::make_shared<CallRecord>());
					}
				});
				return future;
			}
			else
			{
				promise->set_value(std::make_shared<CallRecord>());
				return future;
			}
		}

		bool RpcClientStub::rpcNotifyCall(std::string methodName, std::unordered_map<std::string, Json::Value> params)
		{
			if (m_connected)
			{
				Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createNotifyCallRequest("2.0", methodName, params);
				return this->m_rpcClient->callNotifyProcedure(request);
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
					promise->set_value(this->m_rpcClient->callNotifyProcedure(request));
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

		CallRecord::Ptr RpcClientOnceStub::rpcReturnCallOnce(std::string ip, short port, std::string id, std::string methodName,
			std::unordered_map<std::string, Json::Value> params,
			const ProcedureDoneCallback callback, 
			int milliseconds_timeout)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createReturnCallRequest("2.0", methodName, id, params);
			auto tmp_rpcClient = Rpc::RpcClientFactory::create();
			if (tmp_rpcClient->connect(ip, port))
			{
				if (tmp_rpcClient->callReturnProcedure(request, callback))
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
					tmp_rpcClient->disconnect();
					return tmp_rpcClient->getReturnCallRecord(id);
				}
			}
			return std::make_shared<CallRecord>();
		}

		std::future<CallRecord::Ptr> RpcClientOnceStub::asyncRpcReturnCallOnce(std::string ip, short port, std::string id, std::string methodName,
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
					if (tmp_rpcClient->callReturnProcedure(request, callback))
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds_timeout));
						tmp_rpcClient->disconnect();
						// Set the result in the promise
						promise->set_value(tmp_rpcClient->getReturnCallRecord(id));
					}
					else
					{
						// Set an empty result in the promise in case of connection failure
						promise->set_value(std::make_shared<CallRecord>());
					}
				});

				return future;
			}
			else
			{
				// Set an empty result in the promise in case of connection failure
				promise->set_value(std::make_shared<CallRecord>());
				return future;
			}
		}

		bool RpcClientOnceStub::rpcNotifyCallOnce(std::string ip, short port, std::string methodName,
			std::unordered_map<std::string, Json::Value> params)
		{
			Nano::JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createNotifyCallRequest("2.0", methodName, params);
			auto tmp_rpcClient = Rpc::RpcClientFactory::create();
			if (tmp_rpcClient->connect(ip, port))
			{
				bool res = tmp_rpcClient->callNotifyProcedure(request);
				tmp_rpcClient->disconnect();
				return res;
			}
			else
			{
				return false;
			}
		}

		std::future<bool> RpcClientOnceStub::asyncRpcNotifyCallOnce(std::string ip, short port, std::string methodName,
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
					bool res = tmp_rpcClient->callNotifyProcedure(request);
					tmp_rpcClient->disconnect();
					promise->set_value(res);
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