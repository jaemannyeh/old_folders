#ifndef LOGLISTENER_H
#define LOGLISTENER_H

namespace YSLog {

class LoggingEvent;

struct LogListener {
  // auto_ptr �Ǵ� shared_ptr �� �ٲٱ�
  virtual bool publish(LoggingEvent* pLogEv) = 0;
};

}

#endif // LOGLISTENER_H
