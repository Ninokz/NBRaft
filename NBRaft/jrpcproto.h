///
/// JSON-RPC 2.0 Specification
///	https://www.jsonrpc.org/specification
///

#pragma once
#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include <unordered_map>

#include <json/json.h>

namespace Nano {
	namespace JrpcProto {
		class JsonRpcRequest {
		public:
			typedef std::shared_ptr<JsonRpcRequest> Ptr;
			using ParameterName = std::string;
			using ParameterValue = Json::Value;
		private:
			JsonRpcRequest() = delete;
		public:
			JsonRpcRequest(const Json::Value rpcRequest);

			JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, Json::Value parametersNameWithValue, std::string requestId);

			JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, Json::Value parametersNameWithValue);

			JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, std::unordered_map<ParameterName, ParameterValue> kv, std::string requestId);

			JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, std::unordered_map<ParameterName, ParameterValue> kv);

			std::string toJsonStr() const;

			Json::Value toJson() const;

			std::string getMethod() const;

			std::string getId() const;

			Json::Value getParams() const;

			std::string getVersion() const;

			Json::Value getParam(const std::string& key) const;

			bool isNotification() const;

			bool isReturnCall() const;
		private:
			Json::Value m_rpcRequest;
		};

		class JsonRpcRequestFactory {
		public:
			static JsonRpcRequest::Ptr createReturnCallRequest(const std::string& version, const std::string& method, const std::string id, std::unordered_map<std::string, Json::Value> params);

			static JsonRpcRequest::Ptr createNotifyCallRequest(const std::string& version, const std::string& method, std::unordered_map<std::string, Json::Value> params);

			static JsonRpcRequest::Ptr createFromJsonStr(const std::string& jsonStr, bool* flag);

			static inline JsonRpcRequest::Ptr createEmptyRequest();

			static inline JsonRpcRequest::Ptr createFromJson(const Json::Value& json, bool* flag);

			template <typename... Args>
			static JsonRpcRequest::Ptr createReturnCallRequest(const std::string& version, const std::string& method, const std::string id, const Args&... args) {
				Json::Value params(Json::objectValue);
				addParams(params, args...);
				JsonRpcRequest::Ptr request = std::make_shared<JsonRpcRequest>(version, method, params, id);
				return request;
			}

			template <typename... Args>
			static JsonRpcRequest::Ptr createNotifyCallRequest(const std::string& version, std::string method, const Args&... args) {
				Json::Value params(Json::objectValue);
				addParams(params, args...);
				JsonRpcRequest::Ptr request = std::make_shared<JsonRpcRequest>(version, method, params, "");
				return request;
			}

			static inline bool fieldsExist(const Json::Value& rpcRequestJson);
		private:		
			static void addParams(Json::Value& params) {}
			template <typename Key, typename Value, typename... Args>
			static void addParams(Json::Value& params, const Key& key, const Value& value, const Args&... args) {
				params[key] = value;
				addParams(params, args...);
			}
		};

		class JsonRpcError {
		public:
			enum class JsonRpcErrorCode {
				ParseError = -32700,
				InvalidRequest = -32600,
				MethodNotFound = -32601,
				InvalidParams = -32602,
				InternalError = -32603
			};
			typedef std::shared_ptr<JsonRpcError> Ptr;
		public:
			JsonRpcError(JsonRpcErrorCode errorCode);

			Json::Value toJson() const;

			std::string	toJsonStr() const;

			int getErrorCode() const;

			std::string getErrorMessage() const;

			static std::string getErrorMessage(JsonRpcErrorCode code);

			static int toInt(JsonRpcErrorCode code);
		private:
			Json::Value m_rpcError;
		};

		class JsonRpcErrorFactory {
		public:
			static JsonRpcError::Ptr createFromErrorCodeEnum(JsonRpcError::JsonRpcErrorCode code);
			static JsonRpcError::Ptr createFromInt(int code);
			static inline JsonRpcError::Ptr createEmptyError();
		};

		class JsonRpcResponse {
		public:
			typedef std::shared_ptr<JsonRpcResponse> Ptr;
		private:
			JsonRpcResponse() = delete;
		public:
			JsonRpcResponse(std::string jsonrpcVersion, std::string requestId, const Json::Value result);
			JsonRpcResponse(std::string jsonrpcVersion, const JsonRpcError& error);
			JsonRpcResponse(const Json::Value rpcResponse);

			Json::Value toJson() const;

			std::string toJsonStr() const;

			std::string getId() const;

			Json::Value getResult() const;

			JsonRpcError::Ptr getError() const;

			bool isError() const;
		private:
			Json::Value m_rpcResponse;
		};

		class JsonRpcResponseFactory
		{
		public:
			static JsonRpcResponse::Ptr createResponseFromJsonStr(const std::string& jsonStr, bool* flag);
			static JsonRpcResponse::Ptr createErrorResponse(const std::string& version, const JsonRpcError& error);
			static JsonRpcResponse::Ptr createResponseFromRequest(const Json::Value& request, const Json::Value result, bool* flag);
			static inline JsonRpcResponse::Ptr createResponseFromJson(const Json::Value& rpcResponseJson, bool* flag);
			static inline JsonRpcResponse::Ptr createEmptyResponse();
			static inline bool fieldsExist(const Json::Value& rpcresponseJson);
		};
	}
}