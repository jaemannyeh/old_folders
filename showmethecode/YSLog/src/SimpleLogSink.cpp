#include <sstream>
#include "SimpleLogSink.h"

namespace YSLog {

static const char * pLevelStrList[6] = {
  "DEBUG",
  "TRACE",
  "INFO",
  "WARN",
  "ERROR",
  "FATAL"
};

bool SimpleLogSink::publish(LoggingEvent* pLogEv)
{
  ostringstream oss;

  oss << "[" << pLevelStrList[pLogEv->level()] << "] " << pLogEv->module() << "." << pLogEv->func() 
    << "() at " << pLogEv->file() << "::" << pLogEv->line() << " - " <<
    pLogEv->str() << endl;
  *_os << oss.str();

  delete pLogEv;

  return true;
}

};
