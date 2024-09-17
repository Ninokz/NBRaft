#pragma once
#include <memory>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>

#include "struct.h"
#include "rpcclientstub.h"


namespace Nano {
	namespace Raft {
        class RaftRpcClientStub : public Rpc::RpcClientStub
        {
        public:
            typedef std::shared_ptr<RaftRpcClientStub> Ptr;
        public:
            RaftRpcClientStub(int timeout);
            virtual ~RaftRpcClientStub();

            void RequestVote(int term, int candidateId, int lastLogIndex, int lastLogTerm,const std::function<void(Json::Value, bool, bool)>& callback);

            std::future<Rpc::CallRecord::Ptr> AsyncRequestVote(int term, int candidateId, int lastLogIndex, int lastLogTerm, const std::function<void(Json::Value, bool, bool)>& callback);

            void AppendEntries(int term, int prevLogIndex, int prevLogTerm,const Json::Value& entries, int leaderCommit,const std::function<void(Json::Value, bool, bool)>& callback);

            std::future<Rpc::CallRecord::Ptr> AsyncAppendEntries(int term, int prevLogIndex, int prevLogTerm,const Json::Value& entries, int leaderCommit, const std::function<void(Json::Value, bool, bool)>& callback);
        private:
			int m_timeout;
        };
	}
}