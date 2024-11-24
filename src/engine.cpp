#include <iostream>
#include <string>
#include <vector>
#include "frontend.hpp"
#include "execution_engine.hpp"
int main()
{
    system("cls"); 
    EvaluationWrapper * main_io = new EvaluationWrapper();
    execution_engine * main_exec_engine = new execution_engine();
    
    std::cout <<  "test write data ? ";
    uint64_t data;
    std::cin >> data;
    if (data)
    {
        main_exec_engine->generate_records(data);
    }
    std::cin.ignore();

    while (true)
    {
        std::cout << DEFAULT << DB_PROMPT;
        std::getline(std::cin , InputBuffer);
        AST_NODE * eval_node = main_io->handle(InputBuffer);
        main_exec_engine->execute(eval_node);
    }
    return 0;
}