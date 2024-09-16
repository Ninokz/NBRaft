#include "BehaviorTree.h"

namespace Nano {
	namespace BehaviorTree {
		// 构造函数，初始化状态为无效
		Behavior::Behavior(std::string uid, Behavior::EMetaType type) :
			m_uid(uid),
			m_status(EStatus::Invalid),
			m_metaType(type)	
		{}

		// 是否运行结束
		bool Behavior::isTerminated() const {
			return isSuccess() || isFailure();
		}

		// 是否成功
		bool Behavior::isSuccess() const {
			return m_status == EStatus::Success;
		}

		// 是否失败
		bool Behavior::isFailure() const {
			return m_status == EStatus::Failure;
		}

		// 是否正在运行
		bool Behavior::isRunning() const {
			return m_status == EStatus::Running;
		}

		std::string Behavior::getUID() const
		{
			return m_uid;
		}

		Behavior::EMetaType Behavior::getType() const
		{
			return m_metaType;
		}

		Behavior::EStatus Behavior::getStatus() const
		{
			return m_status;
		}

		// 节点运行，会返回本次调用的结果，为父节点接下来的运行提供依据
		Behavior::EStatus Behavior::tick() {
			if (!isRunning()) {
				onInitialize();
			}
			m_status = onUpdate();
			if (!isRunning()) {
				onTerminate();
			}
			return m_status;
		}

		// 重置该节点的运作
		void Behavior::reset() {
			m_status = EStatus::Invalid;
		}

		// 强行打断该节点的运作
		void Behavior::abort() {
			onTerminate();
			m_status = EStatus::Aborted;
		}

		// 添加子节点，默认为空实现，子类可以重写
		void Behavior::addChild(Behavior::Ptr child) {}

		Composite::Composite(std::string uid) :
			Behavior(uid,Behavior::EMetaType::Composite),
			m_children(std::make_shared<BehaviorPtrDoubleList>())
		{
		}

		void Composite::removeChile(std::string uid)
		{
			for (auto it = m_children->begin(); it != m_children->end(); ++it) {
				if ((*it)->getUID() == uid) {
					m_children->erase(it);
					break;
				}
			}
		}

		void Composite::removeChild(Behavior::Ptr child)
		{
			m_children->remove(child);
		}

		void Composite::clearChildren()
		{
			m_children->clear();
		}

		void Composite::addChild(Behavior::Ptr child)
		{
			m_children->push_back(child);
		}

		Sequence::Sequence(std::string uid) :
			Composite(uid),
			m_currentChild(m_children->end())
		{
		}

		void Sequence::onInitialize()
		{
			m_currentChild = m_children->begin();
		}

		Behavior::EStatus Sequence::onUpdate()
		{
			/// 逐个执行子节点，直到有一个失败
			while (m_currentChild != m_children->end()) {
				Behavior::EStatus s = (*m_currentChild)->tick();//记录子节点运行返回的结果状态
				if (s != Behavior::EStatus::Success) {
					return s;
				}
				++m_currentChild;
			}
			return Behavior::EStatus::Success;
		}
		Selector::Selector(std::string uid) :
			Sequence(uid)
		{
		}

		Behavior::EStatus Selector::onUpdate()
		{
			/// 逐个执行子节点，直到有一个成功
			for (auto it = m_children->begin(); it != m_children->end(); ++it) {
				Behavior::EStatus s = (*it)->tick();
				if (s != Behavior::EStatus::Failure) {
					return s;
				}
			}
			return Behavior::EStatus::Failure;
		}
		Parallel::Parallel(std::string uid, Policy successPolicy, Policy failurePolicy) :
			Composite(uid),
			m_successPolicy(successPolicy),
			m_failurePolicy(failurePolicy)
		{
		}

		Behavior::EStatus Parallel::onUpdate()
		{
			int successCount = 0;
			int failureCount = 0;
			const size_t childCount = m_children->size();
			for (auto it = m_children->begin(); it != m_children->end(); ++it) {
				if (!(*it)->isTerminated()) {
					(*it)->tick();	/// 如果子节点没有结束，继续执行
				}
				if ((*it)->isSuccess()) {
					++successCount;	/// 记录成功的子节点数量
					if (m_successPolicy == Policy::RequireOne) {
						return Behavior::EStatus::Success;	/// 如果成功策略是RequireOne，只要有一个成功就返回成功
					}
				}
				if ((*it)->isFailure()) {
					++failureCount;	/// 记录失败的子节点数量
					if (m_failurePolicy == Policy::RequireOne) {
						return Behavior::EStatus::Failure;	/// 如果失败策略是RequireOne，只要有一个失败就返回失败
					}
				}
			}
			if (m_failurePolicy == Policy::RequireAll && failureCount == childCount)
			{
				return Behavior::EStatus::Failure;	/// 如果失败策略是RequireAll，所有子节点都失败才返回失败
			}
			if (m_successPolicy == Policy::RequireAll && successCount == childCount)
			{
				return Behavior::EStatus::Success;	/// 如果成功策略是RequireAll，所有子节点都成功才返回成功
			}
			return Behavior::EStatus::Running;
		}

		void Parallel::onTerminate()
		{
			for (auto it = m_children->begin(); it != m_children->end(); ++it) {
				if ((*it)->isRunning()) {
					(*it)->abort();	/// 如果子节点正在运行，强行打断
				}
			}
		}

		Filter::Filter(std::string uid) :
			Sequence(uid)
		{
		}

		void Filter::AddCondition(Behavior::Ptr condition)
		{
			this->m_children->push_front(condition);
		}

		void Filter::AddAction(Behavior::Ptr action)
		{
			this->m_children->push_back(action);
		}

		ActiveSelector::ActiveSelector(std::string uid) :
			Selector(uid)
		{
		}

		Behavior::EStatus ActiveSelector::onUpdate()
		{
			auto prev = m_currentChild;
			onInitialize();	// m_currentChild 更新为 m_children->begin()
			Behavior::EStatus s = Selector::onUpdate();
			if (prev != m_children->end() && m_currentChild != prev)
			{
				(*prev)->abort();
			}
			return s;
		}

		Monitor::Monitor(std::string uid, Policy successPolicy, Policy failurePolicy) :
			Parallel(uid, successPolicy, failurePolicy)
		{
		}

		void Monitor::AddCondition(Behavior::Ptr condition)
		{
			m_children->push_front(condition);
		}

		void Monitor::AddAction(Behavior::Ptr action)
		{
			m_children->push_back(action);
		}

		Decorator::Decorator(std::string uid) :
			Behavior(uid,Behavior::EMetaType::Decorator),
			m_child(nullptr)
		{
		}

		void Decorator::addChild(Behavior::Ptr child)
		{
			m_child = child;
		}

		Inverter::Inverter(std::string uid) :
			Decorator(uid)
		{
		}

		Behavior::EStatus Inverter::onUpdate()
		{
			m_child->tick();
			if (m_child->isFailure())
				return EStatus::Success;
			if (m_child->isSuccess())
				return EStatus::Failure;
			return EStatus::Running;
		}
		Repeat::Repeat(std::string uid, int limit) :
			Decorator(uid),
			m_limit(limit),
			m_conunter(0)
		{
		}

		void Repeat::onInitialize()
		{
			m_conunter = 0;
		}

		Action::Action(std::string uid) :
			Behavior(uid,Behavior::EMetaType::Action)
		{
		}

		void Action::addChild(Behavior::Ptr child)
		{
		}

		Behavior::EStatus Repeat::onUpdate()
		{
			while (m_limit < 0 || m_conunter < m_limit)
			{
				Behavior::EStatus s = m_child->tick();
				if (s == EStatus::Running)
					return EStatus::Running;
				if (s == EStatus::Failure)
					return EStatus::Failure;
				++m_conunter;
			}
			return EStatus::Success;
		}

		BehaviorTree::BehaviorTree() :
			m_root()
		{
		}

		BehaviorTree::BehaviorTree(Behavior::Ptr root) :
			m_root(root)
		{
		}

		void BehaviorTree::setRoot(Behavior::Ptr root)
		{
			m_root = root;
		}

		void BehaviorTree::tick()
		{
			m_root->tick();
		}

		bool BehaviorTree::isRootExist() const
		{
			if (m_root)
				return true;
			return false;
		}

		BehaviorTreeBuilder::BehaviorTreeBuilder() :
			m_tree(std::make_shared<BehaviorTree>()), 
			m_nodeStack()
		{

		}

		void BehaviorTreeBuilder::addBehavior(Behavior::Ptr behavior)
		{
			if (m_tree->isRootExist()) //有根节点时，加入构建栈
			{
				auto peek = m_nodeStack.top();
				peek->addChild(behavior);
			}
			else //当树没根时，新增得节点视为根节点
			{
				m_tree->setRoot(behavior);
			}
			//只有组合节点和修饰节点需要进构建堆
			if (behavior->getType() == Behavior::EMetaType::Composite || behavior->getType() == Behavior::EMetaType::Decorator)
				m_nodeStack.push(behavior);
		}

		void BehaviorTreeBuilder::treeTick()
		{
			m_tree->tick();
		}

		BehaviorTreeBuilder::Ptr BehaviorTreeBuilder::Back()
		{
			if (m_nodeStack.size() > 1)
				m_nodeStack.pop();
			return shared_from_this();
		}

		BehaviorTree::Ptr BehaviorTreeBuilder::End()
		{
			/// pop all nodes
			while (m_nodeStack.size() > 1)
				m_nodeStack.pop();
			return m_tree;
		}

		BehaviorTreeBuilder::Ptr BehaviorTreeBuilder::Sequence(std::string uid)
		{
			Sequence::Ptr seq = std::make_shared<Nano::BehaviorTree::Sequence>(uid);
			addBehavior(seq);
			return shared_from_this();
		}

		BehaviorTreeBuilder::Ptr BehaviorTreeBuilder::Selector(std::string uid)
		{
			Selector::Ptr sel = std::make_shared<Nano::BehaviorTree::Selector>(uid);
			addBehavior(sel);
			return shared_from_this();
		}

		BehaviorTreeBuilder::Ptr BehaviorTreeBuilder::Parallel(std::string uid, Parallel::Policy successPolicy, Parallel::Policy failurePolicy)
		{
			Parallel::Ptr par = std::make_shared<Nano::BehaviorTree::Parallel>(uid, successPolicy, failurePolicy);
			addBehavior(par);
			return shared_from_this();
		}

		BehaviorTreeBuilder::Ptr BehaviorTreeBuilder::Inverter(std::string uid)
		{
			Inverter::Ptr inv = std::make_shared<Nano::BehaviorTree::Inverter>(uid);
			addBehavior(inv);
			return shared_from_this();
		}

		BehaviorTreeBuilder::Ptr BehaviorTreeBuilder::Repeat(std::string uid, int limit)
		{
			Repeat::Ptr rep = std::make_shared<Nano::BehaviorTree::Repeat>(uid, limit);
			addBehavior(rep);
			return shared_from_this();
		}

		BehaviorTreeBuilder::Ptr BehaviorTreeBuilder::Action(Action::Ptr actionPtr)
		{
			addBehavior(actionPtr);
			return shared_from_this();
		}

		BehaviorTreeBuilder::Ptr BehaviorTreeBuilder::Filter(std::string uid)
		{
			Filter::Ptr fil = std::make_shared<Nano::BehaviorTree::Filter>(uid);
			addBehavior(fil);
			return shared_from_this();
		}

		BehaviorTreeBuilder::Ptr BehaviorTreeBuilder::ActiveSelector(std::string uid)
		{
			ActiveSelector::Ptr act = std::make_shared<Nano::BehaviorTree::ActiveSelector>(uid);
			addBehavior(act);
			return shared_from_this();
		}

		BehaviorTreeBuilder::Ptr BehaviorTreeBuilder::Monitor(std::string uid, Parallel::Policy successPolicy, Parallel::Policy failurePolicy)
		{
			Monitor::Ptr mon = std::make_shared<Nano::BehaviorTree::Monitor>(uid, successPolicy, failurePolicy);
			addBehavior(mon);
			return shared_from_this();
		}
	}
}