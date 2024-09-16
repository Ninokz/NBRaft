#include "raftservice.h"

namespace Nano {
	namespace Raft {
		RaftService::RaftService()
		{
		}

		RaftService::~RaftService()
		{
		}

		void RaftService::RequestVote(
			int term, 
			int candidateId, 
			int lastLogIndex, 
			int lastLogTerm, 
			const UserDoneCallback& done)
		{
			Raft::RequestVoteArgs args;
			args.term = term;
			args.candidateId = candidateId;
			args.lastLogIndex = lastLogIndex;
			args.lastLogTerm = lastLogTerm;

			//m_doRequestVoteCallback(args, [done](const Raft::RequestVoteReply& reply) {
			//	Json::Value response;
			//	response["term"] = reply.term;
			//	response["voteGranted"] = reply.voteGranted;
			//	done(response);
			//});
			m_doRequestVoteCallback(args, [=](const Raft::RequestVoteReply& reply) {
				Json::Value response;
				response["term"] = reply.term;
				response["voteGranted"] = reply.voteGranted;
				done(std::move(response));
			});
		}

		void RaftService::AppendEntries(
			int term,
			int prevLogIndex, 
			int prevLogTerm, 
			Json::Value entries, 
			int leaderCommit, 
			const UserDoneCallback& done)
		{
			Raft::AppendEntriesArgs args;
			args.term = term;
			args.prevLogIndex = prevLogIndex;
			args.prevLogTerm = prevLogTerm;
			args.entries = std::move(entries);
			args.leaderCommit = leaderCommit;

			//m_doAppendEntriesCallback(args, [done](const Raft::AppendEntriesReply& reply) {
			//	Json::Value response;
			//	response["term"] = reply.term;
			//	response["success"] = reply.success;
			//	response["expectIndex"] = reply.expectIndex;
			//	response["expectTerm"] = reply.expectTerm;
			//	done(response);
			//});
			m_doAppendEntriesCallback(args, [=](const Raft::AppendEntriesReply& reply) {
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