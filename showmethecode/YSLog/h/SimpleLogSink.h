#ifndef SIMPLELOGSINK_H
#define SIMPLELOGSINK_H

#include <iostream>
#include "LogListener.h"
#include "LoggingEvent.h"

namespace YSLog {

// destructor ���� os �� �������ؾ� �ϳ� ?
// �׷� ����ڰ� cout, cerr ���� �Ѱ� �ָ� ��� �ؾ� �ϳ� ?
// �׷� ���� Ư��ó�� ???
// �ƴϸ� �̰͵� shared_ptr �� ó�� ?
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
