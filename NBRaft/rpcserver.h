#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "ceventhandler.h"
#include "session.h"
#include "packet.h"

#include "baseserver.h"
#include "rpcservice.h"
#include "rpcprocedure.h"

#include "stealThreadPool.h"
#include "functionWrapper.h"

#include "jrpcproto.h"
#include "rpcexception.h"

namespace Nano {
	namespace Rpc {
		class RpcServer : public Communication::BaseServer, public Communication::IDataReadyEventHandler,
			public std::enable_shared_from_this<RpcServer>
		{
			friend class RpcServerFactory;
		public:
			typedef std::shared_ptr<RpcServer> Ptr;
		protected:
			RpcServer(short port);
			void Init();
		public:
			virtual ~RpcServer();
			void addProcedureReturn(std::string methodName, RpcService::ProcedureReturnUniqPtr p);
			void addProcedureNotify(std::string methodName, RpcService::ProcedureNotifyUniqPtr p);
		private:
			void onDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet) override;
			void handleProcedureReturn(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request);
			void handleProcedureNotify(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request);
			void handleJsonRpcErrorException(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcError::Ptr error);
		private:
			RpcService::Ptr m_rpcService;
		};

		class RpcServerFactory
		{
		public:
			static RpcServer::Ptr create(short port) {
				auto ptr = std::shared_ptr<RpcServer>(new RpcServer(port));
				ptr->Init();
				return ptr;
			}
		};
	}
}