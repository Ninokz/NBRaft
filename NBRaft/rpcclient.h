#pragma once
#include <memory>
#include <string>
#include <unordered_map>
#include <ctime>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "ceventhandler.h"
#include "session.h"
#include "packet.h"

#include "rpcservice.h"
#include "rpcprocedure.h"
#include "baseclient.h"

#include "stealThreadPool.h"
#include "functionWrapper.h"

#include "jrpcproto.h"
#include "rpcexception.h"
#include "nocopyable.h"

namespace Nano {
	namespace Rpc {
		class CallRecord : public Noncopyable
		{
		public:
			typedef std::shared_ptr<CallRecord> Ptr;
		public:
			CallRecord() : request(nullptr), response(nullptr), timestamp(std::time(nullptr)) {}
			CallRecord(JrpcProto::JsonRpcRequest::Ptr request) : request(request), response(nullptr), timestamp(std::time(nullptr)) {}
			~CallRecord() = default;

			bool isDone() {
				return response != nullptr;
			}

			bool isError() {
				return request == nullptr || response == nullptr;
			}
		public:
			time_t timestamp;
			JrpcProto::JsonRpcRequest::Ptr request;
			JrpcProto::JsonRpcResponse::Ptr response;
		};

		class RpcClient : public Communication::BaseClient, public Communication::IDataReadyEventHandler,
			public std::enable_shared_from_this<RpcClient>
		{
			friend class RpcClientFactory;
		public:
			typedef std::shared_ptr<RpcClient> Ptr;
			typedef std::unordered_map<std::string, std::pair<CallRecord::Ptr, ProcedureDoneCallback>> CallRecordMap;
		protected:
			RpcClient();
			void Init();
		public:
			virtual ~RpcClient();
			bool callReturnProcedure(JrpcProto::JsonRpcRequest::Ptr request, const ProcedureDoneCallback callback);
			bool callNotifyProcedure(JrpcProto::JsonRpcRequest::Ptr request);

			CallRecord::Ptr getReturnCallRecord(const std::string& id);
			void clearCallRecords();
			void removeCallRecord(const std::string& id);
		private:
			void onDataReady(std::shared_ptr<Communication::Session> sender, std::shared_ptr<Communication::RecvPacket> packet) override;
		private:
			std::mutex m_mutex;
			CallRecordMap m_callRecords;
		};

		class RpcClientFactory
		{
		public:
			static RpcClient::Ptr create() {
				auto ptr = std::shared_ptr<RpcClient>(new RpcClient());
				ptr->Init();
				return ptr;
			}
		};
	}
}