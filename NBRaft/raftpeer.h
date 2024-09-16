#pragma once
#include <memory>
#include <string>
#include "nocopyable.h"
#include "rpcclientstub.h"

#include "raft.h"

namespace Nano {
	namespace Raft {
		class RaftPeer: public Noncopyable
		{
		public:
			typedef std::shared_ptr<RaftPeer> Ptr;
			typedef std::unique_ptr<Nano::Rpc::RpcClientStub> RpcClientStubUniPtr;
		public:
		};
	}
}
