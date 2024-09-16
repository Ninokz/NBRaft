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
			// 运行结果状态的枚举
			enum class EStatus {
				Failure = 0,    // 失败
				Success = 1,    // 成功
				Running = 2,    // 运行中
				Aborted = 3,    // 中断
				Invalid = 4		// 无效
			};
			enum class EMetaType {
				Composite = 0b00,		// 组合节点 (2-bit: 00)
				Decorator = 0b01,		// 修饰节点 (2-bit: 01)
				Action =	0b10		// 动作节点 (2-bit: 10)
										// 预留: 0b11 可以用于未来扩展
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
			// 当进入该节点时才会执行一次的函数，类似FSM的OnEnter
			virtual void onInitialize() {};
			// 该节点的运行逻辑，会时时返回执行结果的状态，类似FSM的OnUpdate
			virtual EStatus onUpdate() = 0;
			// 当运行结束时才会执行一次的函数，类似FSM的OnExit
			virtual void onTerminate() {};
		protected:
			EStatus m_status;		// 运行状态
			std::string m_uid;		// 节点的唯一标识
			EMetaType m_metaType;	// 节点的元类型
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