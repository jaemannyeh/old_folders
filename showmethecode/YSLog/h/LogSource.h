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

// Macro Expansion �� ������ ���� ��: pLogEv, level ��
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

// msg �� ��ȣ�� �Ϻη� ���� �ʾұ� ������ << operator ���� ���� evaluation �Ǵ�
// operator �� ���� ��쿡�� ����� �ٸ� ����� ��Ÿ�� ���� ����
// �����ؼ� �����. �� ������ �ذ��� �� �ִ� ��� �� ������ ???
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
