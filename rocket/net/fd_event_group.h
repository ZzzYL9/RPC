#ifndef RPC_FD_EVENT_GROUP_H
#define RPC_FD_EVENT_GROUP_H

#include <vector>
#include "../common/mutex.h"
#include "fdevent.h"

namespace rocket {

class FdEventGroup {

 public:

  FdEventGroup(int size);

  ~FdEventGroup();
  FdEvent* getFdEvent(int fd);

 public:
  static FdEventGroup* GetFdEventGroup();

 private:
  int m_size {0};
  std::vector<FdEvent*> m_fd_group;
  Mutex m_mutex;

};

}

#endif