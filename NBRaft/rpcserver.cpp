#include "rpcserver.h"

namespace Nano {
	namespace Rpc {
		RpcServer::RpcServer(short port) : Communication::BaseServer(port), m_rpcService(std::make_unique<RpcService>())
		{

		}

		RpcServer::~RpcServer()
		{
		}

		void RpcServer::Init()
		{
			m_ceventHandler->addDataReadyHandler(weak_from_this());
		}

		void RpcServer::addProcedureReturn(std::string methodName, RpcService::ProcedureReturnUniqPtr p)
		{
			m_rpcService->addProcedureReturn(std::move(methodName), std::move(p));
		}

		void RpcServer::addProcedureNotify(std::string methodName, RpcService::ProcedureNotifyUniqPtr p)
		{
			m_rpcService->addProcedureNotify(std::move(methodName), std::move(p));
		}

		void RpcServer::onDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet)
		{
			try {
				std::string requestJsonStr = Nano::Communication::decode(packet->m_data, packet->m_size);
				bool generateResult = false;
				JrpcProto::JsonRpcRequest::Ptr request = JrpcProto::JsonRpcRequestFactory::createFromJsonStr(requestJsonStr, &generateResult);
				if (!generateResult) {
					throw RpcException(JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::ParseError),
						JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::ParseError));
				}
				else {
					if (request->isReturnCall()) {
						handleProcedureReturn(sender, request);
					}
					else if (request->isNotification()) {
						handleProcedureNotify(sender, request);
					}
				}
			}
			catch (RpcException& e) {
				auto errorResponse = JrpcProto::JsonRpcErrorFactory::createFromInt(e.err());
				handleJsonRpcErrorException(sender, errorResponse);
			}
		}

		void RpcServer::handleProcedureReturn(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request)
		{
			Nano::Concurrency::StealThreadPool::GetInstance()->submit([this, sender, request]() {
				std::string method = request->getMethod();
				Json::Value reqJson = request->toJson();
				this->m_rpcService->callProcedureReturn(method, reqJson,
					[this, sender](Json::Value response) {
						try {
							std::string responseStr = response.toStyledString();
							char* buffer = nullptr;
							int len = 0;
							if (Nano::Communication::encode(responseStr, &buffer, &len))
							{
								sender->send(buffer, len);
								delete[] buffer;
								buffer = nullptr;
							}
							else
								throw RpcException(JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::ParseError),
									JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::ParseError));
						}
						catch (RpcException& e) {
							auto errorResponse = JrpcProto::JsonRpcErrorFactory::createFromInt(e.err());
							handleJsonRpcErrorException(sender, errorResponse);
						}
					});
				});
		}

		void RpcServer::handleProcedureNotify(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcRequest::Ptr request)
		{
			auto stealthreadPool = Nano::Concurrency::StealThreadPool::GetInstance();
			stealthreadPool->submit([this, sender, request]() mutable {
				std::string method = request->getMethod();
				Json::Value reqJson = request->toJson();
				this->m_rpcService->callProcedureNotify(method, reqJson);
				});
		}

		void RpcServer::handleJsonRpcErrorException(std::shared_ptr<Communication::Session> sender, JrpcProto::JsonRpcError::Ptr error)
		{
			std::string responseStr = error->getErrorMessage();
			char* buffer = nullptr;
			int len = 0;
			if (Nano::Communication::encode(responseStr, &buffer, &len))
			{
				sender->send(buffer, len);
				delete[] buffer;
				buffer = nullptr;
			}
			else
			{
				//std::cerr << "encode error" << std::endl;
			}
		}
	}
}