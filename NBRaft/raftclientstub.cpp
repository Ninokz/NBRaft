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

            auto wrappedCallback = [callback](Json::Value response) {
                callback(response, false, false);  // Assuming no error or timeout for simplicity
            };

            this->rpcReturnCall("requestVote", "requestVote", params, wrappedCallback, m_timeout);
        }

        std::future<Rpc::CallRecord::Ptr> RaftRpcClientStub::AsyncRequestVote(int term, int candidateId, int lastLogIndex, int lastLogTerm, const std::function<void(Json::Value, bool, bool)>& callback)
        {
            std::unordered_map<std::string, Json::Value> params;
            params["term"] = term;
            params["candidateId"] = candidateId;
            params["lastLogIndex"] = lastLogIndex;
            params["lastLogTerm"] = lastLogTerm;

            auto wrappedCallback = [callback](Json::Value response) {
                callback(response, false, false);  // Assuming no error or timeout for simplicity
                };

            return this->asyncRpcReturnCall("requestVote", "requestVote", params, wrappedCallback, m_timeout);
        }

        void RaftRpcClientStub::AppendEntries(int term, int prevLogIndex, int prevLogTerm, const Json::Value& entries, int leaderCommit, const std::function<void(Json::Value, bool, bool)>& callback)
        {
            std::unordered_map<std::string, Json::Value> params;
            params["term"] = term;
            params["prevLogIndex"] = prevLogIndex;
            params["prevLogTerm"] = prevLogTerm;
            params["entries"] = entries;
            params["leaderCommit"] = leaderCommit;

            auto wrappedCallback = [callback](Json::Value response) {
                callback(response, false, false);  // Assuming no error or timeout for simplicity
                };

            this->rpcReturnCall("appendEntries", "appendEntries", params, wrappedCallback, m_timeout);
        }

        std::future<Rpc::CallRecord::Ptr> RaftRpcClientStub::AsyncAppendEntries(int term, int prevLogIndex, int prevLogTerm, const Json::Value& entries, int leaderCommit, const std::function<void(Json::Value, bool, bool)>& callback)
        {
            std::unordered_map<std::string, Json::Value> params;
            params["term"] = term;
            params["prevLogIndex"] = prevLogIndex;
            params["prevLogTerm"] = prevLogTerm;
            params["entries"] = entries;
            params["leaderCommit"] = leaderCommit;

            auto wrappedCallback = [callback](Json::Value response) {
                callback(response, false, false);  // Assuming no error or timeout for simplicity
                };

            return this->asyncRpcReturnCall("appendEntries", "appendEntries", params, wrappedCallback, m_timeout);
        }
    }
}