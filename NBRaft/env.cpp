#include "env.h"

namespace Nano {
	namespace Env {
		EnvMgr::EnvMgr() : m_rootPath(EnvMgr::getCurrentDirectoryPlatformSpecific())
		{
		}

		std::string EnvMgr::getRootPath() {
			return m_rootPath;
		}

		std::string EnvMgr::getCurrentDirectoryPlatformSpecific()
		{
#ifdef _WIN32
			wchar_t cwd[MAX_PATH];
			DWORD length = GetCurrentDirectory(MAX_PATH, cwd);
			if (length > 0 && length < MAX_PATH) {
				char* cstr = new char[MAX_PATH];
				size_t convertedChars = 0;
				errno_t err = wcstombs_s(&convertedChars, cstr, MAX_PATH, cwd, _TRUNCATE);
				if (err != 0) {
					delete[] cstr;
					throw std::runtime_error("_ACRTIMP errno_t __cdecl wcstombs_s failed");
				}
				else {
					std::string str(cstr);
					delete[] cstr;
					return str;
				}
			}
			else {
				throw std::runtime_error("GetCurrentDirectory failed or buffer too small");
			}
#elif __linux__
			char cwd[PATH_MAX];
			if (getcwd(cwd, sizeof(cwd)) != NULL) {
				return std::string(cwd);
			}
			else {
				throw std::runtime_error("getcwd() failed on Linux");
			}
#endif
		}

		void EnvMgr::addEnvVar(const std::string& key, const std::string& value) {
			m_envVars[key] = value;
		}

		std::string EnvMgr::getEnvVar(const std::string& key) {
			return m_envVars[key];
		}

		void EnvMgr::delEnvVar(const std::string& key) {
			m_envVars.erase(key);
		}
	}
}