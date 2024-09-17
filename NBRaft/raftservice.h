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

			typedef std::function<void(Json::Value response)> ProcedureDoneCallback;				// Raft 过程执行完成后的回调函数
			typedef std::function<void(Json::Value&, const ProcedureDoneCallback&)> ReturnFunc;		// Raft 处理完成需要回调响应的函数
			typedef std::function<void(Json::Value&)> NotifyFunc;									// Raft 处理完成不需要回调响应的函数
		public:



		private:
			
		};
	}
}