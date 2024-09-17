#include "raftservice.h"

namespace Nano {
	namespace Raft {
		void RaftService::requestVote(int term, int candidateId, 
			int lastLogIndex, int lastLogTerm, const UserDoneCallback& done)
		{
			Raft::RequestVoteArgs args;
			args.term = term;
			args.candidateId = candidateId;
			args.lastLogIndex = lastLogIndex;
			args.lastLogTerm = lastLogTerm;

			m_doRequestVoteCallback(args, [done](const Raft::RequestVoteReply& reply) {
				Json::Value val;
				val["term"] = reply.term;
				val["voteGranted"] = reply.voteGranted;
				done(std::move(val));
			});
		}

		void RaftService::appendEntries(int term, int prevLogIndex, int prevLogTerm, 
			Json::Value entries, int leaderCommit, const UserDoneCallback& done)
		{
			Raft::AppendEntriesArgs args;
			args.term = term;
			args.prevLogIndex = prevLogIndex;
			args.prevLogTerm = prevLogTerm;
			args.entries = std::move(entries);
			args.leaderCommit = leaderCommit;

			m_doAppendEntriesCallback(args, [done](const Raft::AppendEntriesReply& reply) {
				Json::Value val;
				val["term"] = reply.term;
				val["success"] = reply.success;
				val["expectIndex"] = reply.expectIndex;
				val["expectTerm"] = reply.expectTerm;
				done(std::move(val));
			});
		}
	}
}