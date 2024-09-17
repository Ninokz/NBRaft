#pragma once
#include "rpcserverstub.h"
#include "raftservice.h"

namespace Nano {
	namespace Raft {
        class RaftRpcServerStub : public Rpc::RpcServerStub {
        public:
            RaftRpcServerStub(short port, std::shared_ptr<Nano::Raft::RaftService> raftService);
            ~RaftRpcServerStub() = default;
        private:
            std::shared_ptr<Nano::Raft::RaftService> m_raftService;
        };
	}
}