#pragma once
#include "BehaviorTree.h"
#include "raftrepeattimer.h"

class RaftNode : public Nano::BehaviorTree::Action {
public:
	RaftNode(const std::string& uid) : Nano::BehaviorTree::Action(uid) {}
	virtual Nano::BehaviorTree::Behavior::EStatus onUpdate() = 0;
};

class CheckElectionTimeout : public RaftNode {
public:
	CheckElectionTimeout() : RaftNode("CheckElectionTimeout") {}
	Nano::BehaviorTree::Behavior::EStatus onUpdate() override {
		// Implementation to check if election timeout has occurred
		// Return Success if timeout occurred, Failure otherwise
		std::cout << "CheckElectionTimeout" << std::endl;
		return Nano::BehaviorTree::Behavior::EStatus::Success;
	}
};

class StartElection : public RaftNode {
public:
	StartElection() : RaftNode("StartElection") {}
	Nano::BehaviorTree::Behavior::EStatus onUpdate() override {
		// Implementation to start an election
		// Return Success if election started, Failure otherwise
		std::cout << "StartElection" << std::endl;
		return Nano::BehaviorTree::Behavior::EStatus::Success;
	}
};

class SendHeartbeat : public RaftNode {
public:
	SendHeartbeat() : RaftNode("SendHeartbeat") {}
	Nano::BehaviorTree::Behavior::EStatus onUpdate() override {
		// Implementation to send heartbeat
		// Return Success if heartbeat sent, Failure otherwise
		std::cout << "SendHeartbeat" << std::endl;
		return Nano::BehaviorTree::Behavior::EStatus::Success;
	}
};

class CommitEntries : public RaftNode {
public:
	CommitEntries() : RaftNode("CommitEntries") {}
	Nano::BehaviorTree::Behavior::EStatus onUpdate() override {
		// Implementation to commit log entries
		// Return Success if entries committed, Failure otherwise
		std::cout << "CommitEntries" << std::endl;
		return Nano::BehaviorTree::Behavior::EStatus::Success;
	}
};

class RaftProtocol {
public:
	RaftProtocol() : m_timer(1, std::bind(&RaftProtocol::onTimer, this)) {
		buildBehaviorTree();
	}

	void start() {
		m_timer.start();
	}

	void stop() {
		m_timer.stop();
	}

private:
	void buildBehaviorTree() {
		auto builder = std::make_shared<Nano::BehaviorTree::BehaviorTreeBuilder>();

		m_behaviorTree = builder->Selector("RaftRoot")
			->Sequence("FollowerSequence")
			->Action(std::make_shared<CheckElectionTimeout>())
			->Action(std::make_shared<StartElection>())
			->Back()
			->Sequence("LeaderSequence")
			->Action(std::make_shared<SendHeartbeat>())
			->Action(std::make_shared<CommitEntries>())
			->Back()
			->End();
	}

	void onTimer() {
		m_behaviorTree->tick();
	}

	Nano::BehaviorTree::BehaviorTree::Ptr m_behaviorTree;
	Nano::Raft::RepeatingTimer<std::function<void()>> m_timer;
};