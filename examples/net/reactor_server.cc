#include "gtl/net/poller.h"
#include "gtl/net/reactor.h"
#include "gtl/net/socket.h"

#include "gflags/gflags.h"

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

void HandleEvents(gtl::Poller& poller, Context& listen_context, int events, void* ptr) {
  Context* rctx = static_cast<Context*>(ptr);
  GTL_INFO("events: {}, info: {}", events, gtl::Socket::GetSocketInfo(rctx->socket));
  if (rctx->socket == listen_context.socket && gtl::Poller::EventReadable(events)) {
    HandleAccept(poller, rctx);
  } else if (gtl::Poller::EventReadable(events)) {
    HandleRead(poller, rctx);
  } else if (gtl::Poller::EventWritable(events)) {
    HandleWrite(poller, rctx);
  }
}

int main(int argc, char* argv[]) {
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  GTL_SET_LEVEL(gtl::LogLevel::kDebug);
  const char* address_str = "[::]:9999";
  gtl::SocketAddress server_address(address_str);
  Context listen_context;
  listen_context.socket = gtl::Socket::ServerStart(server_address);
  if (listen_context.socket < 0) {
    return 0;
  }

  gtl::Reactor reactor;

  GTL_INFO("init begin");
  bool success = reactor.Init();
  GTL_INFO("init status: {}", success);
  success = reactor.poller()->Add(listen_context.socket, gtl::Poller::kReadable | gtl::Poller::kWritable,
                                  (void*)&listen_context);
  GTL_INFO("Add status: {}", success);

  reactor.poller()->set_wait_callback([](int event_num) { GTL_INFO("wait done, event_num: {}", event_num); });
  reactor.poller()->set_event_callback(std::bind(HandleEvents, std::ref(*reactor.poller()), std::ref(listen_context),
                                                 std::placeholders::_1, std::placeholders::_2));

  reactor.Dispatch();
  return 0;
}