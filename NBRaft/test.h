#pragma once
#include <unordered_map>

#include "rpcclientstub.h"
#include "rpcserverstub.h"
#include "rpcprocedure.h"

#include <json/json.h>

/// hello world test case once

void helloworldReturnService(Json::Value& request, const Nano::Rpc::ProcedureDoneCallback& done) {
	Json::Value result = "Hello, " + request["params"]["name"].asString() + "!";
	std::cout << "helloworldReturnService: " << result << std::endl;
	bool flag = false;
	Nano::JrpcProto::JsonRpcResponse::Ptr response = Nano::JrpcProto::JsonRpcResponseFactory::createResponseFromRequest(request, result, &flag);
	done(response->toJson());
}

void RpcServerStubHelloWorldTest() {
	Nano::Rpc::RpcServerStub::Ptr rpcServerStub = std::make_shared<Nano::Rpc::RpcServerStub>(9800);
	std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap = {
	  {"name", Json::ValueType::stringValue}
	};
	rpcServerStub->registReturn("helloworldMethod", paramsNameTypesMap, helloworldReturnService);
	rpcServerStub->run();
	system("pause");
	rpcServerStub->stop();
}

void helloworldCallback(Json::Value response) {
	std::cout << response.toStyledString() << std::endl;
};

void ClientStubHelloWorldTest() {
	std::unordered_map<std::string, Json::Value> params = {
	  {"name", "World"}
	};
	auto result = Nano::Rpc::RpcClientOnceStub::rpcReturnCallOnce("127.0.0.1", 9800, "1", "helloworldMethod", params, helloworldCallback, 3000);
	std::cout << result->response->getResult().asString() << std::endl;
}

//// Substract Test Case once

void substractReturnService(Json::Value& request, const Nano::Rpc::ProcedureDoneCallback& done) {
	int subtrahend = request["params"]["subtrahend"].asInt();
	int minuend = request["params"]["minuend"].asInt();
	Json::Value result = minuend - subtrahend;

	bool flag = false;
	Nano::JrpcProto::JsonRpcResponse::Ptr response = Nano::JrpcProto::JsonRpcResponseFactory::createResponseFromRequest(request, result, &flag);
	done(response->toJson());
}

void RpcServerStubSubstractTest() {
	Nano::Rpc::RpcServerStub::Ptr rpcServerStub = std::make_shared<Nano::Rpc::RpcServerStub>(9800);
	std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap = {
	  {"subtrahend", Json::ValueType::intValue},
	  {"minuend", Json::ValueType::intValue}
	};

	rpcServerStub->registReturn("substractMethod", paramsNameTypesMap, substractReturnService);
	rpcServerStub->run();
	system("pause");
	rpcServerStub->stop();
}

void substractCallback(Json::Value response) {
	std::cout << response.toStyledString() << std::endl;
};

void ClientStubSubstractTest() {
	std::unordered_map<std::string, Json::Value> params = {
	  {"subtrahend", 23},
	  {"minuend", 42}
	};
	auto result = Nano::Rpc::RpcClientOnceStub::rpcReturnCallOnce("127.0.0.1", 9800, "1", "substractMethod", params, substractCallback, 3000);
	std::cout << result->response->getResult().asString() << std::endl;
}

/// hello world notify test case once

void ClientStubhelloNotifyTest() {
	Nano::Rpc::RpcClientStub::Ptr rpcClientStub = std::make_shared<Nano::Rpc::RpcClientStub>();
	std::unordered_map<std::string, Json::Value> params = {
	  {"notify", "World"}
	};
	auto result = Nano::Rpc::RpcClientOnceStub::rpcNotifyCallOnce("127.0.0.1", 9800, "helloworldMethod", params);
	std::cout << "notify success :" << result << std::endl;
}

void helloNotifyService(Json::Value& request)
{
}

void RpcServerStubhelloNotifyTest() {
	Nano::Rpc::RpcServerStub::Ptr rpcServerStub = std::make_shared<Nano::Rpc::RpcServerStub>(9800);
	std::unordered_map<std::string, Json::ValueType> paramsNameTypesMap = {
	  {"notify", Json::ValueType::stringValue}
	};
	rpcServerStub->registNotify("helloNotifyMethod", paramsNameTypesMap, helloNotifyService);
	rpcServerStub->run();
	system("pause");
	rpcServerStub->stop();
}
