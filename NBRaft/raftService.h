#pragma once
#include "rpcprocedure.h"
#include "nocopyable.h"

#include "struct.h"

namespace Nano {
	namespace Raft {
		class RaftService : public Nano::Noncopyable
		{
		public:
			typedef std::shared_ptr<RaftService> Ptr;
			typedef std::function<void(Json::Value response)> UserDoneCallback;
		public:
			void requestVote(int term,
				int candidateId,
				int lastLogIndex,
				int lastLogTerm,
				const UserDoneCallback& done);
			void appendEntries(int term,
				int prevLogIndex,
				int prevLogTerm,
				Json::Value entries,
				int leaderCommit,
				const UserDoneCallback& done);

			void setRequestVoteCallback(const DoRequestVoteCallback& callback) {
				m_requestVoteCallback = callback;
			}

			void setAppendEntriesCallback(const DoAppendEntriesCallback& callback) {
				m_appendEntriesCallback = callback;
			}
		private:
			DoRequestVoteCallback m_requestVoteCallback;
			DoAppendEntriesCallback m_appendEntriesCallback;
		};
	}
}

