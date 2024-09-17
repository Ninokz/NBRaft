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
		/// 1. 在 RpcClientStub 的基础上，封装了 Raft 协议的 RPC 调用
		/// 2. 通过 RaftPeer::requestVote 和 RaftPeer::appendEntries 发送 RequestVote 和 AppendEntries RPC 请求
		/// 3. 通过 RaftPeer::setRequestVoteReplyCallback 和 RaftPeer::setAppendEntriesReplyCallback 设置回调函数处理 RPC 请求的回复
		/// 4. 在Raft Cluster中的一个节点，负责与其他节点通信，Cluster中每一个节点都是一个Peer
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
