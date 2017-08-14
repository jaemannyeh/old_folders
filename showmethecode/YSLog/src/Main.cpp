#include <iostream>
#include <fstream>
#include <sstream>
#include "LogSource.h"
#include "SimpleLogSink.h"

// TODO: 전반적으로 포인터들은 모두 smart pointer 들로 바꾸자
// log4cxx 같은 MACRO 도 정의해야 한다.

using namespace std;

int
main(void)
{
  YSLog::LogSource root("Root", true, YSLog::DEBUG);
  YSLog::SimpleLogSink sls(new ofstream("log.txt", ios_base::out | ios_base::app));

  // To the file only
  root.subscribe(&sls);

  YSLog::LoggingEvent* pLogEv = new YSLog::LoggingEvent(
	"Root", YSLog::DEBUG, __FILE__, __FUNCTION__, __LINE__, 
	"Test Logging");

  *pLogEv << " ===> Wow! It's working";
  root.log(pLogEv);

  root.log(YSLog::INFO, __FILE__, __FUNCTION__, __LINE__, "Test Logging #2");

  YSLOG_DEBUG(root, "Error occurred, error code = 100");
  YSLOG_TRACE(root, "Error occurred, error code = 100");
  YSLOG_INFO(root, "Error occurred, error code = 100");
  YSLOG_WARN(root, "Error occurred, error code = 100");
  YSLOG_ERROR(root, "Error occurred, error code = 100");
  YSLOG_FATAL(root, "Error occurred, error code = 100");

  YSLog::SimpleLogSink coutSink(&cout);

  // To both the file and the standard output
  root.subscribe(&coutSink);

  YSLOG_DEBUG(root, "Error occurred, error code = 100");
  YSLOG_TRACE(root, "Error occurred, error code = 100");
  YSLOG_INFO(root, "Error occurred, error code = 100");
  YSLOG_WARN(root, "Error occurred, error code = 100");
  YSLOG_ERROR(root, "Error occurred, error code = 100");
  YSLOG_FATAL(root, "Error occurred, error code = 100");

  // to the standard output only
  root.unsubscribe(&sls);

  YSLOG_STARTLOG(root, YSLog::FATAL, "");
  YSLOG_APPENDLOG("Test Logging #3 - " << "Error code = " << 204);
  YSLOG_ENDLOG(root);

  YSLOG_STARTLOG(root, YSLog::INFO, "Starting -");
  YSLOG_APPENDLOG(endl << "step #1" << endl);
  YSLOG_APPENDLOG("step #2" << endl);
  YSLOG_APPENDLOG("step #3" << endl);
  YSLOG_ENDLOG(root);

  return 0;
}

