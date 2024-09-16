#pragma once
#include <functional>

namespace Nano {
	namespace Raft {
		enum class ERaftState
		{
			Follower,
			Candidate,
			Leader
		};

		struct RequestVoteArgs
		{
			int term = -1;
			int candidateId = -1;
			int lastLogIndex = -1;
			int lastLogTerm = -1;
		};

		struct RequestVoteReply
		{
			int term = -1;
			bool voteGranted = false;

			typedef std::function<void(int, const RequestVoteArgs&, const RequestVoteReply&)> RequestVoteReplyCallback;
		};
	}
}