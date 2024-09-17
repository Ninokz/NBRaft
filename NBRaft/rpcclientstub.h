#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include <json/json.h>

#include "nocopyable.h"
#include "rpcclient.h"

#include "simplethreadpool.h"

namespace Nano {
	namespace Rpc {
		class RpcClientStub : public Noncopyable
		{
		public:
			typedef std::shared_ptr<RpcClientStub> Ptr;
			RpcClientStub();
			virtual ~RpcClientStub();

			bool connect(std::string ip, short port);
			void disconnect();
			CallRecord::Ptr rpcReturnCall(std::string id, std::string methodName, std::unordered_map<std::string, Json::Value> params, const ProcedureDoneCallback callback, int milliseconds_timeout);
			std::future<CallRecord::Ptr> asyncRpcReturnCall(std::string id, std::string methodName, std::unordered_map<std::string, Json::Value> params, const ProcedureDoneCallback callback, int milliseconds_timeout);
			bool rpcNotifyCall(std::string methodName, std::unordered_map<std::string, Json::Value> params);
			std::future<bool> asyncRpcNotifyCall(std::string methodName, std::unordered_map<std::string, Json::Value> params);

			static CallRecord::Ptr rpcReturnCallOnce(std::string ip, short port, std::string id, std::string methodName, std::unordered_map<std::string, Json::Value> params, const ProcedureDoneCallback callback, int milliseconds_timeout);
			static std::future<CallRecord::Ptr> asyncRpcReturnCallOnce(std::string ip, short port, std::string id, std::string methodName, std::unordered_map<std::string, Json::Value> params, const ProcedureDoneCallback callback, int milliseconds_timeout);
			static bool rpcNotifyCallOnce(std::string ip, short port, std::string methodName, std::unordered_map<std::string, Json::Value> params);
			static std::future<bool> asyncRpcNotifyCallOnce(std::string ip, short port, std::string methodName, std::unordered_map<std::string, Json::Value> params);
		protected:
			RpcClient::Ptr m_rpcClient;
			bool m_connected;
		};
	}
}