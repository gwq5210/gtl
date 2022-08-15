#include "gtl/thread/thread.h"

#include <signal.h>

#include "gtl/util/defer.h"

namespace gtl {

std::atomic_int Thread::thread_index(0);

int Thread::GetThreadIndex() { return thread_index++; }

void* Thread::Routine(void* arg) {
  RoutineArg* routine_arg = reinterpret_cast<RoutineArg*>(arg);
  if (routine_arg == nullptr) {
    return nullptr;
  }
  auto routine = std::move(routine_arg->routine);
  void* data = routine_arg->data;
  delete routine_arg;
  return routine(data);
}

Thread::Attr* Thread::NewAttr(const Options& options) {
  if (options.joinable && options.stack_min_size == 0) {
    return nullptr;
  }

  Attr* attr = new Attr();
  int ret = pthread_attr_init(attr);
  if (ret != 0) {
    GTL_ERROR("pthread_attr_init failed, errno:{}, errmsg:{}", errno, strerror(errno));
    delete attr;
    return nullptr;
  }
  SetDetachState(attr, options.joinable ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED);
  if (options.stack_min_size > 0) {
    SetStackMinSize(attr, options.stack_min_size);
  }
  return attr;
}

void Thread::DeleteAttr(Attr* attr) {
  if (attr == nullptr) {
    return;
  }
  int ret = pthread_attr_destroy(attr);
  if (ret != 0) {
    GTL_ERROR("pthread_attr_destroy failed, errno:{}, errmsg:{}", errno, strerror(errno));
  }
  delete attr;
}

bool Thread::GetDetachState(Attr* attr, int& detach_state) {
  int ret = pthread_attr_getdetachstate(attr, &detach_state);
  if (ret != 0) {
    GTL_ERROR("pthread_attr_getdetachstate failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}

bool Thread::SetDetachState(Attr* attr, int detach_state) {
  int ret = pthread_attr_setdetachstate(attr, detach_state);
  if (ret != 0) {
    GTL_ERROR("pthread_attr_setdetachstate failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}

bool Thread::GetStackMinSize(Attr* attr, size_t& stack_min_size) {
  int ret = pthread_attr_getstacksize(attr, &stack_min_size);
  if (ret != 0) {
    GTL_ERROR("pthread_attr_getstacksize failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}

bool Thread::SetStackMinSize(Attr* attr, size_t stack_min_size) {
  stack_min_size = stack_min_size < PTHREAD_STACK_MIN ? PTHREAD_STACK_MIN : stack_min_size;
  int ret = pthread_attr_setstacksize(attr, stack_min_size);
  if (ret != 0) {
    GTL_ERROR("pthread_attr_setstacksize failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}

bool Thread::Detach() {
  if (!valid_) {
    return false;
  }

  int ret = pthread_detach(tid_);
  if (ret != 0) {
    GTL_ERROR("pthread_detach failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  set_detached(true);
  return true;
}

bool Thread::Join(void** retval /* = nullptr*/) {
  if (!valid_) {
    return false;
  }

  if (detached()) {
    GTL_ERROR("thread already detached");
    return false;
  }

  void* join_retval = nullptr;
  int ret = pthread_join(tid_, &join_retval);
  if (ret != 0) {
    GTL_ERROR("pthread_join failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  if (join_retval == PTHREAD_CANCELED) {
    GTL_INFO("thread canceled");
  }
  if (retval) {
    *retval = join_retval;
  }

  Clear();
  return true;
}

bool Thread::Cancel() {
  if (!valid_) {
    return false;
  }

  int ret = pthread_cancel(tid_);
  if (ret != 0) {
    GTL_ERROR("pthread_cancel failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}

bool Thread::Start(std::function<void*(void*)>&& routine, const std::string& name /* = ""*/, void* data /* = nullptr*/,
                   const Options* options /* = nullptr*/) {
  if (valid_) {
    GTL_ERROR("thread already created");
    return false;
  }

  Attr* attr = options ? NewAttr(*options) : nullptr;
  Defer defer([attr]() { DeleteAttr(attr); });

  RoutineArg* routine_arg = new RoutineArg();
  routine_arg->routine = std::move(routine);
  routine_arg->data = data;
  int ret = pthread_create(&tid_, attr, Routine, routine_arg);
  if (ret != 0) {
    GTL_ERROR("pthread_create failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  if (name.empty()) {
    GenerateName();
  } else {
    set_name(name);
  }
  valid_ = true;
  if (options && !options->joinable) {
    set_detached(true);
  }
  GTL_DEBUG("thread {} created", name_);
  return true;
}

bool Thread::Kill(int signo) {
  if (!valid_) {
    return false;
  }

  int ret = pthread_kill(tid_, signo);
  if (ret != 0) {
    GTL_ERROR("pthread_kill failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}

void Thread::GenerateName() {
  int thread_index = GetThreadIndex();
  set_name("thread_" + std::to_string(thread_index));
}

}  // namespace gtl