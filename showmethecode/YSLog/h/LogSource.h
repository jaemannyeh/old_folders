#ifndef LOGSOURCE_H
#define LOGSOURCE_H

#include <list>
#include <string>
#include "LogPublisher.h"
#include "LogListener.h"
#include "LoggingEvent.h"

namespace YSLog {

class LogSource : public LogPublisher {
private:
  string _module;
  bool _enable;
  LogLevel _level;
  list<LogListener*> _lListenerPtr;

public:
  LogSource(string mod, bool enable = true, LogLevel lvl = YSLog::WARN) :
    _module(mod), _enable(enable), _level(lvl), _lListenerPtr()
  {
  }

  virtual bool subscribe(LogListener* pListener);
  virtual bool unsubscribe(LogListener* pListener);

  bool isEnabled(LogLevel lvl) const
  {
    return (_enable && lvl >= _level && !_lListenerPtr.empty());
  }
  string module() const;
  bool log(LogLevel lvl, const char* file, const char* func, int line, const char* msg) const;
  bool log(LoggingEvent* pLogEv) const;
  bool unconditionalLog(LoggingEvent* pLogEv) const;
};

}

// Macro Expansion 시 에러의 좋은 예: pLogEv, level 등
#define YSLOG_LOG(logSource, lvl, msg)  \
  { \
    if ((logSource).isEnabled((lvl))) { \
	  YSLog::LoggingEvent* pLogEv = new YSLog::LoggingEvent( \
	    (logSource).module().c_str(), (lvl), \
		__FILE__, __FUNCTION__, __LINE__, (msg)); \
	  (logSource).unconditionalLog(pLogEv); \
    } \
  }

#define YSLOG_DEBUG(logSource, msg) YSLOG_LOG((logSource), YSLog::DEBUG, (msg))
#define YSLOG_TRACE(logSource, msg) YSLOG_LOG((logSource), YSLog::TRACE, (msg))
#define YSLOG_INFO(logSource, msg) YSLOG_LOG((logSource), YSLog::INFO, (msg))
#define YSLOG_WARN(logSource, msg) YSLOG_LOG((logSource), YSLog::WARN, (msg))
#define YSLOG_ERROR(logSource, msg) YSLOG_LOG((logSource), YSLog::ERROR, (msg))
#define YSLOG_FATAL(logSource, msg) YSLOG_LOG((logSource), YSLog::FATAL, (msg))

// msg 를 괄호로 일부러 묶지 않았기 때문에 << operator 보다 먼저 evaluation 되는
// operator 가 있을 경우에는 예상과 다른 결과가 나타날 수도 있음
// 조심해서 써야함. 이 문제를 해결할 수 있는 방법 좀 없을까 ???
#define YSLOG_STARTLOG(logSource, lvl, msg) \
  { \
  YSLog::LoggingEvent* pLogEv = 0; \
  if ((logSource).isEnabled((lvl))) { \
    pLogEv = new YSLog::LoggingEvent( \
      (logSource).module().c_str(), lvl, __FILE__, __FUNCTION__, __LINE__, ""); \
      *pLogEv << msg; \
  } \
  (void)0

#define YSLOG_APPENDLOG(msg) \
  if (pLogEv) { \
    *pLogEv << msg; \
  } \
  (void)0

#define YSLOG_ENDLOG(logSource) \
  if (pLogEv) { \
    (logSource).log(pLogEv); \
    pLogEv = 0; \
  } \
  } \
  (void)0

#endif // LOGSOURCE_H
