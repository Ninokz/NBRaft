#pragma once
#include <memory>
#include <string>
#include <functional>
#include <json/json.h>

#include "nocopyable.h"
#include "rpcserverstub.h"

#include "raft.h"

namespace Nano {
	namespace Raft {
        typedef std::function<void(Json::Value response)> UserDoneCallback;

		class RaftService
		{
		public:
			typedef std::shared_ptr<RaftService> Ptr;
		public:
            RaftService();
			~RaftService();

            void SetDoRequestVoteCallback(const Raft::DoRequestVoteCallback& cb)
            {
                m_doRequestVoteCallback = cb;
            }

            void SetDoAppendEntriesCallback(const Raft::DoAppendEntriesCallback& cb)
            {
                m_doAppendEntriesCallback = cb;
            }

            void RequestVote(int term,
                int candidateId,
                int lastLogIndex,
                int lastLogTerm,
                const UserDoneCallback& done);

            void AppendEntries(int term,
                int prevLogIndex,
                int prevLogTerm,
                Json::Value entries,
                int leaderCommit,
                const UserDoneCallback& done);
		protected:
			Raft::DoRequestVoteCallback m_doRequestVoteCallback;
			Raft::DoAppendEntriesCallback m_doAppendEntriesCallback;
		};
	}
}