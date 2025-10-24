/*
 *
 * Copyright 2021-2025 Software Radio Systems Limited
 *
 * This file is part of srsRAN.
 *
 * srsRAN is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsRAN is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#pragma once

#include "cameron314/concurrentqueue.h"
#include "srsran/adt/detail/concurrent_queue_helper.h"
#include "srsran/adt/detail/concurrent_queue_params.h"
#include "srsran/adt/span.h"

namespace srsran {

/// \brief Specialization for moodycamel lockfree MPMC without a blocking mechanism.
///
/// The moodycamel lockfree MPMC queue is not technically a queue. It is more linked list of sub-queues, each sub-queue
/// used by a different producer. The consumer will round-robin across the sub-queues to dequeue elements. This means
/// that there are no linearizability guarantees.
/// While this queue provides a bounded queue API, it is hard to use it because it requires that the users know how
/// many producers they will need. So, instead we just use the unbounded queue API.
template <typename T>
class concurrent_queue<T, concurrent_queue_policy::moodycamel_lockfree_mpmc, concurrent_queue_wait_policy::non_blocking>
{
  using queue_impl = moodycamel::ConcurrentQueue<T>;

public:
  /// Value stored in the queue.
  using value_type = T;
  /// The queue policy used by this queue.
  static constexpr concurrent_queue_policy queue_policy = concurrent_queue_policy::moodycamel_lockfree_mpmc;
  /// The queue wait policy used by this queue.
  static constexpr concurrent_queue_wait_policy wait_policy = concurrent_queue_wait_policy::non_blocking;

  /// Explicit sequential consumer class for faster dequeueing.
  class consumer_type
  {
  public:
    consumer_type(queue_impl& q) : queue(&q), token(q) {}

    [[nodiscard]] bool   try_pop(T& elem) { return queue->try_dequeue(token, elem); }
    [[nodiscard]] size_t try_pop_bulk(span<T> batch)
    {
      return queue->try_dequeue_bulk(token, batch.begin(), batch.size());
    }

  protected:
    queue_impl*               queue;
    moodycamel::ConsumerToken token;
  };

  /// \brief Constructs a concurrent queue and pre-reserves the configured capacity.
  /// \param[in] qsize The capacity to reserve.
  /// \param[in] nof_prereserved_producers Number of producers to pre-reserve.
  /// \remark Given the moodycamel queue particularities, there is no guarantee that the provided capacity will
  /// always be available in the case of multiple producers.
  explicit concurrent_queue(size_t qsize, size_t nof_prereserved_producers = 2) :
    queue(qsize, 0, nof_prereserved_producers)
  {
  }

  /// \brief Pushes an element to the queue. If the producer sub-queue is full, the call mallocs a new block.
  [[nodiscard]] bool try_push(const T& elem) { return queue.enqueue(elem); }
  [[nodiscard]] bool try_push(T&& elem) { return queue.enqueue(std::move(elem)); }
  template <typename U>
  [[nodiscard]] size_t try_push_bulk(span<U> batch)
  {
    if constexpr (std::is_const_v<U>) {
      return queue.enqueue_bulk(batch.begin(), batch.size()) ? batch.size() : 0;
    }
    return queue.enqueue_bulk(std::make_move_iterator(batch.begin()), batch.size()) ? batch.size() : 0;
  }

  /// \brief Pushes an element to the queue in a blocking fashion.
  template <typename U>
  void push_blocking(U&& elem)
  {
    queue.enqueue(std::forward<U>(elem));
  }

  /// \brief Pops an element from the queue in a non-blocking fashion.
  ///
  /// If the queue is empty, the call returns false.
  [[nodiscard]] bool try_pop(T& elem) { return queue.try_dequeue(elem); }

  /// \brief Pops a batch of elements from the queue in a non-blocking fashion.
  [[nodiscard]] size_t try_pop_bulk(span<T> batch) { return queue.try_dequeue_bulk(batch.begin(), batch.size()); }

  /// Returns an estimate of the total number of elements currently in the queue.
  size_t size() const { return queue.size_approx(); }

  /// Returns an estimate on whether the queue is empty.
  bool empty() const { return size() == 0; }

  /// This queue is unbounded. It allocates when current capacity is reached.
  size_t capacity() const { return std::numeric_limits<size_t>::max(); }

  /// Creates a sequential consumer for this queue.
  consumer_type create_consumer() { return consumer_type(queue); }

protected:
  queue_impl queue;
};

/// Specialization for moodycamel lockfree MPMC with a blocking mechanism based on sleeps.
template <typename T>
class concurrent_queue<T, concurrent_queue_policy::moodycamel_lockfree_mpmc, concurrent_queue_wait_policy::sleep>
  : public detail::queue_sleep_crtp<
        concurrent_queue<T, concurrent_queue_policy::moodycamel_lockfree_mpmc, concurrent_queue_wait_policy::sleep>>,
    private concurrent_queue<T,
                             concurrent_queue_policy::moodycamel_lockfree_mpmc,
                             concurrent_queue_wait_policy::non_blocking>

{
  using non_block_queue_base = concurrent_queue<T,
                                                concurrent_queue_policy::moodycamel_lockfree_mpmc,
                                                concurrent_queue_wait_policy::non_blocking>;
  using blocking_ext_base    = detail::queue_sleep_crtp<concurrent_queue>;

public:
  using value_type                                           = T;
  static constexpr concurrent_queue_policy      queue_policy = concurrent_queue_policy::moodycamel_lockfree_mpmc;
  static constexpr concurrent_queue_wait_policy wait_policy  = concurrent_queue_wait_policy::sleep;

  /// Explicit consumer class for faster dequeueing.
  class consumer_type
  {
  public:
    consumer_type(concurrent_queue& q) : parent(&q), token(q.queue) {}

    [[nodiscard]] bool   try_pop(T& elem) { return parent->queue.try_dequeue(token, elem); }
    [[nodiscard]] size_t try_pop_bulk(span<const T> batch)
    {
      return parent->queue.try_pop_bulk(token, batch.begin(), batch.size());
    }
    [[nodiscard]] bool pop_blocking(T& elem)
    {
      return detail::queue_helper::pop_blocking_generic(*this, elem, parent->policy);
    }

  protected:
    concurrent_queue*                  parent;
    typename moodycamel::ConsumerToken token;
  };

  // Inherited non-blocking API methods.
  using non_block_queue_base::capacity;
  using non_block_queue_base::empty;
  using non_block_queue_base::size;
  using non_block_queue_base::try_pop;
  using non_block_queue_base::try_pop_bulk;
  using non_block_queue_base::try_push;
  using non_block_queue_base::try_push_bulk;

  // Inherited blocking API methods
  using blocking_ext_base::call_on_pop_blocking;
  using blocking_ext_base::pop_blocking;
  using blocking_ext_base::push_blocking;
  using blocking_ext_base::request_stop;

  explicit concurrent_queue(size_t qsize, std::chrono::microseconds sleep_time_ = std::chrono::microseconds{0}) :
    blocking_ext_base(sleep_time_), non_block_queue_base(qsize)
  {
  }
  explicit concurrent_queue(size_t                    qsize,
                            size_t                    nof_prereserved_producers = 2,
                            std::chrono::microseconds sleep_time_               = std::chrono::microseconds{0}) :
    blocking_ext_base(sleep_time_), non_block_queue_base(qsize, nof_prereserved_producers)
  {
  }

  /// Creates a sequential consumer for this queue.
  consumer_type create_consumer() { return consumer_type(*this); }
};

} // namespace srsran
