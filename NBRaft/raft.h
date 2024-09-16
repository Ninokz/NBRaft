#pragma once

namespace Nano {
	namespace Raft {
		enum class ERaftState
		{
			Follower,
			Candidate,
			Leader
		};


	}
}