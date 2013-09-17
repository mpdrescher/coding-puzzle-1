#include <iostream>
#include <ostream>
#include <string>
#include <memory>
#include <stack>
#include <utility>

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

    struct parenthesis_state final : public state
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

    // impls
    state_ptr initial_state::next(char c) const
    {
        return c == open_paren ? state_ptr{new parenthesis_state} : state_ptr{};
    }

    bool initial_state::isclose(char) const
    {
        return false;
    }

    state_ptr parenthesis_state::next(char c) const
    {
        return c == open_brace ? state_ptr{new curly_braces_state}
            : state_ptr{};
    }

    bool parenthesis_state::isclose(char c) const
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
            case open_paren:   return state_ptr{new parenthesis_state};
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
}

int main()
{
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::cout << (wellformed(line) ? "True" : "False") << std::endl;
    }

    return 0;
}

// Some implementation notes and ideas for further improvement:
// - <iostream> is slow. We could use <stdio.h> functions instead

