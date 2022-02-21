
#ifndef SRSGNB_ASYNC_H
#define SRSGNB_ASYNC_H

#include "awaiter_memory_storage.h"
#include "function_signature.h"
#include "srsgnb/support/srsran_assert.h"

namespace srsgnb {

namespace detail {

/// Special coroutine suspension point indexes
enum coro_state_tag_t : int32_t { tag_final_suspend = -2, tag_destroyed = -1, tag_init = 0 };

/// Metafunction for deriving Awaitable -> Awaiter
template <typename Awaitable>
using awaiter_t = decltype(std::declval<Awaitable>().get_awaiter());

/// Metafunction for deriving Awaitable -> Result
template <typename Awaitable>
using awaitable_result_t = decltype(std::declval<awaiter_t<Awaitable> >().await_resume());

/// SFINAE based on whether Result is void
template <typename Result, typename FunctionReturn = Result>
using enable_if_void = std::enable_if_t<std::is_same<Result, void>::value, FunctionReturn>;
template <typename Result, typename FunctionReturn = Result>
using enable_if_nonvoid = std::enable_if_t<not std::is_same<Result, void>::value, FunctionReturn>;

/// Metafunction for deriving coroutine implementation -> Promise
template <typename F>
using promise_of = typename std::decay_t<get_type_from_index_t<0, decltype(callable_arguments<F>())> >::promise_type;

/// Metafunction for deriving coroutine implementation -> Future Type
template <typename F>
using future_of = typename std::decay_t<get_type_from_index_t<0, decltype(callable_arguments<F>())> >::future_type;

/// Base class for coroutine frames
template <typename Promise>
struct base_coro_frame;

/// Common coroutine frame functionality that is independent of the promise type
/// Contains a small memory buffer to store awaiters and the current coroutine state
template <>
struct base_coro_frame<void> {
  base_coro_frame<void>()                             = default;
  base_coro_frame<void>(const base_coro_frame<void>&) = delete;
  base_coro_frame<void>(base_coro_frame<void>&&)      = delete;
  base_coro_frame<void>& operator=(const base_coro_frame<void>&) = delete;
  base_coro_frame<void>& operator=(base_coro_frame<void>&&) = delete;
  virtual ~base_coro_frame()                                = default;

  /// Resume coroutine from previously suspended point
  virtual void resume() = 0;

  /// Destroy coroutine frame
  virtual void destroy() = 0;

  /// Destroy coroutine function local objects. Note: Promise and coroutine state stay intact
  virtual void on_return() = 0;

  /// Used to save the state of the coroutine at suspension points
  int32_t state_index = detail::tag_init;

  /// in-place memory buffer to store awaiter
  static const std::size_t                                     default_mem_buffer_storage = 64;
  detail::awaiter_memory_storage_t<default_mem_buffer_storage> mem_buffer;

  /// Checks if coroutine is at a suspension point
  bool is_suspended() const { return not mem_buffer.empty(); }

  /// called during await operation to save the awaitable/awaiter in mem buffer, and pass resumable as continuation
  /// \param awaitable_obj object that coroutine awaits on
  /// \return true if awaitable is ready and false if the coroutine needs to suspend
  template <typename Awaitable>
  bool on_await_suspend(Awaitable&& awaitable_obj)
  {
    srsran_assert(not is_suspended(), "Trying to suspend already suspended coroutine");

    // store awaitable/awaiter in memory buffer
    mem_buffer.emplace(std::forward<Awaitable>(awaitable_obj));

    // check if awaiter is ready. If not, suspend coroutine
    detail::awaiter_t<Awaitable>& awaiter_obj = mem_buffer.get_awaiter<Awaitable>();
    if (not awaiter_obj.await_ready()) {
      awaiter_obj.await_suspend(this);
      return false;
    }
    return true;
  }

  /// called by coroutine to clear awaiter, after a awaitable is set
  template <typename Awaitable, typename EventType = detail::awaitable_result_t<Awaitable> >
  detail::enable_if_nonvoid<EventType> on_await_resume()
  {
    srsran_assert(is_suspended(), "Trying to resume non-suspended coroutine");

    // Extract result from awaiter and clear awaiter from memory buffer
    auto&     awaiter = mem_buffer.get_awaiter<Awaitable>();
    EventType ev      = awaiter.await_resume();
    mem_buffer.clear<Awaitable>();
    return ev;
  }

  /// called by resumable to clear awaiter and extract result, after a awaitable is set
  template <typename Awaitable, typename EventType = detail::awaitable_result_t<Awaitable> >
  detail::enable_if_void<EventType> on_await_resume()
  {
    srsran_assert(is_suspended(), "Trying to resume non-suspended coroutine");

    // Clear awaiter from memory buffer
    auto& a = mem_buffer.get_awaiter<Awaitable>();
    a.await_resume();
    mem_buffer.clear<Awaitable>();
  }
};

/// Extension of base_coro_frame<void> when Promise type is specified. Besides the coro state, it stores the promise
template <typename Promise>
struct base_coro_frame : public base_coro_frame<void>, private Promise {
  using promise_type = Promise;
  using future_type  = decltype(std::declval<promise_type>().get_return_object());

  /// Get promise object
  Promise&       promise() { return *this; }
  const Promise& promise() const { return *this; }

  /// From promise object, obtain the coroutine frame pointer
  static base_coro_frame<Promise>* from_promise(Promise* p) { return static_cast<base_coro_frame<Promise>*>(p); }
};

/// Coroutine Frame. Besides the coro state and promise, it stores the coroutine function
template <typename FunT>
struct coro_frame : public base_coro_frame<detail::promise_of<FunT> > {
  template <typename... Args>
  explicit coro_frame(Args&&... args)
  {
    // Create coroutine object in embedded memory buffer
    new (&task_storage) FunT(std::forward<Args>(args)...);
  }

  /// Resume coroutine from last suspended point
  void resume() final { (*get_impl())(*this); }

  /// Destroy coroutine frame
  void destroy() final
  {
    srsran_assert(this->state_index == detail::tag_final_suspend,
                  "coroutine deleted but it is not at final suspension point\n");

    // resume on final_suspend awaitable that is stored in coroutine frame memory_buffer
    this->template on_await_resume<decltype(this->promise().final_suspend())>();

    // called after final suspension in order to destroy & deallocate coroutine frame
    delete this;
  }

  /// Destroy coroutine function object at the moment of return
  void on_return() final
  {
    this->state_index = detail::tag_final_suspend;
    get_impl()->~FunT();
  }

private:
  FunT* get_impl() { return reinterpret_cast<FunT*>(&task_storage); }

  std::aligned_storage_t<sizeof(FunT), alignof(FunT)> task_storage;
};

} // namespace detail

/// Coroutine handle that will be passed between awaitables
template <typename Promise = void>
struct coro_handle;

/// Specialization with common functionality that is independent of promise type
template <>
struct coro_handle<void> {
  coro_handle() = default;
  coro_handle(detail::base_coro_frame<void>* frame_ptr_) : frame_ptr(frame_ptr_) {}
  coro_handle& operator=(detail::base_coro_frame<void>* frame_ptr_)
  {
    frame_ptr = frame_ptr_;
    return *this;
  }

  void resume() { frame_ptr->resume(); }
  void operator()() { resume(); }

  bool empty() const { return frame_ptr == nullptr; }

  bool operator==(const coro_handle<>& h) const { return frame_ptr == h.frame_ptr; }

private:
  detail::base_coro_frame<void>* frame_ptr = nullptr;
};

/// Specialization that extends coro_handle<void> with functionality that depends on the promise_type
template <typename Promise>
struct coro_handle : public coro_handle<> {
  coro_handle() = default;
  explicit coro_handle(detail::base_coro_frame<Promise>* frame_ptr_) : coro_handle<void>(frame_ptr_) {}
  coro_handle<Promise>& operator=(detail::base_coro_frame<Promise>* frame_ptr_)
  {
    this->frame_ptr = frame_ptr_;
    return *this;
  }

  /// Get stored promise object
  Promise&       promise() { return static_cast<detail::base_coro_frame<Promise>*>(frame_ptr)->promise(); }
  const Promise& promise() const { return static_cast<detail::base_coro_frame<Promise>*>(frame_ptr)->promise(); }

  /// From promise, extract coroutine handle
  static coro_handle<Promise> from_promise(Promise* p)
  {
    return coro_handle<Promise>{detail::base_coro_frame<Promise>::from_promise(p)};
  }

  /// Destroy coroutine frame
  void destroy()
  {
    if (frame_ptr != nullptr) {
      // destroy coroutine frame
      frame_ptr->destroy();
      frame_ptr = nullptr;
    }
  }

  /// Check if coroutine run until final suspension point
  bool done() const { return promise().ready(); }
};

/// Awaitable that never suspends
struct suspend_never {
  bool           await_ready() const { return true; }
  void           await_suspend(coro_handle<> cb) const {}
  void           await_resume() const {}
  suspend_never& get_awaiter() { return *this; }
};

/// Base class for resumable tasks that are not coroutines
template <typename FutureType>
struct base_resumable_procedure {
  using future_type  = FutureType;
  using promise_type = typename FutureType::promise_type;
  using result_type  = typename FutureType::result_type;

  /// Initiation of the resumable task
  virtual void start() = 0;

  /// Resume method
  void operator()(detail::base_coro_frame<promise_type>& ctx)
  {
    if (frame_ptr == nullptr) {
      frame_ptr = &ctx;
      start();
      return;
    }
    resume_method();
  }

protected:
  /// Called by resumable task to await Awaitable object to finish
  template <typename Awaitable, typename Derived>
  detail::enable_if_void<detail::awaitable_result_t<Awaitable> > async_await(Awaitable&& a, void (Derived::*action)())
  {
    auto resume_func = [this, action]() {
      frame_ptr->template on_await_resume<Awaitable>();
      (static_cast<Derived*>(this)->*action)();
    };
    if (not frame_ptr->on_await_suspend(a)) {
      resume_method = resume_func;
    } else {
      resume_func();
    }
  }

  /// Called by resumable task to await Awaitable object to finish and return result
  template <typename Awaitable, typename Derived, typename Arg>
  detail::enable_if_nonvoid<detail::awaitable_result_t<Awaitable>, void> async_await(Awaitable&& a,
                                                                                     void (Derived::*action)(Arg))
  {
    auto resume_func = [this, action]() {
      (static_cast<Derived*>(this)->*action)(frame_ptr->template on_await_resume<Awaitable>());
    };
    if (not frame_ptr->on_await_suspend(a)) {
      resume_method = resume_func;
    } else {
      resume_func();
    }
  }

  /// Called on resumable task completion
  template <typename R = result_type>
  detail::enable_if_void<R> async_return()
  {
    frame_ptr->promise().return_value();
    final_await();
  }

  /// Called on resumable task completion with non-void return
  template <typename U, typename R = result_type>
  detail::enable_if_nonvoid<R, void> async_return(U&& r)
  {
    static_assert(std::is_convertible<U, result_type>::value, "Invalid value returned in async_return(R)");
    frame_ptr->promise().return_value(std::forward<U>(r));
    final_await();
  }

  ~base_resumable_procedure() = default;

private:
  void final_await()
  {
    // destroy procedure object including its members (without destroying the rest of the frame)
    auto* local_ptr = frame_ptr;
    frame_ptr->on_return();
    // Note: Do not touch any procedure variable from now on.

    // suspend on final awaiter
    if (local_ptr->on_await_suspend(local_ptr->promise().final_suspend())) {
      // destroy coroutine frame
      local_ptr->destroy();
    }
  }

  detail::base_coro_frame<promise_type>* frame_ptr = nullptr; ///< pointer to coroutine frame
  std::function<void()>                  resume_method;       ///< next method to call on resume
};

/// Creates coroutine frame and launches task if eager
template <typename FunT, typename... Args>
auto launch_async(Args&&... args) -> typename detail::future_of<FunT>
{
  auto* frame = new detail::coro_frame<FunT>(std::forward<Args>(args)...);
  return frame->promise().get_return_object();
}

} // namespace srsgnb

#endif // SRSGNB_ASYNC_H
