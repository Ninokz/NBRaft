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
		/// rpcservice �������� struct.h ��Raft��ؽṹ�嵽 rpcserverstub

		class RaftService : public Noncopyable
		{
		public:
			typedef std::shared_ptr<RaftService> Ptr;
		public:

		};
	}
}