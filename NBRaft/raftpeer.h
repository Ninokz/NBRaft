#pragma once
#include <memory>
#include <string>
#include "nocopyable.h"
#include "rpcclientstub.h"

#include "raft.h"
#include "struct.h"

namespace Nano {
	namespace Raft {
		//// RaftPeer :
		/// 1. �� RpcClientStub �Ļ����ϣ���װ�� Raft Э��� RPC ����
		/// 2. ͨ�� RaftPeer::requestVote �� RaftPeer::appendEntries ���� RequestVote �� AppendEntries RPC ����
		/// 3. ͨ�� RaftPeer::setRequestVoteReplyCallback �� RaftPeer::setAppendEntriesReplyCallback ���ûص��������� RPC ����Ļظ�
		/// 4. ��Raft Cluster�е�һ���ڵ㣬�����������ڵ�ͨ�ţ�Cluster��ÿһ���ڵ㶼��һ��Peer
		class RaftPeer : public Noncopyable
		{
		public:
			typedef std::shared_ptr<RaftPeer> Ptr;
			typedef std::unique_ptr<Nano::Rpc::RpcClientStub> RpcClientStubUPtr;
		public:
			RaftPeer(int peerId, std::string ip, short port);
			~RaftPeer();

			void setEndpoint(std::string ip, short port);

			void connect();
			void disconnect();

			void requestVote(const RequestVoteArgs& args);
			void appendEntries(const AppendEntriesArgs& args);

			void setRequestVoteReplyCallback(const Raft::RequestVoteReplyCallback& callback);
			void setAppendEntriesReplyCallback(const Raft::AppendEntriesReplyCallback& callback);

		private:
			const int m_peerId;
			bool m_isconnected;
			std::string m_ip;
			short m_port;

			Raft::RequestVoteReplyCallback m_requestVoteReplyCallback;
			Raft::AppendEntriesReplyCallback m_appendEntriesReplyCallback;

			RpcClientStubUPtr m_rpcClientStub;
		};
	}
}
