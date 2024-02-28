#ifndef RPC_TCP_BUFFER_H
#define RPC_TCP_BUFFER_H

#include <vector>
#include <memory>


// 这段代码定义了一个名为 `TcpBuffer` 的类，用于管理 TCP 缓冲区。它主要包括以下功能：

// 1. 初始化时设置缓冲区大小，并根据需要动态调整大小。
// 2. 提供了读写指针的位置查询功能，以及返回可读和可写字节数的函数。
// 3. 提供了向缓冲区写入数据和从缓冲区读取数据的方法。
// 4. 当读写指针移动到一定位置时，会对缓冲区进行调整，以避免内存空间的浪费。
// 实现了基本的缓冲区管理功能，可以用于在网络通信中进行数据的读写操作。
namespace rocket {

class TcpBuffer {

 public:

  typedef std::shared_ptr<TcpBuffer> s_ptr;

  TcpBuffer(int size);

  ~TcpBuffer();

  // 返回可读字节数
  int readAble();

  // 返回可写的字节数
  int writeAble();

  int readIndex();

  int writeIndex();

  void writeToBuffer(const char* buf, int size);

  void readFromBuffer(std::vector<char>& re, int size); 

  void resizeBuffer(int new_size); // 调整缓冲区大小

  void adjustBuffer(); // 调整缓冲区大小，以便释放未使用的空间。

  void moveReadIndex(int size); // 移动读指针。

  void moveWriteIndex(int size); // 移动写指针。

 private:
  int m_read_index {0};
  int m_write_index {0};
  int m_size {0};

 public:
  std::vector<char> m_buffer;

};


}


#endif