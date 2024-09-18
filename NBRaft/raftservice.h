#pragma once
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "struct.h"
#include "nocopyable.h"
#include "rpcservice.h"

namespace Nano {
	namespace Raft {
		/// rpcservice 用于适配 struct.h 中Raft相关结构体到 rpcserverstub

		class RaftService 
		{
		public:
			typedef std::shared_ptr<RaftService> Ptr;
			/// 以下是参数被 Json 化后的回调函数
			using RaftProcedureDoneCallback = Rpc::ProcedureDoneCallback;			// Raft 过程执行完成后的回调函数 - 参数 Json化 - 不用关注，此方法由 RpcServer 内部在执行完成后用于发送响应

			using RequestVoteProcedureFunc = Rpc::ReturnFunc;						// 请求投票过程: Raft 处理完成需要回调响应的函数 - 参数 Json化
			using AppendEntriesProcedureFunc = Rpc::ReturnFunc;						// 添加日志过程: Raft 处理完成不需要回调响应的函数 - 参数 Json化

			/// 消息传递过程
			// Json::value -> 传入到 ReturnFunc 中处理，然后回调 ReturnFunc中 ProcedureDoneCallback 回送响应


		public:


		
		};
	}
}