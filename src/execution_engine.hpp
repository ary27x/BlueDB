
#ifndef __EXECUTION_ENGINE_H
#define __EXECUTION_ENGINE_H

#include <iostream>
#include <string>
#include "frontend.hpp"
#include "pager.hpp"
#include "lru_cache.hpp"


enum 
{
    ERROR_TABLE_NOT_FOUND,
    ERROR_ATTRIBUTE_MISMATCH,
    ERROR_ATTRIBUTE_UNDERFLOW, 
    ERROR_ATTRIBUTE_OVERFLOW, 
} EXEC_CODE;


class execution_engine
{
    private :
    Pager * main_pager;
    public :
    execution_engine()
    {
        main_pager = new Pager();
    } 

    bool handle_new(AST_NODE *& action_node)
    {
        // to the handle checking
        // if the name of the table already exists, then one should 
        // return false from here
        // else then proceed with teh main_pager create nmew haedp 
        bool status = main_pager->create_new_heap(action_node);
        if (!status)
            std::cout << "Error , the given table already exists in the table ! " << std::endl;
        else 
            std::cout << "the data was written : " << std::endl;
    }
    bool handle_add(AST_NODE *& action_node)
    {

        bool status = main_pager->add_to_heap(action_node);
        return true;

    }
    bool handle_print(AST_NODE *& action_node)
    {
        bool status = main_pager->get_heap(action_node);
        if (!status)
            std::cout << "Error , the given table does not exist in the table ! " << std::endl;

    }
    bool handle_update(AST_NODE *& action_node)
    {

    }
    bool handle_remove(AST_NODE *& action_node)
    {

    }

    bool handle_exit(AST_NODE *& action_node)
    {

    }
    

    bool execute (AST_NODE *& action_node)
    {
        switch(action_node->NODE_TYPE)
        {
            case NODE_NEW    : return this->handle_new(action_node); 
            case NODE_ADD    : return this->handle_add(action_node); 
            case NODE_PRINT  : return this->handle_print(action_node); 
            case NODE_UPDATE : return this->handle_update(action_node); 
            case NODE_REMOVE : return this->handle_remove(action_node);
            case NODE_EXIT   : return this->handle_exit(action_node); 
        }
    }
};

#endif