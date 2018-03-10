#include "VDLog.h"

using namespace VideoDromm;

VDLog::VDLog()
{
	auto sysLogger = log::makeLogger<log::LoggerSystem>();
	sysLogger->setLoggingLevel(log::LEVEL_WARNING);
#ifdef _DEBUG
	/* fs::path path;
	path = getAppPath() / "log";
	auto bLog = log::makeLogger<log::LoggerBreakpoint>();
	bLog->setTriggerLevel(log::LEVEL_ERROR); 
	log::makeLogger<log::LoggerFileRotating>(path.string(), "videodromm.%Y.%m.%d.txt", false);*/
	log::makeLogger<log::LoggerFileRotating>("/tmp/videodromm", "videodromm.%Y.%m.%d.txt", false);
#else
	
#endif  // _DEBUG
}
