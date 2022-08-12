#include "gtl/thread/mutex.h"

#include "gtl/util/defer.h"

namespace gtl {

Mutex::Attr* Mutex::NewAttr(const Options& options) {
  if (options.type == Type::kDefault) {
    return nullptr;
  }

  Attr* attr = new Attr();
  int ret = pthread_mutexattr_init(attr);
  if (ret != 0) {
    GTL_ERROR("pthread_mutexattr_init failed, errno:{}, errmsg:{}", errno, strerror(errno));
    delete attr;
    return nullptr;
  }
  SetType(attr, options.type);
  return attr;
}

void Mutex::DeleteAttr(Attr* attr) {
  if (attr == nullptr) {
    return;
  }

  int ret = pthread_mutexattr_destroy(attr);
  if (ret != 0) {
    GTL_ERROR("pthread_mutexattr_destroy failed, errno:{}, errmsg:{}", errno, strerror(errno));
  }
  delete attr;
}

bool Mutex::GetType(Attr* attr, Type& type) {
  int itype = 0;
  int ret = pthread_mutexattr_gettype(attr, &itype);
  if (ret != 0) {
    GTL_ERROR("pthread_mutexattr_gettype failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  type = Type(itype);
  return true;
}

bool Mutex::SetType(Attr* attr, Type type) {
  int ret = pthread_mutexattr_settype(attr, (int)type);
  if (ret != 0) {
    GTL_ERROR("pthread_mutexattr_settype failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}

bool Mutex::Init(const Options* options /* = nullptr*/) {
  Attr* attr = options ? NewAttr(*options) : nullptr;
  Defer defer([attr]() { DeleteAttr(attr); });

  int ret = pthread_mutex_init(&lock_, attr);
  if (ret != 0) {
    GTL_ERROR("pthread_mutex_init failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}

bool Mutex::Destroy() {
  int ret = pthread_mutex_destroy(&lock_);
  if (ret != 0) {
    GTL_ERROR("pthread_mutex_destroy failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}

}  // namespace gtl