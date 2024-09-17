#pragma once
#include <memory>
#include <string>
#include <functional>
#include <json/json.h>

#include "nocopyable.h"
#include "rpcserverstub.h"
#include "rpcprocedure.h"

#include "struct.h"

namespace Nano {
	namespace Raft {
		//// RaftService:
		/// 封装 Raft 参数适配 RpcServerStub registReturn 和 registNotify 方法的参数
		class RaftService : public Noncopyable
		{
		public:
			typedef std::shared_ptr<RaftService> Ptr;
			typedef std::function<void(Json::Value val)> UserDoneCallback;
		public:
			void requestVote(int term,int candidateId,int lastLogIndex,int lastLogTerm,const UserDoneCallback& done);
			void appendEntries(int term, int prevLogIndex, int prevLogTerm, Json::Value entries, int leaderCommit, const UserDoneCallback& done);
		private:
			Raft::DoRequestVoteCallback m_doRequestVoteCallback;
			Raft::DoAppendEntriesCallback m_doAppendEntriesCallback;
		};
	}
}