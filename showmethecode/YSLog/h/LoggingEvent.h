#ifndef LOGGINGEVENT_H
#define LOGGINGEVENT_H

#include <sstream>
#include <string>

#ifndef __FUNCTION__
#define __FUNCTION__ "Unknown"
#endif

using namespace std;

namespace YSLog {

enum LogLevel {
  DEBUG = 0,
  TRACE = 1,
  INFO = 2,
  WARN = 3,
  ERROR = 4,
  FATAL = 5
};

class LoggingEvent : public ostringstream {
private:
  string _module;
  LogLevel _level;
  string _file;
  string _func;
  int _line;

  // Prohibit invocation of default constructor
  LoggingEvent();

public:
  LoggingEvent(
    const char * mod, LogLevel lvl, const char * file,
    const char * func, int line,
    const char * msg = ""
  ) :
    ostringstream(), _module(mod), _level(lvl), _file(file), _func(func), _line(line)
  {
    *this << msg;
  }

  string module() const
  {
    return _module;
  }

  LogLevel level() const
  {
    return _level;
  }

  string file() const
  {
    return _file;
  }

  string func() const
  {
    return _func;
  }

  int line() const
  {
    return _line;
  }

};

}

#endif // LOGGINGEVENT_H
