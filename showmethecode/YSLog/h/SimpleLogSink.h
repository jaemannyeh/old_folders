#ifndef SIMPLELOGSINK_H
#define SIMPLELOGSINK_H

#include <iostream>
#include "LogListener.h"
#include "LoggingEvent.h"

namespace YSLog {

// destructor 에서 os 를 릴리즈해야 하나 ?
// 그럼 사용자가 cout, cerr 등을 넘겨 주면 어떻게 해야 하나 ?
// 그런 경우는 특별처리 ???
// 아니면 이것도 shared_ptr 로 처리 ?
class SimpleLogSink : public LogListener {
private:
  ostream* _os;

public:
  SimpleLogSink(ostream* os) : _os(os)
  {
  }

  virtual bool publish(LoggingEvent* pLogEv);
};

}

#endif // SIMPLELOGSINK_H
