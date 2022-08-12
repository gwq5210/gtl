#include "gtl/thread/cond_var.h"

namespace gtl {

bool CondVar::Init() {
  int ret = pthread_cond_init(&cond_var_, nullptr);
  if (ret != 0) {
    GTL_ERROR("pthread_cond_init failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}

bool CondVar::Destroy() {
  int ret = pthread_cond_destroy(&cond_var_);
  if (ret != 0) {
    GTL_ERROR("pthread_cond_destroy failed, errno:{}, errmsg:{}", errno, strerror(errno));
    return false;
  }
  return true;
}

}  // namespace gtl