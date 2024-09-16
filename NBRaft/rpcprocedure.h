#pragma once
#include <functional>
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

#include <json/json.h>

#include "rpcexception.h"
#include "jrpcproto.h"
#include "nocopyable.h"

namespace Nano {
	namespace Rpc {
		typedef std::function<void(Json::Value response)> ProcedureDoneCallback;				// ����ִ����ɺ�Ļص�����
		typedef std::function<void(Json::Value&, const ProcedureDoneCallback&)> ReturnFunc;		// ���������Ҫ�ص���Ӧ�ĺ���
		typedef std::function<void(Json::Value&)> NotifyFunc;									// ������ɲ���Ҫ�ص���Ӧ�ĺ���

		template <typename Func>
		class RpcProcedure : public Noncopyable
		{
		public:
			typedef std::shared_ptr<RpcProcedure> Ptr;
		public:
			template<typename... ParamNameAndTypes>
			explicit RpcProcedure(Func&& callback, ParamNameAndTypes&&... nameAndTypes) :
				m_callback(std::forward<Func>(callback))
			{
				constexpr int n = sizeof...(nameAndTypes);
				static_assert(n % 2 == 0, "procedure must have param name and type pairs");
				initProcedure(std::forward<ParamNameAndTypes>(nameAndTypes)...);
			}

			explicit RpcProcedure(Func&& callback, std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap) :
				m_callback(std::forward<Func>(callback)),
				m_standardParams(std::move(paramsNameTypesMap))
			{
			}

			void invoke(Json::Value& request, const ProcedureDoneCallback& done) {
				validateRequest(request);
				m_callback(request, done);
			}

			void invoke(Json::Value& request) {
				validateRequest(request);
				m_callback(request);
			}
		private:
			void initProcedure() {}

			template<typename Name, typename Type, typename... Rest>
			void initProcedure(Name&& paramName, Type&& paramType, Rest&&... rest)
			{
				static_assert(std::is_same_v<std::decay_t<Type>, Json::ValueType>,
					"param type must be Json::ValueType");
				m_standardParams.insert(std::make_pair(std::forward<Name>(paramName),
					std::forward<Type>(paramType)));
				initProcedure(std::forward<Rest>(rest)...);
			}

			inline void validateRequest(Json::Value& request) const;
			inline bool validateGeneric(Json::Value& request) const;
		private:
			Func m_callback;
			std::unordered_map<std::string, Json::ValueType> m_standardParams;	// �����б�
		};

		template <>
		inline void RpcProcedure<ReturnFunc>::validateRequest(Json::Value& request) const {
			if (!validateGeneric(request))
				throw RpcException(JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::InvalidParams),
					JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::InvalidParams));
		}

		template <>
		inline void RpcProcedure<NotifyFunc>::validateRequest(Json::Value& request) const
		{
			if (!validateGeneric(request))
				throw RpcException(JrpcProto::JsonRpcError::toInt(JrpcProto::JsonRpcError::JsonRpcErrorCode::InvalidParams),
					JrpcProto::JsonRpcError::getErrorMessage(JrpcProto::JsonRpcError::JsonRpcErrorCode::InvalidParams));
		}

		template <typename Func>
		bool RpcProcedure<Func>::validateGeneric(Json::Value& request) const {
			if (!request.isMember("params")) {
				return false;
			}
			else
			{
				Json::Value params = request["params"];
				for (Json::Value::const_iterator it = params.begin(); it != params.end(); ++it) {
					std::string key = it.key().asString();
					Json::Value value = *it;
					if (m_standardParams.find(key) == m_standardParams.end()) {
						return false;
					}
					auto standradItem = m_standardParams.find(key);
					if (standradItem->second != value.type()) {
						return false;
					}
				}
				return true;
			}
		}
		typedef RpcProcedure<ReturnFunc> ProcedureReturn;
		typedef RpcProcedure<NotifyFunc> ProcedureNotify;
	}
}