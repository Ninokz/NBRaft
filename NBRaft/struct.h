#pragma once
#include <unordered_map>
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
			Leader			// �쵼��: ������ͻ�����������ͻ����������д��������ô leader �Ὣ�������׷�ӵ��Լ�����־�У����������ڵ㷢�� appendEntries RPC ����
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

		// ���������ṹ�嵽 Json::Value �� unordered_map ��ת��
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

		typedef std::function<void(const RequestVoteReply&)> RequestVoteDoneCallback;		// ��һ���ڵ��������ڵ㷢��ͶƱ������յ�ͶƱ�ظ�ʱ�������ô˻ص��Դ���ͶƱ���
		typedef std::function<void(const AppendEntriesReply&)> AppendEntriesDoneCallback;	// ��һ���ڵ��������ڵ㷢����־׷��������յ��ظ�ʱ�������ô˻ص��Դ���׷�ӽ��

		typedef std::function<void(const RequestVoteArgs&,
			const RequestVoteDoneCallback&)> DoRequestVoteCallback;							//���ڵ���Ҫ���������ڵ����ͶƱʱ�����ô˺�����������ͶƱ���󣬿������첽�ģ�ͶƱ������� RequestVoteDoneCallback �ص��д���
		typedef std::function<void(const AppendEntriesArgs& args,
			const AppendEntriesDoneCallback& done)> DoAppendEntriesCallback;				// ��һ���ڵ���Ҫ�������ڵ㷢����־׷������ʱ�����ô˺�������������־׷�����󣬿������첽�ģ�׷�ӽ������ AppendEntriesDoneCallback �ص��д���

		typedef std::function<void(int, const RequestVoteArgs&,
			const RequestVoteReply&)> RequestVoteReplyCallback;								// ѡ������ظ� �ص� (peerId, args, reply)	�� Raft ѡ�ٹ����У��쵼�ߺ�ѡ���������ڵ㷢��ͶƱ����󣬵��յ������ڵ��ͶƱ�ظ�ʱ������ص��ᱻ���ã�����ͶƱ���������¼�ĸ��ڵ㷢����������Ӧ�Ļظ���

		typedef std::function<void(int, const AppendEntriesArgs&,
			const AppendEntriesReply&)> AppendEntriesReplyCallback;							// ��־׷������ظ� �ص� (peerId, args, reply)	�� Raft �㷨�У��쵼�߽ڵ�������߽ڵ㷢����־��Ŀ׷������󣬸����߻᷵��һ�� AppendEntriesReply������ص����������ĸ������ߵ���Ӧ���������־��Ŀ�Ƿ�ɹ�׷��
	}
}