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
			int term = -1;			 // ��ѡ�˵�����
			int candidateId = -1;	 // ����ѡƱ�ĺ�ѡ�˵� Id
			int lastLogIndex = -1;	 // ��ѡ�˵������־��Ŀ������ֵ
			int lastLogTerm = -1;	 // ��ѡ�������־��Ŀ������
		};

		struct RequestVoteReply
		{
			int term = -1;				// ��ǰ���ںţ��Ա��ѡ��ȥ�����Լ������ں�
			bool voteGranted = false;	// ��ѡ��Ӯ���˴���ѡƱʱΪ��
			typedef std::function<void(int,
				const RequestVoteArgs&,
				const RequestVoteReply&)> RequestVoteReplyCallback;		// ѡ������ظ� �ص� (peerId, args, reply)
		};
	}
}