#pragma once

#include "rpcclientstub.h"

namespace Nano {
	namespace Raft {
		class RaftCallRecord : public Rpc::CallRecord
		{
		public:
			typedef std::shared_ptr<RaftCallRecord> Ptr;
		};
		class RaftRpcClientStub : protected Rpc::RpcClientStub
		{
		public:
			typedef std::shared_ptr<RaftRpcClientStub> Ptr;
		public:

		private:
		};
	}
}