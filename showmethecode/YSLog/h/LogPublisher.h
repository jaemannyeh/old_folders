#ifndef LOGPUBLISHER_H
#define LOGPUBLISHER_H

namespace YSLog {

struct LogListener;

struct LogPublisher {
  // shared_ptr �� �ٲٱ�
  virtual bool subscribe(LogListener* pListener) = 0;
  virtual bool unsubscribe(LogListener* pListener) = 0;
};

}

#endif // LOGPUBLISHER_H
