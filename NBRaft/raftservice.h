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

			typedef std::function<void(Json::Value response)> ProcedureDoneCallback;				// Raft ����ִ����ɺ�Ļص�����
			typedef std::function<void(Json::Value&, const ProcedureDoneCallback&)> ReturnFunc;		// Raft ���������Ҫ�ص���Ӧ�ĺ���
			typedef std::function<void(Json::Value&)> NotifyFunc;									// Raft ������ɲ���Ҫ�ص���Ӧ�ĺ���
		public:



		private:
			
		};
	}
}