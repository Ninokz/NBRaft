#pragma once
#include <functional>
#include <json/json.h>

namespace Nano {
	namespace Raft {
		enum class ERaftState
		{
			Undefined,		// δ����: δ֪״̬, һ�����ڳ�ʼ��
			Follower,		// ������: ֻ�Ǳ����������󣬲��ᷢ���κ����󣬸�����Ӧ���� leader �� candidate ������
			Candidate,		// ��ѡ��: ��� follower ��ѡ��ʱ���������Ȼû�н��յ����� leader ������������Ϊ leader �Ѿ�崻��ˡ�
							// ��ʱ��ǰ�ļ�Ⱥ����Ҫ��һ���µ� leader������������ candidate ������һ��ѡ�١������ѡ���У���ô����ѡƱ����Ϊ leader
			Leader			// �쵼��: ������ͻ�����������ͻ����������д��������ô leader �Ὣ�������׷�ӵ��Լ�����־�У����������ڵ㷢�� AppendEntries RPC ����
							// ����ͻ���������Ƕ���������ô leader ��ֱ�ӷ����Լ���״̬
		};

		//
		// Request Vote RPC (����ѡƱ): �� candidate ��������ѡ��ʱΪ�Լ���Ʊ
		// Append Entries RPC(�����־): �� leader ������Ϊһ�����������ƺ����ڸ�����־�������ڵ㡣
		//

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
		};

		struct AppendEntriesArgs
		{
			int term = -1;				// �쵼�˵�����
			int leaderId = -1;			// �쵼�˵� Id���Ա��ڸ������ض�������
			int prevLogIndex = -1;		// �µ���־��Ŀ����֮ǰ������ֵ
			int prevLogTerm = -1;		// prevLogIndex ��Ŀ������
			Json::Value entries;		// Ҫ�������־��Ŀ������Ϊ��־��Ŀ���Ǳ����͸����з������ģ�
			int leaderCommit = -1;		// �쵼���Ѿ��ύ����־������ֵ
		};

		struct AppendEntriesReply
		{
			int term = -1;				// ��ǰ�����ڣ��Ա��쵼��ȥ�����Լ�
			bool success = false;		// ��������߰�����ƥ���� prevLogIndex �� prevLogTerm ����־ʱΪ��
			int expectIndex = -1;		// ��������������־��Ŀ����
			int expectTerm = -1;		// ��������������־��Ŀ����	
		};

		typedef std::function<void(const RequestVoteReply&)> RequestVoteDoneCallback;		// ��һ���ڵ��������ڵ㷢��ͶƱ������յ�ͶƱ�ظ�ʱ�������ô˻ص��Դ���ͶƱ���
		typedef std::function<void(const AppendEntriesReply&)> AppendEntriesDoneCallback;	// ��һ���ڵ��������ڵ㷢����־׷��������յ��ظ�ʱ�������ô˻ص��Դ���׷�ӽ��

		typedef std::function<void(const RequestVoteArgs&,
			const RequestVoteDoneCallback&)> DoRequestVoteCallback;							//���ڵ���Ҫ���������ڵ����ͶƱʱ�����ô˺�����������ͶƱ���󣬿������첽�ģ�ͶƱ������� RequestVoteDoneCallback �ص��д���
		typedef std::function<void(const AppendEntriesArgs& args,
			const AppendEntriesDoneCallback& done)> DoAppendEntriesCallback;				// ��һ���ڵ���Ҫ�������ڵ㷢����־׷������ʱ�����ô˺�������������־׷�����󣬿������첽�ģ�׷�ӽ������ AppendEntriesDoneCallback �ص��д���

		typedef std::function<void(int,const RequestVoteArgs&,
			const RequestVoteReply&)> RequestVoteReplyCallback;								// ѡ������ظ� �ص� (peerId, args, reply)	�� Raft ѡ�ٹ����У��쵼�ߺ�ѡ���������ڵ㷢��ͶƱ����󣬵��յ������ڵ��ͶƱ�ظ�ʱ������ص��ᱻ���ã�����ͶƱ���������¼�ĸ��ڵ㷢����������Ӧ�Ļظ���

		typedef std::function<void(int,const AppendEntriesArgs&,
			const AppendEntriesReply&)> AppendEntriesReplyCallback;							// ��־׷������ظ� �ص� (peerId, args, reply)	�� Raft �㷨�У��쵼�߽ڵ�������߽ڵ㷢����־��Ŀ׷������󣬸����߻᷵��һ�� AppendEntriesReply������ص����������ĸ������ߵ���Ӧ���������־��Ŀ�Ƿ�ɹ�׷��
	}
}