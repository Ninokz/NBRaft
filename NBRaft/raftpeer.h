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
			typedef std::unique_ptr<Nano::Rpc::RpcClientStub> ClientPtr;
		public:

		protected:
			const int m_peerId;;
			const std::string m_peerIp;
			const short m_peerPort;
			bool m_connected = false;
			ClientPtr m_client;

			RequestVoteReply::RequestVoteReplyCallback m_requestVoteReplyCallback;
		};
	}
}
