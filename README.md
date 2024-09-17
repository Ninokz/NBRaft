# NBRaft

本项目是 Raft 共识算法的 C++ 实现，基于个人的RPC库（[NBRpc](https://github.com/Ninokz/NBRpc)）构建，NBRpc基于个人的（[NBlib](https://github.com/Ninokz/NBlib)）构建，底层封装自 `Boost.Asio`

## Intro.

Raft 实现的核心包含在以下文件中：

- `raft.h` / `raft.cpp`：主要的 Raft 算法实现。
- `raftRpcClientStub.h` / `raftRpcClientStub.cpp`：用于 Raft 通信的 RPC 客户端。
- `raftRpcServerStub.h` / `raftRpcServerStub.cpp`：用于 Raft 通信的 RPC 服务器。
- `raftrepeattimer.h`：Raft 算法中使用的重复计时器实现。
- `struct.h`：Raft 实现中使用的关键结构定义。

## Dependency.

- [NBRpc](https://github.com/Ninokz/NBRpc): RPC Frameworks based on NBlib
- [NBlib](https://github.com/Ninokz/NBlib): Net lib based on boost.asio
- [Boost 1.85.0](https://www.boost.org/users/history/version_1_85_0.html)
- [jsoncpp 1.9.5](https://github.com/open-source-parsers/jsoncpp)

## Ref.

关于 Raft 共识算法的更详细信息，请参阅 [Raft 论文](https://raft.github.io/raft.pdf)