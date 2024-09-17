#include "raftService.h"

namespace Nano {
	namespace Raft {
		void RaftService::requestVote(int term, int candidateId,
			int lastLogIndex, int lastLogTerm, 
			const UserDoneCallback& done)
		{
			Raft::RequestVoteArgs args;
			args.term = term;
			args.candidateId = candidateId;
			args.lastLogIndex = lastLogIndex;
			args.lastLogTerm = lastLogTerm;

			m_requestVoteCallback(args, [=](const RequestVoteReply& reply) {
				Json::Value response;
				response["term"] = reply.term;
				response["voteGranted"] = reply.voteGranted;
				done(std::move(response));
			});
		}

		void RaftService::appendEntries(int term, int prevLogIndex, 
			int prevLogTerm, Json::Value entries, 
			int leaderCommit, const UserDoneCallback& done)
		{
			Raft::AppendEntriesArgs args;
			args.term = term;
			args.prevLogIndex = prevLogIndex;
			args.prevLogTerm = prevLogTerm;
			args.entries = entries;
			args.leaderCommit = leaderCommit;

			m_appendEntriesCallback(args, [=](const AppendEntriesReply& reply) {
				Json::Value response;
				response["term"] = reply.term;
				response["success"] = reply.success;
				response["expectIndex"] = reply.expectIndex;
				response["expectTerm"] = reply.expectTerm;
				done(std::move(response));
			});
		}
	}
}