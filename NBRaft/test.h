#pragma once
#include "BehaviorTree.h"
#include "raftrepeattimer.h"
#include "raftrpcserverstub.h"

void servertest()
{
	std::shared_ptr<Nano::Raft::RaftService> raftService = std::make_shared<Nano::Raft::RaftService>();
	raftService->setRequestVoteCallback([](const Nano::Raft::RequestVoteArgs& args, const Nano::Raft::RequestVoteDoneCallback& done) {
		Nano::Raft::RequestVoteReply reply;
		reply.term = 1;
		reply.voteGranted = true;
		std::cout << "requestVote" << std::endl;
		done(reply);
	});
	raftService->setAppendEntriesCallback([](const Nano::Raft::AppendEntriesArgs& args, const Nano::Raft::AppendEntriesDoneCallback& done) {
		Nano::Raft::AppendEntriesReply reply;
		reply.term = 1;
		reply.success = true;
		std::cout << "appendEntries" << std::endl;
		done(reply);
	});

	Nano::Raft::RaftRpcServerStub server(12345, raftService);
	server.run();
}