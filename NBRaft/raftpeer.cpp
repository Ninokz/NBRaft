#include "raftpeer.h"

namespace Nano {
	namespace Raft {
		RaftPeer::RaftPeer(int peerId, std::string ip, short port) : 
			m_peerId(peerId), 
			m_ip(ip), m_port(port), 
			m_isconnected(false),
			m_rpcClientStub(std::make_unique<Nano::Rpc::RpcClientStub>())
		{
		}

		RaftPeer::~RaftPeer()
		{
		}

		void RaftPeer::setEndpoint(std::string ip, short port)
		{
			m_ip = ip;
			m_port = port;
		}

		void RaftPeer::connect()
		{
			if (!m_isconnected)
				m_isconnected = m_rpcClientStub->connect(m_ip, m_port);
		}

		void RaftPeer::disconnect()
		{
			if (m_isconnected)
			{
				m_rpcClientStub->disconnect();
				m_isconnected = false;
			}
		}

		void RaftPeer::requestVote(const RequestVoteArgs& args)
		{
			if (!m_isconnected)
				return;

		}

		void RaftPeer::appendEntries(const AppendEntriesArgs& args)
		{
			if (!m_isconnected)
				return;
		}

		void RaftPeer::setRequestVoteReplyCallback(const Raft::RequestVoteReplyCallback& callback)
		{
			this->m_requestVoteReplyCallback = callback;
		}

		void RaftPeer::setAppendEntriesReplyCallback(const Raft::AppendEntriesReplyCallback& callback)
		{
			this->m_appendEntriesReplyCallback = callback;
		}
	}
}