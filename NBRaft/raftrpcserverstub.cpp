#include "raftrpcserverstub.h"

namespace Nano {
	namespace Raft {
        RaftRpcServerStub::RaftRpcServerStub(short port, std::shared_ptr<Nano::Raft::RaftService> raftService) : RpcServerStub(port), m_raftService(raftService)
        {
            // 注册 requestVote 方法
            this->registReturn(
                "requestVote",
                {
                    {"term", Json::intValue},
                    {"candidateId", Json::intValue},
                    {"lastLogIndex", Json::intValue},
                    {"lastLogTerm", Json::intValue}
                },
                [this](Json::Value& params, const Rpc::ProcedureDoneCallback& done) {
                    int term = params["term"].asInt();
                    int candidateId = params["candidateId"].asInt();
                    int lastLogIndex = params["lastLogIndex"].asInt();
                    int lastLogTerm = params["lastLogTerm"].asInt();

                    m_raftService->requestVote(term, candidateId, lastLogIndex, lastLogTerm, done);
                }
            );

            // 注册 appendEntries 方法
            this->registReturn(
                "appendEntries",
                {
                    {"term", Json::intValue},
                    {"prevLogIndex", Json::intValue},
                    {"prevLogTerm", Json::intValue},
                    {"entries", Json::arrayValue},
                    {"leaderCommit", Json::intValue}
                },
                [this](Json::Value& params, const Rpc::ProcedureDoneCallback& done) {
                    int term = params["term"].asInt();
                    int prevLogIndex = params["prevLogIndex"].asInt();
                    int prevLogTerm = params["prevLogTerm"].asInt();
                    Json::Value entries = params["entries"];
                    int leaderCommit = params["leaderCommit"].asInt();

                    m_raftService->appendEntries(term, prevLogIndex, prevLogTerm, entries, leaderCommit, done);
                }
            );
        }
	}
}