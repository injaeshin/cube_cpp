#ifndef QUEUE_ADAPTER_HPP
#define QUEUE_ADAPTER_HPP

#include "concurrentqueue.h"

namespace core {

template <typename T>
using ConcurrentQueue = moodycamel::ConcurrentQueue<T>;

}
#endif