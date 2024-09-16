#pragma once
#include "rpcserver.h"
#include "rpcservice.h"
#include "rpcprocedure.h"

namespace Nano {
	namespace Rpc {
		class RpcServerStub : public Noncopyable
		{
		public:
			typedef std::shared_ptr<RpcServerStub> Ptr;
		public:
			RpcServerStub(short port);
			~RpcServerStub();
			void registReturn(std::string methodName, std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap, ReturnFunc done);
			void registNotify(std::string methodName, std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap, NotifyFunc done);
			void run();
			void stop();
		private:
			RpcServer::Ptr m_rpcServer;
		};
	}
}