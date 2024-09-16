#include "rpcservice.h"

namespace Nano {
	namespace Rpc {
		void RpcService::callProcedureReturn(std::string methodName,
			Json::Value& request,
			const ProcedureDoneCallback& done)
		{
			auto it = m_procedureReturn.find(methodName);
			if (it == m_procedureReturn.end())
				throw RpcException(JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound),
					JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound));
			if (it->second)
				it->second->invoke(request, done);
			else
				throw RpcException(JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::InternalError),
					JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::InternalError));
		}

		void RpcService::callProcedureNotify(std::string methodName,
			Json::Value& request)
		{
			auto it = m_procedureNotfiy.find(methodName);
			if (it == m_procedureNotfiy.end())
				throw RpcException(JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound),
					JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::MethodNotFound));
			if (it->second)
				it->second->invoke(request);
			else
				throw RpcException(JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::InternalError),
					JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::InternalError));
		}

		bool RpcService::hasProcedureReturn(std::string methodName)
		{
			return this->m_procedureReturn.find(methodName) != this->m_procedureReturn.end();
		}

		bool RpcService::hasProcedureNotify(std::string methodName)
		{
			return this->m_procedureNotfiy.find(methodName) != this->m_procedureNotfiy.end();
		}
	}
}