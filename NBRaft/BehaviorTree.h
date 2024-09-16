#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <stack>
#include <unordered_map>

namespace Nano {
	namespace BehaviorTree {
		class Behavior {
		public:
			// ���н��״̬��ö��
			enum class EStatus {
				Failure = 0,    // ʧ��
				Success = 1,    // �ɹ�
				Running = 2,    // ������
				Aborted = 3,    // �ж�
				Invalid = 4		// ��Ч
			};
			enum class EMetaType {
				Composite = 0b00,		// ��Ͻڵ� (2-bit: 00)
				Decorator = 0b01,		// ���νڵ� (2-bit: 01)
				Action =	0b10		// �����ڵ� (2-bit: 10)
										// Ԥ��: 0b11 ��������δ����չ
			};
			typedef std::shared_ptr<Behavior> Ptr;
		public:
			Behavior() = delete;
			Behavior(std::string uid, Behavior::EMetaType type);
			virtual ~Behavior() = default;
			bool isTerminated() const;
			bool isSuccess() const;
			bool isFailure() const;
			bool isRunning() const;
			std::string getUID() const;

			EMetaType getType() const;
			EStatus getStatus() const;

			EStatus tick();
			void reset();
			void abort();
			virtual void addChild(Behavior::Ptr child);
		protected:
			// ������ýڵ�ʱ�Ż�ִ��һ�εĺ���������FSM��OnEnter
			virtual void onInitialize() {};
			// �ýڵ�������߼�����ʱʱ����ִ�н����״̬������FSM��OnUpdate
			virtual EStatus onUpdate() = 0;
			// �����н���ʱ�Ż�ִ��һ�εĺ���������FSM��OnExit
			virtual void onTerminate() {};
		protected:
			EStatus m_status;		// ����״̬
			std::string m_uid;		// �ڵ��Ψһ��ʶ
			EMetaType m_metaType;	// �ڵ��Ԫ����
		};

		class Composite : public Behavior {
		public:
			typedef std::shared_ptr<Composite> Ptr;
			typedef std::list<Behavior::Ptr> BehaviorPtrDoubleList;
		public:
			Composite() = delete;
			Composite(std::string uid);
			virtual ~Composite() = default;
			virtual void removeChile(std::string uid);
			virtual void removeChild(Behavior::Ptr child);
			void clearChildren();
			virtual void addChild(Behavior::Ptr child) override;
		protected:
			std::shared_ptr<BehaviorPtrDoubleList> m_children;
		};

		class Sequence : public Composite {
		public:
			typedef std::shared_ptr<Sequence> Ptr;
			typedef Composite::BehaviorPtrDoubleList::iterator it_currentChild;
		public:
			Sequence(std::string uid);
			virtual ~Sequence() = default;
		protected:
			virtual void onInitialize() override;
			virtual EStatus onUpdate() override;
		protected:
			it_currentChild m_currentChild;
		};

		class Selector : public Sequence {
		public:
			typedef std::shared_ptr<Selector> Ptr;
		public:
			Selector() = delete;
			Selector(std::string uid);
			virtual ~Selector() = default;
		protected:
			virtual EStatus onUpdate() override;
		};

		class Parallel : public Composite {
		public:
			typedef std::shared_ptr<Parallel> Ptr;
			enum class Policy
			{
				RequireOne = 0,
				RequireAll = 1
			};
		public:
			Parallel() = delete;
			Parallel(std::string uid, Policy successPolicy, Policy failurePolicy);
			virtual ~Parallel() = default;

		protected:
			virtual EStatus onUpdate() override;
			virtual void onTerminate() override;
		protected:
			Policy m_successPolicy;
			Policy m_failurePolicy;
		};

		class Filter : public Sequence {
		public:
			typedef std::shared_ptr<Filter> Ptr;
		public:
			Filter() = delete;
			Filter(std::string uid);
			virtual ~Filter() = default;
			void AddCondition(Behavior::Ptr condition);
			void AddAction(Behavior::Ptr action);
		};

		class ActiveSelector : public Selector
		{
		public:
			typedef std::shared_ptr<ActiveSelector> Ptr;
		public:
			ActiveSelector() = delete;
			ActiveSelector(std::string uid);
			virtual ~ActiveSelector() = default;
			Behavior::EStatus onUpdate() override;
		};

		class Monitor : public Parallel {
		public:
			typedef std::shared_ptr<Monitor> Ptr;
		public:
			Monitor() = delete;
			Monitor(std::string uid, Policy successPolicy, Policy failurePolicy);
			virtual ~Monitor() = default;

			void AddCondition(Behavior::Ptr condition);
			void AddAction(Behavior::Ptr action);
		};

		class Decorator : public Behavior {
		public:
			typedef std::shared_ptr<Decorator> Ptr;
		public:
			Decorator() = delete;
			Decorator(std::string uid);
			virtual ~Decorator() = default;

			void addChild(Behavior::Ptr child) override;
		protected:
			Behavior::Ptr m_child;
		};

		class Inverter : public Decorator {
		public:
			typedef std::shared_ptr<Inverter> Ptr;
		public:
			Inverter() = delete;
			Inverter(std::string uid);
			virtual ~Inverter() = default;
		protected:
			Behavior::EStatus onUpdate() override;
		};

		class Repeat : public Decorator {
		public:
			typedef std::shared_ptr<Repeat> Ptr;
		public:
			Repeat() = delete;
			Repeat(std::string uid, int limit);
			virtual ~Repeat() = default;
		protected:
			virtual void onInitialize() override;
			Behavior::EStatus onUpdate() override;
		protected:
			int m_conunter;
			int m_limit;
		};

		class Action : public Behavior {
		public:
			typedef std::shared_ptr<Action> Ptr;
		public:
			Action() = delete;
			Action(std::string uid);
			virtual ~Action() = default;
			virtual void addChild(Behavior::Ptr child) final;
		};

		class BehaviorTree {
		public:
			typedef std::shared_ptr<BehaviorTree> Ptr;
		public:
			BehaviorTree();
			BehaviorTree(Behavior::Ptr root);
			virtual ~BehaviorTree() = default;

			void setRoot(Behavior::Ptr root);
			void tick();
			bool isRootExist() const;
		protected:
			Behavior::Ptr m_root;
		};

		class BehaviorTreeBuilder : public std::enable_shared_from_this<BehaviorTreeBuilder>
		{
		public:
			typedef std::shared_ptr<BehaviorTreeBuilder> Ptr;
		public:
			BehaviorTreeBuilder();
			virtual ~BehaviorTreeBuilder() = default;

			void addBehavior(Behavior::Ptr behavior);
			void treeTick();
			BehaviorTreeBuilder::Ptr Back();
			BehaviorTree::Ptr End();

			BehaviorTreeBuilder::Ptr Sequence(std::string uid);
			BehaviorTreeBuilder::Ptr Selector(std::string uid);
			BehaviorTreeBuilder::Ptr Parallel(std::string uid, Parallel::Policy successPolicy, Parallel::Policy failurePolicy);
			BehaviorTreeBuilder::Ptr Filter(std::string uid);
			BehaviorTreeBuilder::Ptr ActiveSelector(std::string uid);
			BehaviorTreeBuilder::Ptr Monitor(std::string uid, Parallel::Policy successPolicy, Parallel::Policy failurePolicy);
			BehaviorTreeBuilder::Ptr Inverter(std::string uid);
			BehaviorTreeBuilder::Ptr Repeat(std::string uid, int limit);
			BehaviorTreeBuilder::Ptr Action(Action::Ptr actionPtr);
		private:
			std::stack<Behavior::Ptr> m_nodeStack;
			BehaviorTree::Ptr m_tree;
		};
	}
}