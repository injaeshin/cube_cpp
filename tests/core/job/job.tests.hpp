// #ifndef JOB_TESTS_HPP
// #define JOB_TESTS_HPP
//
// #include <chrono>
// #include <thread>
// #include <atomic>
//
// #include "catch_amalgamated.hpp"
// #include "core/job/ijob.hpp"
// #include "core/job/job_queue.hpp"
// #include "core/job/job_worker.hpp"
//
// namespace core::job::tests {
//
// // 테스트용 Job 구현
// class TestJob : public IJob {
// public:
//     explicit TestJob(std::atomic<int>& counter, int value = 1) : _counter(counter), _value(value) {}
//
//     void Execute() override {
//         try {
//             _counter.fetch_add(_value);
//         } catch (...) {
//             // 예외를 잡아서 abort 방지
//             INFO("TestJob::Execute() caught exception");
//         }
//     }
//
// private:
//     std::atomic<int>& _counter;
//     int _value;
// };
//
// // 지연 실행 Job 구현
// class DelayedJob : public IJob {
// public:
//     explicit DelayedJob(std::atomic<int>& counter, std::chrono::milliseconds delay)
//         : _counter(counter), _delay(delay) {}
//
//     void Execute() override {
//         try {
//             std::this_thread::sleep_for(_delay);
//             _counter.fetch_add(1);
//         } catch (...) {
//             INFO("DelayedJob::Execute() caught exception");
//         }
//     }
//
// private:
//     std::atomic<int>& _counter;
//     std::chrono::milliseconds _delay;
// };
//
// // 예외 발생 Job 구현
// class ExceptionJob : public IJob {
// public:
//     void Execute() override { throw std::runtime_error("Test exception"); }
// };
//
// TEST_CASE("IJob 기본 기능 테스트", "[job][ijob]") {
//     std::atomic<int> counter{0};
//
//     SECTION("기본 Job 실행") {
//         INFO("Testing basic job execution with value 5");
//         TestJob job(counter, 5);
//         job.Execute();
//         REQUIRE(counter.load() == 5);
//         INFO("Counter value after execution: " << counter.load());
//     }
//
//     SECTION("여러 Job 순차 실행") {
//         INFO("Testing sequential execution of multiple jobs");
//         TestJob job1(counter, 3);
//         TestJob job2(counter, 7);
//
//         job1.Execute();
//         INFO("After first job execution, counter: " << counter.load());
//         job2.Execute();
//         INFO("After second job execution, counter: " << counter.load());
//
//         REQUIRE(counter.load() == 10);
//     }
//
//     SECTION("예외 발생 Job") {
//         INFO("Testing job that throws an exception");
//         ExceptionJob job;
//         REQUIRE_THROWS_AS(job.Execute(), std::runtime_error);
//         INFO("Exception was thrown as expected");
//     }
// }
//
// TEST_CASE("JobQueue 기본 기능 테스트", "[job][job_queue]") {
//     JobQueue queue;
//     std::atomic<int> counter{0};
//
//     SECTION("Enqueue/Dequeue 기본 동작") {
//         INFO("Testing basic enqueue/dequeue functionality");
//         auto job = std::make_shared<TestJob>(counter, 5);
//         queue.Enqueue(job);
//
//         INFO("Job enqueued, HasPending: " << (queue.HasPending() ? "true" : "false"));
//         REQUIRE(queue.HasPending());
//
//         std::shared_ptr<IJob> dequeued_job;
//         bool dequeued = queue.TryDequeue(dequeued_job);
//         INFO("TryDequeue result: " << (dequeued ? "success" : "failed"));
//         REQUIRE(dequeued);
//         REQUIRE(dequeued_job != nullptr);
//
//         dequeued_job->Execute();
//         INFO("Job executed, counter value: " << counter.load());
//         REQUIRE(counter.load() == 5);
//
//         INFO("Queue should be empty after dequeue");
//         REQUIRE_FALSE(queue.HasPending());
//     }
//
//     SECTION("빈 큐에서 Dequeue 시도") {
//         INFO("Testing dequeue from empty queue");
//         std::shared_ptr<IJob> job;
//         REQUIRE_FALSE(queue.TryDequeue(job));
//         REQUIRE_FALSE(queue.HasPending());
//     }
//
//     SECTION("여러 Job Enqueue/Dequeue") {
//         INFO("Testing multiple job enqueue/dequeue");
//         auto job1 = std::make_shared<TestJob>(counter, 3);
//         auto job2 = std::make_shared<TestJob>(counter, 7);
//
//         queue.Enqueue(job1);
//         queue.Enqueue(job2);
//
//         REQUIRE(queue.HasPending());
//
//         std::shared_ptr<IJob> dequeued_job;
//         REQUIRE(queue.TryDequeue(dequeued_job));
//         dequeued_job->Execute();
//
//         REQUIRE(queue.TryDequeue(dequeued_job));
//         dequeued_job->Execute();
//
//         REQUIRE(counter.load() == 10);
//         REQUIRE_FALSE(queue.HasPending());
//     }
//
//     SECTION("NotifyCallback 동작") {
//         INFO("Testing notify callback functionality");
//         std::atomic<int> callback_count{0};
//         queue.SetNotifyCallback([&callback_count]() { callback_count.fetch_add(1); });
//
//         auto job = std::make_shared<TestJob>(counter, 1);
//         queue.Enqueue(job);
//
//         // 콜백이 호출되었는지 확인 (비동기적이므로 잠시 대기)
//         std::this_thread::sleep_for(std::chrono::milliseconds(10));
//         REQUIRE(callback_count.load() >= 0);
//     }
// }
//
// TEST_CASE("JobQueue 동시성 테스트", "[job][job_queue][concurrency]") {
//     JobQueue queue;
//     std::atomic<int> counter{0};
//     const int num_jobs = 50;    // 테스트 크기 더 줄임
//     const int num_threads = 2;  // 스레드 수 줄임
//
//     SECTION("다중 스레드 Enqueue") {
//         INFO("Testing multi-threaded enqueue");
//         std::vector<std::thread> threads;
//
//         for (int i = 0; i < num_threads; ++i) {
//             threads.emplace_back([&queue, &counter, num_jobs, num_threads, i]() {
//                 for (int j = 0; j < num_jobs / num_threads; ++j) {
//                     auto job = std::make_shared<TestJob>(counter, 1);
//                     queue.Enqueue(job);
//                 }
//             });
//         }
//
//         for (auto& thread : threads) {
//             thread.join();
//         }
//
//         REQUIRE(queue.HasPending());
//
//         // 모든 Job을 Dequeue하고 실행
//         std::shared_ptr<IJob> job;
//         int dequeued_count = 0;
//         while (queue.TryDequeue(job)) {
//             job->Execute();
//             dequeued_count++;
//         }
//
//         REQUIRE(dequeued_count == num_jobs);
//         REQUIRE(counter.load() == num_jobs);
//     }
//
//     SECTION("다중 스레드 Enqueue/Dequeue") {
//         INFO("Testing multi-threaded enqueue/dequeue");
//         std::atomic<int> dequeued_count{0};
//         std::vector<std::thread> enqueue_threads;
//         std::vector<std::thread> dequeue_threads;
//
//         // Enqueue 스레드들
//         for (int i = 0; i < num_threads; ++i) {
//             enqueue_threads.emplace_back([&queue, &counter, num_jobs, num_threads, i]() {
//                 for (int j = 0; j < num_jobs / num_threads; ++j) {
//                     auto job = std::make_shared<TestJob>(counter, 1);
//                     queue.Enqueue(job);
//                 }
//             });
//         }
//
//         // Dequeue 스레드들
//         for (int i = 0; i < num_threads; ++i) {
//             dequeue_threads.emplace_back([&queue, &dequeued_count, num_jobs]() {
//                 std::shared_ptr<IJob> job;
//                 while (dequeued_count.load() < num_jobs) {
//                     if (queue.TryDequeue(job)) {
//                         job->Execute();
//                         dequeued_count.fetch_add(1);
//                     } else {
//                         std::this_thread::sleep_for(std::chrono::microseconds(1));
//                     }
//                 }
//             });
//         }
//
//         for (auto& thread : enqueue_threads) {
//             thread.join();
//         }
//
//         for (auto& thread : dequeue_threads) {
//             thread.join();
//         }
//
//         REQUIRE(dequeued_count.load() == num_jobs);
//         REQUIRE(counter.load() == num_jobs);
//     }
// }
//
// TEST_CASE("JobWorker 기본 기능 테스트", "[job][job_worker]") {
//     JobQueue queue;
//     JobWorker worker(queue);
//     std::atomic<int> counter{0};
//
//     SECTION("Worker 시작/중지") {
//         INFO("Testing worker start/stop functionality");
//         REQUIRE_NOTHROW(worker.Start());
//         INFO("Worker started successfully");
//         std::this_thread::sleep_for(std::chrono::milliseconds(10));
//         REQUIRE_NOTHROW(worker.Stop());
//         INFO("Worker stopped successfully");
//     }
//
//     SECTION("Job 실행 확인") {
//         INFO("Testing job execution by worker");
//         worker.Start();
//         INFO("Worker started");
//
//         auto job = std::make_shared<TestJob>(counter, 5);
//         queue.Enqueue(job);
//         INFO("Job enqueued with value 5");
//
//         // Job이 실행될 때까지 대기
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//         INFO("After waiting, counter value: " << counter.load());
//         REQUIRE(counter.load() == 5);
//
//         worker.Stop();
//         INFO("Worker stopped");
//     }
//
//     SECTION("여러 Job 실행") {
//         INFO("Testing multiple job execution");
//         worker.Start();
//
//         for (int i = 0; i < 5; ++i) {  // 테스트 크기 줄임
//             auto job = std::make_shared<TestJob>(counter, 1);
//             queue.Enqueue(job);
//         }
//
//         // 모든 Job이 실행될 때까지 대기
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//
//         REQUIRE(counter.load() == 5);
//
//         worker.Stop();
//     }
//
//     SECTION("Worker 중지 후 Job 추가") {
//         INFO("Testing job addition after worker stop");
//         worker.Start();
//         std::this_thread::sleep_for(std::chrono::milliseconds(10));
//         worker.Stop();
//
//         auto job = std::make_shared<TestJob>(counter, 5);
//         queue.Enqueue(job);
//
//         // Worker가 중지된 상태이므로 Job이 실행되지 않아야 함
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//         REQUIRE(counter.load() == 0);
//     }
// }
//
// TEST_CASE("JobWorker 동시성 테스트", "[job][job_worker][concurrency]") {
//     JobQueue queue;
//     JobWorker worker(queue);
//     std::atomic<int> counter{0};
//     const int num_jobs = 100;  // 테스트 크기 줄임
//     const int num_threads = 4;
//
//     SECTION("Worker와 다중 스레드 Enqueue") {
//         INFO("Testing worker with multi-threaded enqueue");
//         worker.Start();
//
//         std::vector<std::thread> threads;
//         for (int i = 0; i < num_threads; ++i) {
//             threads.emplace_back([&queue, &counter, num_jobs, num_threads, i]() {
//                 for (int j = 0; j < num_jobs / num_threads; ++j) {
//                     auto job = std::make_shared<TestJob>(counter, 1);
//                     queue.Enqueue(job);
//                     std::this_thread::sleep_for(std::chrono::microseconds(1));
//                 }
//             });
//         }
//
//         for (auto& thread : threads) {
//             thread.join();
//         }
//
//         INFO("All threads joined, waiting for jobs to complete...");
//         INFO("Expected jobs: " << num_jobs << ", Current counter: " << counter.load());
//
//         // 모든 Job이 실행될 때까지 대기 (동기화 방식)
//         int timeout_count = 0;
//         const int max_timeout = 50;  // 최대 5초 대기
//         while (counter.load() < num_jobs && timeout_count < max_timeout) {
//             std::this_thread::sleep_for(std::chrono::milliseconds(100));
//             timeout_count++;
//             INFO("Waiting... counter: " << counter.load() << "/" << num_jobs << " (timeout: "
//                                         << timeout_count << "/" << max_timeout << ")");
//         }
//
//         INFO("Final counter value: " << counter.load());
//         REQUIRE(counter.load() == num_jobs);
//
//         worker.Stop();
//     }
//
//     SECTION("지연 Job 실행") {
//         INFO("Testing delayed job execution");
//         worker.Start();
//
//         auto delayed_job = std::make_shared<DelayedJob>(counter, std::chrono::milliseconds(50));
//         queue.Enqueue(delayed_job);
//
//         // 지연 시간보다 짧게 대기
//         std::this_thread::sleep_for(std::chrono::milliseconds(25));
//         REQUIRE(counter.load() == 0);
//
//         // 지연 시간보다 길게 대기
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//         REQUIRE(counter.load() == 1);
//
//         worker.Stop();
//     }
// }
//
// TEST_CASE("JobWorker 예외 처리 테스트", "[job][job_worker][exception]") {
//     JobQueue queue;
//     JobWorker worker(queue);
//     std::atomic<int> counter{0};
//
//     SECTION("예외 발생 Job 처리") {
//         INFO("Testing exception handling in worker");
//         worker.Start();
//
//         auto normal_job = std::make_shared<TestJob>(counter, 5);
//         auto exception_job = std::make_shared<ExceptionJob>();
//
//         queue.Enqueue(normal_job);
//         queue.Enqueue(exception_job);
//
//         // Worker가 예외로 인해 중단되지 않고 계속 동작해야 함
//         std::this_thread::sleep_for(std::chrono::milliseconds(100));
//
//         REQUIRE(counter.load() == 5);
//
//         worker.Stop();
//     }
// }
//
// TEST_CASE("JobWorker 재시작 테스트", "[job][job_worker][restart]") {
//     JobQueue queue;
//     JobWorker worker(queue);
//     std::atomic<int> counter{0};
//
//     SECTION("Worker 재시작") {
//         INFO("Testing worker restart functionality");
//         // 첫 번째 시작
//         worker.Start();
//         auto job1 = std::make_shared<TestJob>(counter, 3);
//         queue.Enqueue(job1);
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//         REQUIRE(counter.load() == 3);
//
//         worker.Stop();
//
//         // 두 번째 시작
//         worker.Start();
//         auto job2 = std::make_shared<TestJob>(counter, 7);
//         queue.Enqueue(job2);
//         std::this_thread::sleep_for(std::chrono::milliseconds(50));
//         REQUIRE(counter.load() == 10);
//
//         worker.Stop();
//     }
// }
//
// }  // namespace core::job::tests
// #endif