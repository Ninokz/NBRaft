#include "jrpcproto.h"

namespace Nano {
	namespace JrpcProto {
		JsonRpcRequest::JsonRpcRequest(const Json::Value rpcRequest)
		{
			if(JsonRpcRequestFactory::fieldsExist(rpcRequest))
				m_rpcRequest = rpcRequest;
			else
				m_rpcRequest = Json::Value();
		}

		JsonRpcRequest::JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, Json::Value parameters, std::string requestId)
		{
			/// JSON-RPC 2.0: A Request object that is a method call is identified by the presence of an "id" member
			m_rpcRequest["jsonrpc"] = jsonrpcVersion;
			m_rpcRequest["method"] = methodName;
			m_rpcRequest["params"] = parameters;
			m_rpcRequest["id"] = requestId;
		}

		JsonRpcRequest::JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, Json::Value parametersNameWithValue)
		{
			/// JSON-RPC 2.0: A Notification is a Request object without an "id" member
			m_rpcRequest["jsonrpc"] = jsonrpcVersion;
			m_rpcRequest["method"] = methodName;
			m_rpcRequest["params"] = parametersNameWithValue;
		}

		JsonRpcRequest::JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, std::unordered_map<ParameterName, ParameterValue> kv, std::string requestId)
		{
			/// JSON-RPC 2.0: A Request object that is a method call is identified by the presence of an "id" member
			m_rpcRequest["jsonrpc"] = jsonrpcVersion;
			m_rpcRequest["method"] = methodName;
			Json::Value params;
			for (const auto& item : kv) {
				params[item.first] = item.second;
			}
			m_rpcRequest["params"] = params;
			m_rpcRequest["id"] = requestId;
		}

		JsonRpcRequest::JsonRpcRequest(std::string jsonrpcVersion, std::string methodName, std::unordered_map<ParameterName, ParameterValue> kv)
		{
			/// JSON-RPC 2.0: A Notification is a Request object without an "id" member
			m_rpcRequest["jsonrpc"] = jsonrpcVersion;
			m_rpcRequest["method"] = methodName;
			Json::Value params;
			for (const auto& item : kv) {
				params[item.first] = item.second;
			}
			m_rpcRequest["params"] = params;
		}

		std::string JsonRpcRequest::toJsonStr() const
		{
			Json::StreamWriterBuilder writer;
			return Json::writeString(writer, m_rpcRequest);
		}

		Json::Value JsonRpcRequest::toJson() const
		{
			return m_rpcRequest;
		}

		std::string JsonRpcRequest::getMethod() const {
			if (m_rpcRequest.isMember("method")) {
				return m_rpcRequest["method"].asString();
			}
			return "";
		}

		std::string JsonRpcRequest::getId() const {
			if (m_rpcRequest.isMember("id")) {
				return m_rpcRequest["id"].asString();
			}
			return "";
		}

		Json::Value JsonRpcRequest::getParams() const {
			if (m_rpcRequest.isMember("params")) {
				return m_rpcRequest["params"];
			}
			return Json::Value();
		}

		std::string JsonRpcRequest::getVersion() const {
			if (m_rpcRequest.isMember("jsonrpc")) {
				return m_rpcRequest["jsonrpc"].asString();
			}
			return "";
		}

		Json::Value JsonRpcRequest::getParam(const std::string& key) const {
			if (m_rpcRequest.isMember("params")) {
				if (m_rpcRequest["params"].isObject()) {
					if (m_rpcRequest["params"].isMember(key)) {
						return m_rpcRequest["params"][key];
					}
				}
			}
			return Json::Value();
		}

		bool JsonRpcRequest::isNotification() const
		{
			/// JSON-RPC 2.0: A Notification is a Request object without an "id" member
			return !m_rpcRequest.isMember("id");
		}

		bool JsonRpcRequest::isReturnCall() const
		{
			/// JSON-RPC 2.0: A Request object that is a method call is identified by the presence of an "id" member
			return m_rpcRequest.isMember("id");
		}

		JsonRpcError::JsonRpcError(JsonRpcErrorCode errorCode)
		{
			m_rpcError["code"] = static_cast<int>(errorCode);
			m_rpcError["message"] = getErrorMessage(errorCode);
		}

		Json::Value JsonRpcError::toJson() const
		{
			return m_rpcError;
		}

		std::string JsonRpcError::toJsonStr() const
		{
			Json::StreamWriterBuilder writer;
			return Json::writeString(writer, m_rpcError);
		}

		int JsonRpcError::getErrorCode() const
		{
			if (m_rpcError.isMember("code"))
			{
				return m_rpcError["code"].asInt();
			}
			return -1;
		}

		std::string JsonRpcError::getErrorMessage() const
		{
			if (m_rpcError.isMember("message"))
			{
				return m_rpcError["message"].asString();
			}
			return "";
		}

		std::string JsonRpcError::getErrorMessage(JsonRpcErrorCode code)
		{
			switch (code) {
#define XX(name) case JsonRpcErrorCode::name: return #name;
				XX(ParseError)
					XX(InvalidRequest)
					XX(MethodNotFound)
					XX(InvalidParams)
					XX(InternalError)
#undef XX
			default:
				return "Unknown error";
			}
		}

		int JsonRpcError::toInt(JsonRpcErrorCode code)
		{
			return static_cast<int>(code);
		}

		JsonRpcResponse::JsonRpcResponse(std::string jsonrpcVersion, std::string requestId, const Json::Value result)
		{
			m_rpcResponse["jsonrpc"] = jsonrpcVersion;
			m_rpcResponse["result"] = result;
			m_rpcResponse["id"] = requestId;
		}

		JsonRpcResponse::JsonRpcResponse(std::string jsonrpcVersion, const JsonRpcError& error)
		{
			m_rpcResponse["jsonrpc"] = jsonrpcVersion;
			m_rpcResponse["error"] = error.toJson();
			m_rpcResponse["id"] = "";
		}

		JsonRpcResponse::JsonRpcResponse(const Json::Value rpcResponse)
		{
			if (JsonRpcResponseFactory::fieldsExist(rpcResponse))
				m_rpcResponse = rpcResponse;
			else
				m_rpcResponse = Json::Value();
		}

		Json::Value JsonRpcResponse::toJson() const
		{
			return m_rpcResponse;
		}

		std::string JsonRpcResponse::toJsonStr() const
		{
			Json::StreamWriterBuilder writer;
			return Json::writeString(writer, m_rpcResponse);
		}

		std::string JsonRpcResponse::getId() const
		{
			if (m_rpcResponse.isMember("id"))
			{
				return m_rpcResponse["id"].asString();
			}
			return "";
		}

		Json::Value JsonRpcResponse::getResult() const
		{
			if (m_rpcResponse.isMember("result"))
			{
				return m_rpcResponse["result"];
			}
			return Json::Value();
		}

		JsonRpcError::Ptr JsonRpcResponse::getError() const
		{
			if (m_rpcResponse.isMember("error"))
			{
				return std::make_shared<JsonRpcError>(JsonRpcError::JsonRpcErrorCode(m_rpcResponse["error"]["code"].asInt()));
			}
			return JsonRpcErrorFactory::createEmptyError();
		}

		bool JsonRpcResponse::isError() const
		{
			return m_rpcResponse.isMember("error");
		}

		JsonRpcRequest::Ptr JsonRpcRequestFactory::createReturnCallRequest(const std::string& version, const std::string& method, const std::string id, std::unordered_map<std::string, Json::Value> params)
		{
			Json::Value paramsJson(Json::objectValue);
			for (const auto& item : params) {
				paramsJson[item.first] = item.second;
			}
			JsonRpcRequest::Ptr request = std::make_shared<JsonRpcRequest>(version, method, paramsJson, id);
			return request;
		}

		JsonRpcRequest::Ptr JsonRpcRequestFactory::createNotifyCallRequest(const std::string& version, const std::string& method, std::unordered_map<std::string, Json::Value> params)
		{
			Json::Value paramsJson(Json::objectValue);
			for (const auto& item : params) {
				paramsJson[item.first] = item.second;
			}
			JsonRpcRequest::Ptr request = std::make_shared<JsonRpcRequest>(version, method, paramsJson);
			return request;
		}

		JsonRpcRequest::Ptr JsonRpcRequestFactory::createFromJsonStr(const std::string& jsonStr, bool* flag)
		{
			Json::CharReaderBuilder readerBuilder;
			Json::Value root;
			std::string errs;

			std::istringstream iss(jsonStr);
			if (!Json::parseFromStream(readerBuilder, iss, &root, &errs)) {
				*flag = false;
				return JsonRpcRequestFactory::createEmptyRequest();
			}
			else
			{
				if (!JsonRpcRequestFactory::fieldsExist(root))
				{
					*flag = false;
					return JsonRpcRequestFactory::createEmptyRequest();
				}
				*flag = true;
				return std::make_shared<JsonRpcRequest>(root);
			}
		}

		JsonRpcRequest::Ptr JsonRpcRequestFactory::createEmptyRequest()
		{
			return std::make_shared<JsonRpcRequest>(Json::Value());
		}

		JsonRpcRequest::Ptr JsonRpcRequestFactory::createFromJson(const Json::Value& json, bool* flag)
		{
			if (!JsonRpcRequestFactory::fieldsExist(json))
			{
				*flag = false;
				return JsonRpcRequestFactory::createEmptyRequest();
			}
			else
			{
				*flag = true;
				return std::make_shared<JsonRpcRequest>(json);
			}
		}

		inline bool JsonRpcRequestFactory::fieldsExist(const Json::Value& rpcRequestJson)
		{
			if (!rpcRequestJson.isMember("jsonrpc") || !rpcRequestJson["jsonrpc"].isString()) {
				//std::cerr << "Invalid or missing 'jsonrpc' field." << std::endl;
				return false;
			}
			if (!rpcRequestJson.isMember("method") || !rpcRequestJson["method"].isString()) {
				//std::cerr << "Invalid or missing 'method' field." << std::endl;
				return false;
			}
			if (!rpcRequestJson.isMember("params") || !rpcRequestJson["params"].isObject())
			{
				//std::cerr << "Invalid or missing 'params' field." << std::endl;
				return false;
			}
			return true;
		}

		JsonRpcResponse::Ptr JsonRpcResponseFactory::createResponseFromJsonStr(const std::string& jsonStr, bool* flag)
		{
			Json::CharReaderBuilder readerBuilder;
			Json::Value root;
			std::string errs;
			std::istringstream iss(jsonStr);
			if (!Json::parseFromStream(readerBuilder, iss, &root, &errs)) {
				*flag = false;
				return JsonRpcResponseFactory::createEmptyResponse();
			}
			if (!JsonRpcResponseFactory::fieldsExist(root))
			{
				*flag = false;
				return JsonRpcResponseFactory::createEmptyResponse();
			}
			else
			{
				*flag = true;
				return  std::make_shared<JsonRpcResponse>(root);
			}
		}

		JsonRpcResponse::Ptr JsonRpcResponseFactory::createResponseFromRequest(const Json::Value& request, const Json::Value result, bool* flag)
		{
			if (!JsonRpcRequestFactory::fieldsExist(request))
			{
				*flag = false;
				return JsonRpcResponseFactory::createEmptyResponse();
			}
			else
			{
				*flag = true;
				return std::make_shared<JsonRpcResponse>(request["jsonrpc"].asString(), request["id"].asString(), result);
			}
		}

		JsonRpcResponse::Ptr JsonRpcResponseFactory::createResponseFromJson(const Json::Value& rpcResponseJson, bool* flag)
		{
			if (!JsonRpcResponseFactory::fieldsExist(rpcResponseJson))
			{
				*flag = false;
				return JsonRpcResponseFactory::createEmptyResponse();
			}
			else
			{
				*flag = true;
				return std::make_shared<JsonRpcResponse>(rpcResponseJson);
			}
		}

		JsonRpcResponse::Ptr JsonRpcResponseFactory::createEmptyResponse()
		{
			return std::make_shared<JsonRpcResponse>(Json::Value());
		}

		JsonRpcResponse::Ptr JsonRpcResponseFactory::createErrorResponse(const std::string& version, const JsonRpcError& error)
		{
			return std::make_shared<JsonRpcResponse>(version, error);
		}

		inline bool JsonRpcResponseFactory::fieldsExist(const Json::Value& rpcresponseJson)
		{
			if (!rpcresponseJson.isMember("jsonrpc") || !rpcresponseJson.isMember("id"))
			{
				//std::cerr << "Invalid or missing 'jsonrpc' or 'id' field." << std::endl;
				return false;
			}
			if (!rpcresponseJson.isMember("error") && !rpcresponseJson.isMember("result"))
			{
				//std::cerr << "Invalid or missing 'error' or 'result' field." << std::endl;
				return false;
			}
			if (rpcresponseJson.isMember("error") && rpcresponseJson.isMember("result"))
			{
				//std::cerr << "Both 'error' and 'result' fields are present." << std::endl;
				return false;
			}
			return true;
		}

		JsonRpcError::Ptr JsonRpcErrorFactory::createFromErrorCodeEnum(JsonRpcError::JsonRpcErrorCode code)
		{
			return std::make_shared<JsonRpcError>(code);
		}

		JsonRpcError::Ptr JsonRpcErrorFactory::createFromInt(int code)
		{
			if (code == 0)
				return std::make_shared<JsonRpcError>(JsonRpcError::JsonRpcErrorCode::ParseError);
			else if (code == 1)
				return std::make_shared<JsonRpcError>(JsonRpcError::JsonRpcErrorCode::InvalidRequest);
			else if (code == 2)
				return std::make_shared<JsonRpcError>(JsonRpcError::JsonRpcErrorCode::MethodNotFound);
			else if (code == 3)
				return std::make_shared<JsonRpcError>(JsonRpcError::JsonRpcErrorCode::InvalidParams);
			else if (code == 4)
				return std::make_shared<JsonRpcError>(JsonRpcError::JsonRpcErrorCode::InternalError);
			else
				return std::make_shared<JsonRpcError>(JsonRpcError::JsonRpcErrorCode::InternalError);
		}

		inline JsonRpcError::Ptr JsonRpcErrorFactory::createEmptyError()
		{
			return std::make_shared<JsonRpcError>(JsonRpcError::JsonRpcErrorCode::InternalError);
		}
	}
}