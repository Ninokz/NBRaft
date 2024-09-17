#pragma once
#include <unordered_map>
#include <functional>

#include <json/json.h>

namespace Nano {
	namespace Raft {
		enum class ERaftState
		{
			Undefined,		// 未定义: 未知状态, 一般用于初始化
			Follower,		// 跟随者: 只是被动处理请求，不会发送任何请求，负责响应来自 leader 和 candidate 的请求
			Candidate,		// 候选人: 如果 follower 在选举时间结束后，依然没有接收到来自 leader 的请求，他会认为 leader 已经宕机了。
							// 这时候当前的集群中需要有一个新的 leader，所以它会变成 candidate 并发起一次选举。如果在选举中，获得大多数选票则会成为 leader
			Leader			// 领导者: 负责处理客户端请求，如果客户端请求的是写操作，那么 leader 会将这个请求追加到自己的日志中，并向其他节点发送 appendEntries RPC 请求
							// 如果客户端请求的是读操作，那么 leader 会直接返回自己的状态
		};

		//
		// Request Vote RPC (请求选票): 由 candidate 发起，用来选举时为自己拉票
		// Append Entries RPC(添加日志): 由 leader 发起，作为一种心跳检测机制和用于复制日志给其他节点。
		//

		struct RequestVoteArgs
		{
			int term = -1;			 // 候选人的任期
			int candidateId = -1;	 // 请求选票的候选人的 Id
			int lastLogIndex = -1;	 // 候选人的最后日志条目的索引值
			int lastLogTerm = -1;	 // 候选人最后日志条目的任期
		};

		struct RequestVoteReply
		{
			int term = -1;				// 当前任期号，以便候选人去更新自己的任期号
			bool voteGranted = false;	// 候选人赢得了此张选票时为真
		};

		struct AppendEntriesArgs
		{
			int term = -1;				// 领导人的任期
			int leaderId = -1;			// 领导人的 Id，以便于跟随者重定向请求
			int prevLogIndex = -1;		// 新的日志条目紧随之前的索引值
			int prevLogTerm = -1;		// prevLogIndex 条目的任期
			Json::Value entries;		// 要保存的日志条目（被称为日志条目的是被发送给所有服务器的）
			int leaderCommit = -1;		// 领导人已经提交的日志的索引值
		};

		struct AppendEntriesReply
		{
			int term = -1;				// 当前的任期，以便领导人去更新自己
			bool success = false;		// 如果跟随者包含了匹配上 prevLogIndex 和 prevLogTerm 的日志时为真
			int expectIndex = -1;		// 跟随者期望的日志条目索引
			int expectTerm = -1;		// 跟随者期望的日志条目任期
		};

		// 适配上述结构体到 Json::Value 与 unordered_map 的转换
		class RaftServiceAdapter
		{
		public:
			static Json::Value RequestVoteArgsToJson(const RequestVoteArgs& args) {
				Json::Value value;
				value["term"] = args.term;
				value["candidateId"] = args.candidateId;
				value["lastLogIndex"] = args.lastLogIndex;
				value["lastLogTerm"] = args.lastLogTerm;
				return value;
			}
			static RequestVoteArgs RequestVoteArgsFromJson(const Json::Value& value) {
				RequestVoteArgs args;
				args.term = value["term"].asInt();
				args.candidateId = value["candidateId"].asInt();
				args.lastLogIndex = value["lastLogIndex"].asInt();
				args.lastLogTerm = value["lastLogTerm"].asInt();
				return args;
			}

			static Json::Value RequestVoteReplyToJson(const RequestVoteReply& reply) {
				Json::Value value;
				value["term"] = reply.term;
				value["voteGranted"] = reply.voteGranted;
				return value;
			}
			static RequestVoteReply RequestVoteReplyFromJson(const Json::Value& value) {
				RequestVoteReply reply;
				reply.term = value["term"].asInt();
				reply.voteGranted = value["voteGranted"].asBool();
				return reply;
			}

			static Json::Value AppendEntriesArgsToJson(const AppendEntriesArgs& args) {
				Json::Value value;
				value["term"] = args.term;
				value["leaderId"] = args.leaderId;
				value["prevLogIndex"] = args.prevLogIndex;
				value["prevLogTerm"] = args.prevLogTerm;
				value["entries"] = args.entries;
				value["leaderCommit"] = args.leaderCommit;
				return value;
			}
			static AppendEntriesArgs AppendEntriesArgsFromJson(const Json::Value& value) {
				AppendEntriesArgs args;
				args.term = value["term"].asInt();
				args.leaderId = value["leaderId"].asInt();
				args.prevLogIndex = value["prevLogIndex"].asInt();
				args.prevLogTerm = value["prevLogTerm"].asInt();
				args.entries = value["entries"];
				args.leaderCommit = value["leaderCommit"].asInt();
				return args;
			}

			static Json::Value AppendEntriesReplyToJson(const AppendEntriesReply& reply) {
				Json::Value value;
				value["term"] = reply.term;
				value["success"] = reply.success;
				value["expectIndex"] = reply.expectIndex;
				value["expectTerm"] = reply.expectTerm;
				return value;
			}
			static AppendEntriesReply AppendEntriesReplyFromJson(const Json::Value& value) {
				AppendEntriesReply reply;
				reply.term = value["term"].asInt();
				reply.success = value["success"].asBool();
				reply.expectIndex = value["expectIndex"].asInt();
				reply.expectTerm = value["expectTerm"].asInt();
				return reply;
			}

			static std::unordered_map<std::string, Json::ValueType> RequestVoteArgsMap()
			{
				std::unordered_map<std::string, Json::ValueType> map;
				map["term"] = Json::ValueType::intValue;
				map["candidateId"] = Json::ValueType::intValue;
				map["lastLogIndex"] = Json::ValueType::intValue;
				map["lastLogTerm"] = Json::ValueType::intValue;
				return map;
			}

			static std::unordered_map<std::string, Json::ValueType> RequestVoteReplyMap()
			{
				std::unordered_map<std::string, Json::ValueType> map;
				map["term"] = Json::ValueType::intValue;
				map["voteGranted"] = Json::ValueType::booleanValue;
				return map;
			}

			static std::unordered_map<std::string, Json::ValueType> AppendEntriesArgsMap()
			{
				std::unordered_map<std::string, Json::ValueType> map;
				map["term"] = Json::ValueType::intValue;
				map["leaderId"] = Json::ValueType::intValue;
				map["prevLogIndex"] = Json::ValueType::intValue;
				map["prevLogTerm"] = Json::ValueType::intValue;
				map["entries"] = Json::ValueType::arrayValue;
				map["leaderCommit"] = Json::ValueType::intValue;
				return map;
			}

			static std::unordered_map<std::string, Json::ValueType> AppendEntriesReplyMap()
			{
				std::unordered_map<std::string, Json::ValueType> map;
				map["term"] = Json::ValueType::intValue;
				map["success"] = Json::ValueType::booleanValue;
				map["expectIndex"] = Json::ValueType::intValue;
				map["expectTerm"] = Json::ValueType::intValue;
				return map;
			}
		};

		typedef std::function<void(const RequestVoteReply&)> RequestVoteDoneCallback;		// 当一个节点向其他节点发送投票请求后，收到投票回复时，将调用此回调以处理投票结果
		typedef std::function<void(const AppendEntriesReply&)> AppendEntriesDoneCallback;	// 当一个节点向其他节点发送日志追加请求后，收到回复时，将调用此回调以处理追加结果

		typedef std::function<void(const RequestVoteArgs&,
			const RequestVoteDoneCallback&)> DoRequestVoteCallback;							//当节点需要请求其他节点进行投票时，调用此函数。它发送投票请求，可能是异步的，投票结果会在 RequestVoteDoneCallback 回调中处理。
		typedef std::function<void(const AppendEntriesArgs& args,
			const AppendEntriesDoneCallback& done)> DoAppendEntriesCallback;				// 当一个节点需要向其他节点发送日志追加请求时，调用此函数。它发送日志追加请求，可能是异步的，追加结果会在 AppendEntriesDoneCallback 回调中处理。

		typedef std::function<void(int, const RequestVoteArgs&,
			const RequestVoteReply&)> RequestVoteReplyCallback;								// 选举请求回复 回调 (peerId, args, reply)	在 Raft 选举过程中，领导者候选人向其他节点发送投票请求后，当收到其他节点的投票回复时，这个回调会被调用，处理投票结果，并记录哪个节点发起的请求和相应的回复。

		typedef std::function<void(int, const AppendEntriesArgs&,
			const AppendEntriesReply&)> AppendEntriesReplyCallback;							// 日志追加请求回复 回调 (peerId, args, reply)	在 Raft 算法中，领导者节点向跟随者节点发送日志条目追加请求后，跟随者会返回一个 AppendEntriesReply，这个回调处理来自哪个跟随者的响应，并检查日志条目是否成功追加
	}
}