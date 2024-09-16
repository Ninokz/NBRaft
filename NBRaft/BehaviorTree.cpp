#include "BehaviorTree.h"

namespace Nano {
	namespace BehaviorTree {
		// ���캯������ʼ��״̬Ϊ��Ч
		Behavior::Behavior(std::string uid, Behavior::EMetaType type) :
			m_uid(uid),
			m_status(EStatus::Invalid),
			m_metaType(type)	
		{}

		// �Ƿ����н���
		bool Behavior::isTerminated() const {
			return isSuccess() || isFailure();
		}

		// �Ƿ�ɹ�
		bool Behavior::isSuccess() const {
			return m_status == EStatus::Success;
		}

		// �Ƿ�ʧ��
		bool Behavior::isFailure() const {
			return m_status == EStatus::Failure;
		}

		// �Ƿ���������
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

		// �ڵ����У��᷵�ر��ε��õĽ����Ϊ���ڵ�������������ṩ����
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

		// ���øýڵ������
		void Behavior::reset() {
			m_status = EStatus::Invalid;
		}

		// ǿ�д�ϸýڵ������
		void Behavior::abort() {
			onTerminate();
			m_status = EStatus::Aborted;
		}

		// ����ӽڵ㣬Ĭ��Ϊ��ʵ�֣����������д
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
			/// ���ִ���ӽڵ㣬ֱ����һ��ʧ��
			while (m_currentChild != m_children->end()) {
				Behavior::EStatus s = (*m_currentChild)->tick();//��¼�ӽڵ����з��صĽ��״̬
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
			/// ���ִ���ӽڵ㣬ֱ����һ���ɹ�
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
					(*it)->tick();	/// ����ӽڵ�û�н���������ִ��
				}
				if ((*it)->isSuccess()) {
					++successCount;	/// ��¼�ɹ����ӽڵ�����
					if (m_successPolicy == Policy::RequireOne) {
						return Behavior::EStatus::Success;	/// ����ɹ�������RequireOne��ֻҪ��һ���ɹ��ͷ��سɹ�
					}
				}
				if ((*it)->isFailure()) {
					++failureCount;	/// ��¼ʧ�ܵ��ӽڵ�����
					if (m_failurePolicy == Policy::RequireOne) {
						return Behavior::EStatus::Failure;	/// ���ʧ�ܲ�����RequireOne��ֻҪ��һ��ʧ�ܾͷ���ʧ��
					}
				}
			}
			if (m_failurePolicy == Policy::RequireAll && failureCount == childCount)
			{
				return Behavior::EStatus::Failure;	/// ���ʧ�ܲ�����RequireAll�������ӽڵ㶼ʧ�ܲŷ���ʧ��
			}
			if (m_successPolicy == Policy::RequireAll && successCount == childCount)
			{
				return Behavior::EStatus::Success;	/// ����ɹ�������RequireAll�������ӽڵ㶼�ɹ��ŷ��سɹ�
			}
			return Behavior::EStatus::Running;
		}

		void Parallel::onTerminate()
		{
			for (auto it = m_children->begin(); it != m_children->end(); ++it) {
				if ((*it)->isRunning()) {
					(*it)->abort();	/// ����ӽڵ��������У�ǿ�д��
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
			onInitialize();	// m_currentChild ����Ϊ m_children->begin()
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
			if (m_tree->isRootExist()) //�и��ڵ�ʱ�����빹��ջ
			{
				auto peek = m_nodeStack.top();
				peek->addChild(behavior);
			}
			else //����û��ʱ�������ýڵ���Ϊ���ڵ�
			{
				m_tree->setRoot(behavior);
			}
			//ֻ����Ͻڵ�����νڵ���Ҫ��������
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