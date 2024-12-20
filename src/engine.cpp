#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include "frontend.hpp"
#include "execution_engine.hpp"

void index_loader(execution_engine *);

int main(int argc , char ** argv)
{
    
    system("title SystemX Engine");
    system("cls"); 
    EvaluationWrapper * main_io = new EvaluationWrapper();
    execution_engine * main_exec_engine = new execution_engine();

    // index_loader(main_exec_engine);

    // uint64_t start = std::stoi(argv[1]);
    // uint64_t stop = std::stoi(argv[2]);
    
    // main_exec_engine->generate_records(start , stop , "main_database.test");

    // exit(0);

    std::thread index_loader_thread(index_loader , main_exec_engine);
    index_loader_thread.detach();

    while (true)
    {
        std::cout << BLUE << DB_PROMPT << DEFAULT;
        std::getline(std::cin , InputBuffer);
        AST_NODE * eval_node = main_io->handle(InputBuffer);
        if (eval_node == nullptr) 
            continue;
        main_exec_engine->execute(eval_node);
    }
 
    return 0;
}

void index_loader(execution_engine * main_exec_engine)
{
    main_exec_engine->main_pager->load_index_files();
}

