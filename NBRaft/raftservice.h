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

		class RaftService : public Noncopyable
		{
		public:
			typedef std::shared_ptr<RaftService> Ptr;
		public:

		};
	}
}