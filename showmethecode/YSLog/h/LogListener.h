#ifndef LOGLISTENER_H
#define LOGLISTENER_H

namespace YSLog {

class LoggingEvent;

struct LogListener {
  // auto_ptr 또는 shared_ptr 로 바꾸기
  virtual bool publish(LoggingEvent* pLogEv) = 0;
};

}

#endif // LOGLISTENER_H
