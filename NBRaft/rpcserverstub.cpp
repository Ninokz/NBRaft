#include "rpcserverstub.h"

namespace Nano {
	namespace Rpc {
		RpcServerStub::RpcServerStub(short port) :m_rpcServer(Rpc::RpcServerFactory::create(port))
		{
		}

		RpcServerStub::~RpcServerStub()
		{
		}

		void RpcServerStub::registReturn(std::string methodName, std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap, ReturnFunc done)
		{
			std::unique_ptr<ProcedureReturn> p = std::make_unique<ProcedureReturn>(
				std::move(done),
				std::move(paramsNameTypesMap)
			);
			m_rpcServer->addProcedureReturn(std::move(methodName), std::move(p));
		}

		void RpcServerStub::registNotify(std::string methodName, std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap, NotifyFunc done)
		{
			std::unique_ptr<ProcedureNotify> p = std::make_unique<ProcedureNotify>(
				std::move(done),
				std::move(paramsNameTypesMap)
			);
			m_rpcServer->addProcedureNotify(std::move(methodName), std::move(p));
		}

		void RpcServerStub::run()
		{
			m_rpcServer->start();
		}

		void RpcServerStub::stop()
		{
			m_rpcServer->stop();
		}
	}
}