#include <iostream>
#include <ostream>
#include <string>
#include <memory>
#include <stack>
#include <utility>
#include <chrono>
#include <sstream>

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

    bool initial_state::isclose(char) const
    {
        return false;
    }

    state_ptr parentheses_state::next(char c) const
    {
        return c == open_brace ? state_ptr{new curly_braces_state}
            : state_ptr{};
    }

    bool parentheses_state::isclose(char c) const
    {
        return c == close_paren;
    }

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

    // Returns a string containing the elapsed time between 'start' and 'end' in
    // milliseconds.
    std::string
    format_time(const std::chrono::time_point<std::chrono::steady_clock>& start,
        const std::chrono::time_point<std::chrono::steady_clock>& end)
    {
        typedef std::chrono::duration<float, std::milli> milliseconds;
        std::ostringstream sstr;
        const auto elapsed = end - start;
        sstr << std::chrono::duration_cast<milliseconds>(elapsed).count() << " [ms]";
        return sstr.str();
    }
}

int main()
{
    auto start = std::chrono::steady_clock::now();

    try
    {
        std::string line;
        std::getline(std::cin, line);
        const unsigned long no_of_testcases{std::stoul(line)};
        for (unsigned long i = 0; i < no_of_testcases; ++i)
        {
            std::getline(std::cin, line);
            const unsigned long no_of_strings{std::stoul(line)};
            for (unsigned long j = 0; j < no_of_strings; ++j)
            {
                std::getline(std::cin, line);
                std::cout << (wellformed(line) ? "True" : "False") << std::endl;
            }
        }
    }
    catch (std::invalid_argument&)
    {
        return 1;
    }

    auto end = std::chrono::steady_clock::now();
    std::cout << "total execution time: " << format_time(start, end) << '\n';

    return 0;
}

// Some implementation notes and ideas for further improvement:
// - <iostream> is slow. We could use <stdio.h> functions instead

