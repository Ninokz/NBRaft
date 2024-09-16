#pragma once
#include <memory>
#include <string>

#include "nocopyable.h"
#include "rpcclientstub.h"

#include "raft.h"

namespace Nano {
	namespace Raft {
		class RaftPeer : public Nano::Noncopyable
		{
		public:
			typedef std::shared_ptr<RaftPeer> Ptr;
			typedef std::unique_ptr<Nano::Rpc::RpcClientStub> RpcClientStubUniPtr;
		public:
			RaftPeer(int peerId, const std::string& peerIp, short peerPort);
			~RaftPeer();
			void connect();
			void disconnect();

		protected:
			const int m_peerId;;
			std::string m_peerIp;
			short m_peerPort;
			bool m_connected = false;
			RpcClientStubUniPtr m_client;

			RequestVoteReply::RequestVoteReplyCallback m_requestVoteReplyCallback;
		};
	}
}
