#include <algorithm>
#include <functional>
#include "Portability.h"
#include "LogSource.h"

using namespace std;

namespace YSLog {

bool LogSource::subscribe(LogListener* pListener)
{
  _lListenerPtr.push_back(pListener);

  return true;
}

bool LogSource::unsubscribe(LogListener* pListener)
{
  _lListenerPtr.remove(pListener);

  return true;
}

string LogSource::module() const
{
  return _module;
}

bool LogSource::log(LogLevel lvl, const char* file, const char* func, int line, const char* msg) const
{
  if (!isEnabled(lvl))
    return false;

  LoggingEvent* pLogEv = new LoggingEvent(_module.c_str(), lvl, file, func, line, msg);

  return unconditionalLog(pLogEv);
}

bool LogSource::log(LoggingEvent* pLogEv) const
{
  if (!isEnabled(pLogEv->level()))
    return false;

  return unconditionalLog(pLogEv);
}

bool LogSource::unconditionalLog(LoggingEvent* pLogEv) const
{
  for_each(_lListenerPtr.begin(), _lListenerPtr.end(),
    bind2nd(mem_fun(&LogListener::publish), pLogEv));

  return true;
}

};
