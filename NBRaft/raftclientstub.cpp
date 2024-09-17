#include "raftclientstub.h"

namespace Nano {
	namespace Raft {
        RaftRpcClientStub::RaftRpcClientStub(int timeout):
			Rpc::RpcClientStub(), 
            m_timeout(timeout)
		{
		}

        RaftRpcClientStub::~RaftRpcClientStub()
        {
        }

        void RaftRpcClientStub::RequestVote(int term, int candidateId, int lastLogIndex, int lastLogTerm, const std::function<void(Json::Value, bool, bool)>& callback)
        {
            std::unordered_map<std::string, Json::Value> params;
            params["term"] = term;
            params["candidateId"] = candidateId;
            params["lastLogIndex"] = lastLogIndex;
            params["lastLogTerm"] = lastLogTerm;
            std::string id = boost::uuids::to_string(boost::uuids::random_generator()());

            auto wrappedCallback = [this,id,callback](Json::Value response) {
				auto record = m_rpcClient->getReturnCallRecord(id);
                if (record)
                {
                    if (record->isDone())
                        callback(response, false, false);
                    else
                        callback(response, false, true);
					m_rpcClient->removeCallRecord(id);
                }
                else
                {
					callback(response, true, true);
                }
            };
            this->rpcReturnCall(id, "requestVote", params, wrappedCallback, m_timeout);
        }

        std::future<Rpc::CallRecord::Ptr> RaftRpcClientStub::AsyncRequestVote(int term, int candidateId, int lastLogIndex, int lastLogTerm, const std::function<void(Json::Value, bool, bool)>& callback)
        {
            std::unordered_map<std::string, Json::Value> params;
            params["term"] = term;
            params["candidateId"] = candidateId;
            params["lastLogIndex"] = lastLogIndex;
            params["lastLogTerm"] = lastLogTerm;
            std::string id = boost::uuids::to_string(boost::uuids::random_generator()());

            auto wrappedCallback = [this, id, callback](Json::Value response) {
				auto record = m_rpcClient->getReturnCallRecord(id);
				if (record)
				{
					if (record->isDone())
						callback(response, false, false);
					else
						callback(response, false, true);
                    m_rpcClient->removeCallRecord(id);
				}
				else
				{
					callback(response, true, true);
				}
            };
            return this->asyncRpcReturnCall(id, "requestVote", params, wrappedCallback, m_timeout);
        }

        void RaftRpcClientStub::AppendEntries(int term, int prevLogIndex, int prevLogTerm, const Json::Value& entries, int leaderCommit, const std::function<void(Json::Value, bool, bool)>& callback)
        {
            std::unordered_map<std::string, Json::Value> params;
            params["term"] = term;
            params["prevLogIndex"] = prevLogIndex;
            params["prevLogTerm"] = prevLogTerm;
            params["entries"] = entries;
            params["leaderCommit"] = leaderCommit;
            std::string id = boost::uuids::to_string(boost::uuids::random_generator()());

            auto wrappedCallback = [this, id, callback](Json::Value response) {
				auto record = m_rpcClient->getReturnCallRecord(id);
				if (record)
				{
					if (record->isDone())
						callback(response, false, false);
					else
						callback(response, false, true);
                    m_rpcClient->removeCallRecord(id);
                }
				else
				{
					callback(response, true, true);
				}
            };
            this->rpcReturnCall(id, "appendEntries", params, wrappedCallback, m_timeout);
        }

        std::future<Rpc::CallRecord::Ptr> RaftRpcClientStub::AsyncAppendEntries(int term, int prevLogIndex, int prevLogTerm, const Json::Value& entries, int leaderCommit, const std::function<void(Json::Value, bool, bool)>& callback)
        {
            std::unordered_map<std::string, Json::Value> params;
            params["term"] = term;
            params["prevLogIndex"] = prevLogIndex;
            params["prevLogTerm"] = prevLogTerm;
            params["entries"] = entries;
            params["leaderCommit"] = leaderCommit;
            std::string id = boost::uuids::to_string(boost::uuids::random_generator()());
            auto wrappedCallback = [this, id, callback](Json::Value response) {
				auto record = m_rpcClient->getReturnCallRecord(id);
				if (record)
				{
					if (record->isDone())
						callback(response, false, false);
					else
						callback(response, false, true);
                    m_rpcClient->removeCallRecord(id);
                }
				else
				{
					callback(response, true, true);
                }
            };
            return this->asyncRpcReturnCall(id, "appendEntries", params, wrappedCallback, m_timeout);
        }
    }
}