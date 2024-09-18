#pragma once
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "struct.h"
#include "nocopyable.h"
#include "rpcservice.h"

namespace Nano {
	namespace Raft {
		/// rpcservice �������� struct.h ��Raft��ؽṹ�嵽 rpcserverstub

		class RaftService 
		{
		public:
			typedef std::shared_ptr<RaftService> Ptr;
			/// �����ǲ����� Json ����Ļص�����
			using RaftProcedureDoneCallback = Rpc::ProcedureDoneCallback;			// Raft ����ִ����ɺ�Ļص����� - ���� Json�� - ���ù�ע���˷����� RpcServer �ڲ���ִ����ɺ����ڷ�����Ӧ

			using RequestVoteProcedureFunc = Rpc::ReturnFunc;						// ����ͶƱ����: Raft ���������Ҫ�ص���Ӧ�ĺ��� - ���� Json��
			using AppendEntriesProcedureFunc = Rpc::ReturnFunc;						// �����־����: Raft ������ɲ���Ҫ�ص���Ӧ�ĺ��� - ���� Json��

			/// ��Ϣ���ݹ���
			// Json::value -> ���뵽 ReturnFunc �д���Ȼ��ص� ReturnFunc�� ProcedureDoneCallback ������Ӧ


		public:


		
		};
	}
}