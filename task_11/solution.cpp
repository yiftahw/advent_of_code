#include <deque>
#include <fstream>
#include <iostream>
#include <cstdint>
#include <vector>
#include <string>
#include <cassert>

#include "expected.hpp"
#include "optional.hpp"
#include "format.hpp"

typedef int32_t (*operation_fp)(int32_t);
typedef bool (*test_fp)(int32_t);

class Monkey; // forward declaration

class Game
{
    private:
    std::vector<Monkey> monkeys;
    public:
    Game() {}
    bool add_monkey(uint32_t monkey_id, uint32_t next_success_id, uint32_t next_fail_id, operation_fp inspect_fp, test_fp test);
    Expected<Monkey*, std::string> get_monkey(uint32_t monkey_id);
    void play(uint32_t rounds);
};

class Monkey
{
    private:
    uint32_t monkey_id;
    uint32_t next_success_monkey_id;
    uint32_t next_fail_monkey_id;
    int32_t inspection_counter;
    std::deque<int32_t> items;
    operation_fp inspect_fp;
    public:
    Monkey(uint32_t monkey_id, uint32_t next_success_monkey_id, uint32_t next_fail_monkey_id, operation_fp inspect_fp, test_fp test) : 
        monkey_id(monkey_id), next_success_monkey_id(next_success_monkey_id), next_fail_monkey_id(next_fail_monkey_id), inspect_fp(inspect_fp), test_item(test) {}
    bool add_item(int32_t item);
    Expected<int32_t, std::string> peak_item(void);
    Expected<int32_t, std::string> pop_item(void);
    void play_round(Game& game);
    int32_t inspect_item(int32_t item) { inspection_counter++; return inspect_fp(item); }
    test_fp test_item;
    uint32_t get_monkey_id() { return monkey_id; }
};

bool Monkey::add_item(int32_t item)
{
    try
    {
        items.push_back(item); 
    }
    catch(const std::bad_alloc& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;
}

Expected<int32_t, std::string> Monkey::peak_item(void)
{
    if(items.size() > 0)
        return Expected<int32_t, std::string>(items.front());
    else
        return Expected<int32_t, std::string>(ErrorTypeTag(), std::string("items list is empty!"));
}

Expected<int32_t, std::string> Monkey::pop_item(void)
{
    if(items.size() > 0)
    {
        int32_t result = items.front();
        items.pop_front();
        return Expected<int32_t, std::string>(result);
    }
    else
        return Expected<int32_t, std::string>(ErrorTypeTag(), std::string("items list is empty!"));
}

void Monkey::play_round(Game& game)
{
    Expected<int32_t, std::string> item = pop_item();
    while(item)
    {
        // inspect and pass the current item to the next monkey
        int32_t after_inspect = inspect_item(item.getValue());
        bool test_res = test_item(after_inspect);

        uint32_t next_monkey_id = test_res ? next_success_monkey_id : next_fail_monkey_id;
        Expected<Monkey*, std::string> res =  game.get_monkey(next_monkey_id);
        //! @todo an optimization here is to search for the reference only 1 time and then save as a member instead of querying the game structure every time
        if(res)
        {
            Monkey* next_monkey = res.getValue();
            next_monkey->add_item(after_inspect);
        }
        else
        {
            std::cout << res.getError() << std::endl; // log the error to screen
            return;
        }
        // last step - get the next item to check
        item = pop_item();
    }
}

bool Game::add_monkey(uint32_t monkey_id, uint32_t next_success_id, uint32_t next_fail_id, operation_fp inspect_fp, test_fp test)
{
    try
    {
        monkeys.push_back(Monkey(monkey_id, next_success_id, next_fail_id, inspect_fp, test));
    }
    catch(const std::bad_alloc& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;
}

Expected<Monkey*, std::string> Game::get_monkey(uint32_t monkey_id)
{
    for(Monkey& monkey : monkeys)
    {
        if(monkey.get_monkey_id() == monkey_id)
            return Expected<Monkey*, std::string>(&monkey);
    }
    return Expected<Monkey*, std::string>(ErrorTypeTag(), format_string("monkey %d not found!\n", monkey_id));
}

void Game::play(uint32_t rounds)
{
    for(uint32_t round = 0; round < rounds; round++)
    {
        for(Monkey monkey : monkeys)
        {
            monkey.play_round(*this);
        }
    }
}

/*
test functions
*/
int32_t inspect_op(int32_t number)
{
    return 2 * number;
}

bool test_divide_by_10(int32_t number)
{
    return number % 10 == 0;
}

//! @todo use <functional> header to create a function factory and use bind to add the constant in the function created


int main()
{
}