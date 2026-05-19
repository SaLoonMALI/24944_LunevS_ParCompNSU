// Sam Lunev. 2026.
#include <chrono>
#include <vector>
#include <queue>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <future>
#include <cmath>
#include <utility>
#include <iostream>
#include <fstream>
#include <sstream>
#include <random>
#include <memory>
#include <filesystem>


template <typename T>
T fun_sin(T x) {
    return std::sin(x);
}

template <typename T>
T fun_sqrt(T x) {
    return std::sqrt(x);
}

template <typename T>
T fun_pow(T x, T y) {
    return std::pow(x, y);
}

// ThreadPool implementation
template <typename T>
class ThreadPool {
private:
    std::vector<std::jthread> workers;
    std::queue<std::function<void()>> tasks;
    std::mutex queue_mutex;
    std::condition_variable cv;
    std::atomic<bool> is_running{false};

public:
    explicit ThreadPool(size_t num_threads) {
        start(num_threads);
    }

    ~ThreadPool() {
        stop();
    }

    void start(size_t num_threads) {
        std::lock_guard<std::mutex> lock(queue_mutex);
        is_running = true;
        for (size_t i = 0; i < num_threads; ++i) {
            workers.emplace_back([this] {
                while (is_running) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        cv.wait(lock, [this] { return !tasks.empty() || !is_running; });
                        if (!is_running && tasks.empty()) return;
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    task();
                }
            });
        }
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            is_running = false;
        }
        cv.notify_all();
    }

    template<typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args) -> std::future<typename std::invoke_result_t<F, Args...>> {
        using return_type = typename std::invoke_result_t<F, Args...>;
        
        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );
        
        std::future<return_type> res = task->get_future();
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            if (!is_running) throw std::runtime_error("ThreadPool is not running");
            tasks.emplace([task]() { (*task)(); });
        }
        cv.notify_one();
        return res;
    }
};

// Random number generator setup
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> dis(0.0, 10.0);

// Client functions with proper synchronization and random parameters
void client_sin(ThreadPool<double>& pool, std::ofstream& out, std::mutex& mtx, int num_tasks) {
    for (int i = 0; i < num_tasks; ++i) {
        double param = dis(gen);
        auto fut = pool.enqueue([param]() { return fun_sin(param); });
        double res = fut.get();
        {
            std::lock_guard<std::mutex> lock(mtx);
            out << "Sin " << param << " " << res << " " << i << "\n";
        }
    }
}

void client_sqrt(ThreadPool<double>& pool, std::ofstream& out, std::mutex& mtx, int num_tasks) {
    for (int i = 0; i < num_tasks; ++i) {
        double param = dis(gen);
        auto fut = pool.enqueue([param]() { return fun_sqrt(param); });
        double res = fut.get();
        {
            std::lock_guard<std::mutex> lock(mtx);
            out << "Sqrt " << param << " " << res << " " << i << "\n";
        }
    }
}

void client_pow(ThreadPool<double>& pool, std::ofstream& out, std::mutex& mtx, int num_tasks) {
    for (int i = 0; i < num_tasks; ++i) {
        double base = dis(gen);
        double exp = dis(gen);
        auto fut = pool.enqueue([base, exp]() { return fun_pow(base, exp); });
        double res = fut.get();
        {
            std::lock_guard<std::mutex> lock(mtx);
            out << "Pow " << base << " " << exp << " " << res << " " << i << "\n";
        }
    }
}

int main() {
    ThreadPool<double> pool(8); // 8 worker threads
    std::ofstream out("res.txt");
    std::mutex file_mutex;
    
    auto start_time = std::chrono::steady_clock::now();
    
    std::vector<std::jthread> clients;
    
    // Start all client threads
    clients.emplace_back([&pool, &out, &file_mutex]() {
        client_sin(pool, out, file_mutex, 10000);
    });
    
    clients.emplace_back([&pool, &out, &file_mutex]() {
        client_sqrt(pool, out, file_mutex, 10000);
    });
    
    clients.emplace_back([&pool, &out, &file_mutex]() {
        client_pow(pool, out, file_mutex, 10000);
    });
    
    // Wait for all client threads to complete
    for (auto& client : clients) {
        client.join();
    }
    
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    std::cout << "Execution time: " << duration.count() << " ms\n";
    
    out.close();
    return 0;
}
