#pragma once
#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#elif __linux__
#include <unistd.h>
#include <limits.h>
#endif
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>

#include "singleton.h"

namespace Nano {
	namespace Env {
		class EnvMgr : public Singleton<EnvMgr> {
			friend class Singleton<EnvMgr>;
		private:
			EnvMgr();
		public:
			std::string getRootPath();

			static std::string getCurrentDirectoryPlatformSpecific();

			void addEnvVar(const std::string& key, const std::string& value);

			std::string getEnvVar(const std::string& key);

			void delEnvVar(const std::string& key);

		private:
			std::string m_rootPath;
			std::map<std::string, std::string> m_envVars;
		};
	}
}