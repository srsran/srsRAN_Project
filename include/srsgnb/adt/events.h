
#ifndef SRSGNB_EVENTS_H
#define SRSGNB_EVENTS_H

#include "move_callback.h"

namespace srsgnb {

template <typename T>
class event_signal
{
public:
  struct listener {
    listener() = default;
    listener(event_signal<T>* parent_, uint32_t id_) : parent(parent_), id(id_) {}
    listener(const listener&) = delete;
    listener(listener&& other) noexcept : parent(other.parent), id(other.id) { other.parent = nullptr; }
    listener& operator=(const listener& other) noexcept = delete;
    listener& operator                                  =(listener&& other) noexcept
    {
      parent       = other.parent;
      id           = other.id;
      other.parent = nullptr;
    }
    ~listener()
    {
      if (parent != nullptr) {
        parent->remove_listener(id);
      }
    }

  private:
    event_signal<T>* parent = nullptr;
    uint32_t         id;
  };

  listener add_listener(move_callback<void(T)> observer_)
  {
    obs = std::move(observer_);
    return {this, 0};
  }
  template <typename Object>
  listener add_listener(Object& obj, void (Object::*resume)(T))
  {
    obs = [&obj, resume](T t) { obj.*resume(t); };
    return {this, 0};
  }
  void remove_listener(uint32_t id) { obs = {}; }

  void operator()(T t)
  {
    if (obs) {
      obs(t);
    }
  }

private:
  move_callback<void(T)> obs;
};

} // namespace srsgnb

#endif // SRSGNB_EVENTS_H
