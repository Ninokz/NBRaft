#include "baseserver.h"

namespace Nano {
	namespace Communication {
		BaseServer::BaseServer(short port) :
			m_port(port),
			m_running(false),
			m_ioc(),
			m_ceventHandler(std::make_shared<CEventHandler>()),
			m_sessionManager(std::make_shared<SessionManager>())
		{
			m_ceventHandler->addConnectHandler(std::static_pointer_cast<IConnectEventHandler>(m_sessionManager));
			m_ceventHandler->addCloseHandler(std::static_pointer_cast<ICloseEventHandler>(m_sessionManager));
			m_acceptor = std::make_unique<ConnectionAcceptor>(m_ioc, port, *m_ceventHandler);
		}

		BaseServer::~BaseServer()
		{
			stop();
		}

		void BaseServer::start()
		{
			try {
				boost::asio::signal_set signals(m_ioc, SIGINT, SIGTERM);
				signals.async_wait([this](auto, auto) {
					m_running = false;
					m_ioc.stop();
					m_acceptor->stopAccept();
					});

				m_acceptor->startAccept();
				m_listenThread = std::thread([this]() {
					m_running = true;
					m_ioc.run();
					});
				m_listenThread.join();
			}
			catch (std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}
		}
		void BaseServer::stop()
		{
			if (m_running)
			{
				m_ioc.stop();
				m_listenThread.join();
				m_running = false;
			}
		}
	}
}