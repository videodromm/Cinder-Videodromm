#include "VDLog.h"

using namespace videodromm;

VDLog::VDLog()
{
	auto sysLogger = log::makeLogger<log::LoggerSystem>();
	sysLogger->setLoggingLevel(log::LEVEL_WARNING);
#ifdef _DEBUG
	log::makeLogger<log::LoggerFileRotating>("/tmp/vd", "vd.%Y.%m.%d.txt", false);
#else
	log::makeLogger<log::LoggerFileRotating>("/tmp/vd", "vd.%Y.%m.%d.txt", false);
#endif  // _DEBUG
}
