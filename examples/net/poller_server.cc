#include "gtl/net/poller.h"
#include "gtl/net/socket.h"

static const int kMsgSize = 13;

struct Context {
  gtl::Socket socket;
  std::vector<std::string> in_msgs;
  int write_index = 0;
};

void HandleAccept(gtl::Poller& poller, Context* ctx) {
  while (true) {
    Context* client_context = new Context();
    client_context->socket = ctx->socket.Accept();
    if (client_context->socket < 0) {
      delete client_context;
      if (errno == EAGAIN) {
        break;
      }
      continue;
    }
    client_context->socket.SetNonBlocking();
    client_context->socket.EnableNoDelay();
    poller.Add(client_context->socket, gtl::Poller::kReadable, client_context);
    GTL_INFO("new client_context: {}, info: {}", fmt::ptr(client_context),
             gtl::Socket::GetSocketInfo(client_context->socket));
  }
}

void HandleRead(gtl::Poller& poller, Context* ctx) {
  std::string in_msg = ctx->socket.RecvAll(kMsgSize);
  GTL_INFO("count: {}, {} -> {}, msg: {}, size: {}", ctx->in_msgs.size(), ctx->socket.GetPeerAddr().ToString(),
           ctx->socket.GetLocalAddr().ToString(), in_msg, in_msg.size());
  ctx->in_msgs.emplace_back(in_msg);
  poller.Mod(ctx->socket, gtl::Poller::kReadable, gtl::Poller::kWritable, ctx);
}

void HandleWrite(gtl::Poller& poller, Context* ctx) {
  ssize_t send_size = ctx->socket.SendAll(ctx->in_msgs[ctx->write_index]);
  GTL_INFO("count: {}, {} <- {}, msg: {}, size: {}, send_size: {}", ctx->write_index,
           ctx->socket.GetPeerAddr().ToString(), ctx->socket.GetLocalAddr().ToString(), ctx->in_msgs[ctx->write_index],
           ctx->in_msgs[ctx->write_index].size(), send_size);
  ctx->write_index++;
  if (ctx->write_index < 2) {
    poller.Mod(ctx->socket, gtl::Poller::kWritable, gtl::Poller::kReadable, ctx);
  } else {
    poller.Del(ctx->socket, gtl::Poller::kWritable, ctx);
    GTL_INFO("delete client_context: {}", fmt::ptr(ctx));
    delete ctx;
  }
}

int main(int argc, char* argv[]) {
  GTL_SET_LEVEL(gtl::LogLevel::kDebug);
  const char* address_str = "[::]:9999";
  gtl::SocketAddress server_address(address_str);
  Context listen_context;
  listen_context.socket = gtl::Socket::ServerStart(server_address);
  if (listen_context.socket < 0) {
    return 0;
  }
#if defined(__APPLE__)
  gtl::KqueuePoller poller;
#else
  gtl::EpollPoller poller;
#endif

  GTL_INFO("init begin");
  bool success = poller.Init();
  GTL_INFO("init status: {}", success);
  success = poller.Add(listen_context.socket, gtl::Poller::kReadable | gtl::Poller::kWritable, (void*)&listen_context);
  GTL_INFO("Add status: {}", success);
  while (true) {
    int ret = poller.Wait();
    GTL_INFO("wait done, ret: {}", ret);
    for (int i = 0; i < ret; ++i) {
      const gtl::Poller::Result& result = poller.GetResult(i);
      Context* rctx = static_cast<Context*>(result.ptr);
      GTL_INFO("events: {}, info: {}", result.events, gtl::Socket::GetSocketInfo(rctx->socket));
      if (rctx->socket == listen_context.socket && gtl::Poller::EventReadable(result.events)) {
        HandleAccept(poller, rctx);
      } else if (gtl::Poller::EventReadable(result.events)) {
        HandleRead(poller, rctx);
      } else if (gtl::Poller::EventWritable(result.events)) {
        HandleWrite(poller, rctx);
      }
    }
  }
  return 0;
}