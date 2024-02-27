#include <unistd.h>
#include "wakeup_fd_event.h"
#include "common/log.h"

namespace rocket{

WakeupFdEvent::WakeupFdEvent(int fd) : FdEvent(fd){
    // init();
}

WakeupFdEvent::~WakeupFdEvent(){

}

// void WakeupFdEvent::init(){
    // m_read_callback = [this](){
    //     char buf[8];
    //     while(read(m_fd, buf, 8) != -1 && errno != EAGAIN){

    //     }
    //     DEBUGLOG("read full bytes from wakeup fd[%d]", m_fd);
    // };
// }

void WakeupFdEvent::wakeup(){
    char buf[8] = {'a'};
    int rt = write(m_fd, buf, 8);
    if(rt != 8){
        ERRORLOG("write to wakeup fd less than 8 bytes, fd[%d]", m_fd);
    }
    DEBUGLOG("success read 8 bytes");
}
}