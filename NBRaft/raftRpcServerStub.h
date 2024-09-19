#pragma once
#include "rpcserverstub.h"

namespace Nano {
	namespace Raft {
		class RaftRpcServerStub : protected Rpc::RpcServerStub
		{
		public:
			typedef std::shared_ptr<RaftRpcServerStub> Ptr;
		public:


		private:

		};
	}
}