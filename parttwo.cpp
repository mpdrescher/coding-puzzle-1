// Solution for part II
// Benjamin Kircher, benjamin.kircher@gmail.com

#include <iostream>
#include <ostream>
#include <string>
#include <memory>
#include <stack>
#include <utility>
#include <thread>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <atomic>
#include <future>
#include <vector>
#include <exception>
#include <stdexcept>
#include <functional>

// #define MEASURE_TIME

#ifdef MEASURE_TIME
# include <chrono>
# include <sstream>
#endif

namespace
{
    const char open_paren    {'('};
    const char close_paren   {')'};
    const char open_brace    {'{'};
    const char close_brace   {'}'};
    const char open_bracket  {'['};
    const char close_bracket {']'};

    struct state;
    typedef std::unique_ptr<state> state_ptr;

    struct state
    {
        virtual ~state() = default;
        virtual state_ptr next(char) const = 0;
        virtual bool isclose(char) const = 0;
    };

    struct initial_state final : public state
    {
        state_ptr next(char) const;
        bool isclose(char) const;
    };

    struct parentheses_state final : public state
    {
        state_ptr next(char) const;
        bool isclose(char) const;
    };

    struct curly_braces_state final : public state
    {
        state_ptr next(char) const;
        bool isclose(char) const;
    };

    struct square_brackets_state final : public state
    {
        state_ptr next(char) const;
        bool isclose(char) const;
    };

    // Implementations
    state_ptr initial_state::next(char c) const
    {
        return c == open_paren ? state_ptr{new parentheses_state} : state_ptr{};
    }

    bool initial_state::isclose(char) const { return false; }

    state_ptr parentheses_state::next(char c) const
    {
        return c == open_brace ? state_ptr{new curly_braces_state}
            : state_ptr{};
    }

    bool parentheses_state::isclose(char c) const { return c == close_paren; }

    state_ptr curly_braces_state::next(char c) const
    {
        return c == open_bracket ? state_ptr{new square_brackets_state}
            : state_ptr{};
    }

    bool curly_braces_state::isclose(char c) const
    {
        return c == close_brace;
    }

    state_ptr square_brackets_state::next(char c) const
    {
        switch (c)
        {
            case open_paren:   return state_ptr{new parentheses_state};
            case open_brace:   return state_ptr{new curly_braces_state};
            case open_bracket: return state_ptr{new square_brackets_state};
        }
        return state_ptr{};
    }

    bool square_brackets_state::isclose(char c) const
    {
        return c == close_bracket;
    }

    // A thread-safe queue using locks and condition variables (from C++
    // Concurrency in Action, chapter 6.2)
    template <typename T> class locked_queue
    {
    public:
        typedef T value_type;

        locked_queue() = default;

        void push(value_type val)
        {
            std::lock_guard<std::mutex> guard{mutex_};
            data_.push(val);
            cond_.notify_one();
        }

        bool try_pop(value_type& val)
        {
            std::lock_guard<std::mutex> guard{mutex_};
            if (data_.empty())
            {
                return false;
            }
            val = data_.front();
            data_.pop();
            return true;
        }

        locked_queue(const locked_queue&) = delete;
        locked_queue& operator=(const locked_queue&) = delete;

    private:
        mutable std::mutex mutex_;
        std::queue<value_type> data_;
        std::condition_variable cond_;
    };

    // A very simple thread pool. Slightly adapted from C++ Concurrency in
    // Action, chapter 9.
    class thread_pool
    {
    public:
        thread_pool() : done_{false}, tasks_{}, workers_{}, joiner_{workers_}
        {
            const unsigned int hwthreads{std::thread::hardware_concurrency()};
            //      std::cout << "hwthreads: " << hwthreads << std::endl;

            try
            {
                for (unsigned int i = 0; i < hwthreads; ++i)
                {
                    workers_.emplace_back(
                        [this]
                        {
                            while (!done_)
                            {
                                std::function<void()> task;
                                if (tasks_.try_pop(task))
                                {
                                    task();
                                }
                                else
                                {
                                    std::this_thread::yield(); // Wait a little
                                                               // bit
                                }
                            }
                        });
                }
            }
            catch (std::exception&)
            {
                done_ = true;
                throw;
            }
        }

        ~thread_pool() { done_ = true; }

        template <typename Function, typename... Args>
        auto submit(Function&& function, Args&&... args) // URef
            -> std::future<typename std::result_of<Function(Args...)>::type>
        {
            typedef typename std::result_of<Function(Args...)>::type
            result_type;

            if (done_)
            {
                throw std::runtime_error("submit on stopped thread_pool");
            }

            auto taskptr = std::make_shared<std::packaged_task<result_type()>>(
                std::bind(std::forward<Function>(function),
                          std::forward<Args>(args)...));

            std::future<result_type> result = taskptr->get_future();
            tasks_.push([taskptr]()
                        { (*taskptr)(); });
            return result;
        }

    private:
        // Ensures we join our worker threads at scope exit.
        class join_threads
        {
        public:
            explicit join_threads(std::vector<std::thread>& threads)
                : threads_(threads)
            {
            }

            ~join_threads()
            {
                for (auto& t : threads_)
                {
                    if (t.joinable())
                    {
                        t.join();
                    }
                }
            }

        private:
            std::vector<std::thread>& threads_;
        };

        std::atomic<bool> done_;
        locked_queue<std::function<void()>> tasks_;
        std::vector<std::thread> workers_;
        join_threads joiner_;
    };

    bool wellformed(const std::string& str)
    {
        if (str.empty())
        {
            return true;
        }

        if (str.length() % 2)
        {
            return false;
        }

        std::stack<state_ptr> stack;
        stack.push(state_ptr{new initial_state});
        for (const auto& c : str)
        {
            auto next = stack.top()->next(c);
            if (next)
            {
                stack.push(std::move(next));
            }
            else if (stack.top()->isclose(c))
            {
                stack.pop();
            }
            else
            {
                return false;
            }
        }

        return true;
    }

#ifdef MEASURE_TIME
    // Returns a string containing the elapsed time between 'start' and 'end' in
    // milliseconds.
    std::string
    format_time(const std::chrono::time_point<std::chrono::steady_clock>& start,
                const std::chrono::time_point<std::chrono::steady_clock>& end)
    {
        typedef std::chrono::duration<float, std::milli> milliseconds;
        std::ostringstream sstr;
        const auto elapsed = end - start;
        sstr << std::chrono::duration_cast<milliseconds>(elapsed).count()
             << " [ms]";
        return sstr.str();
    }
#endif
}

int main()
{
#ifdef MEASURE_TIME
    auto start = std::chrono::steady_clock::now();
#endif

    thread_pool pool;
    std::vector<std::future<std::string>> results;

    try
    {
        std::string line;
        std::getline(std::cin, line);
        const unsigned long no_of_testcases{std::stoul(line)};
        for (unsigned long i = 0; i < no_of_testcases; ++i)
        {
            std::getline(std::cin, line);
            const unsigned long no_of_strings{std::stoul(line)};
            for (unsigned long index = 0; index < no_of_strings; ++index)
            {
                std::getline(std::cin, line);
                results.push_back(pool.submit([ index, line ]()->std::string
                                              {
                    return std::to_string(index + 1) +
                           (wellformed(line) ? ":true" : ":false");
                }));
            }
        }
    }
    catch (std::invalid_argument&) { return 1; }

    for (auto& result : results)
    {
        std::cout << result.get() << std::endl;
    }

#ifdef MEASURE_TIME
    auto end = std::chrono::steady_clock::now();
    std::cout << "total execution time: " << format_time(start, end) << '\n';
#endif

    return 0;
}
