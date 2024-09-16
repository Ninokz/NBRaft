#pragma once
#include <functional>

namespace Nano {
	namespace Raft {
		enum class ERaftState
		{
			Undefined,
			Follower,
			Candidate,
			Leader
		};

		struct RequestVoteArgs
		{
			int term = -1;			 // 候选人的任期
			int candidateId = -1;	 // 请求选票的候选人的 Id
			int lastLogIndex = -1;	 // 候选人的最后日志条目的索引值
			int lastLogTerm = -1;	 // 候选人最后日志条目的任期
		};

		struct RequestVoteReply
		{
			int term = -1;				// 当前任期号，以便候选人去更新自己的任期号
			bool voteGranted = false;	// 候选人赢得了此张选票时为真
			typedef std::function<void(int,
				const RequestVoteArgs&,
				const RequestVoteReply&)> RequestVoteReplyCallback;		// 选举请求回复 回调 (peerId, args, reply)
		};
	}
}