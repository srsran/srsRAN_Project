
#ifndef SRSGNB_SHARED_MUTEX_H
#define SRSGNB_SHARED_MUTEX_H

#include <pthread.h>

namespace srsgnb {

class shared_mutex
{
public:
  shared_mutex() { pthread_rwlock_init(&m, nullptr); }
  ~shared_mutex() { pthread_rwlock_destroy(&m); }
  shared_mutex(const shared_mutex&) = delete;
  shared_mutex(shared_mutex&&)      = delete;
  shared_mutex& operator=(const shared_mutex&) = delete;
  shared_mutex& operator=(shared_mutex&&) = delete;

  void lock() { pthread_rwlock_wrlock(&m); }
  void unlock() { pthread_rwlock_unlock(&m); }

  void lock_shared() { pthread_rwlock_rdlock(&m); }
  void unlock_shared() { pthread_rwlock_unlock(&m); }

private:
  pthread_rwlock_t m;
};

struct shared_lock_guard {
public:
  shared_lock_guard() = default;
  shared_lock_guard(shared_mutex& m_) : m(&m_) { m->lock_shared(); }
  ~shared_lock_guard() { m->unlock_shared(); }
  shared_lock_guard(const shared_lock_guard&) = delete;
  shared_lock_guard(shared_lock_guard&& other) noexcept : m(other.m) { other.m = nullptr; }
  shared_lock_guard& operator=(const shared_lock_guard&) = delete;
  shared_lock_guard& operator=(shared_lock_guard&&) = delete;

  bool owns_lock() const { return m != nullptr; }

private:
  shared_mutex* m = nullptr;
};

template <typename T>
class shared_synchronized
{
public:
  shared_synchronized() = default;
  shared_synchronized(shared_mutex& m, T& t_) : lock(m), t(&t_) {}
  shared_synchronized(shared_lock_guard&& m, T& t_) : lock(std::move(m)), t(&t_) {}

  T*       operator->() { return t; }
  const T* operator->() const { return t; }

  bool empty() const { return lock.owns_lock(); }

private:
  shared_lock_guard lock;
  T*                t = nullptr;
};

} // namespace srsgnb

#endif // SRSGNB_SHARED_MUTEX_H
