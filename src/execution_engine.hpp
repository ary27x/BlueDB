
#ifndef __EXECUTION_ENGINE_H
#define __EXECUTION_ENGINE_H

#include <iostream>
#include <string>
#include "frontend.hpp"
#include "pager.hpp"
#include "lru_cache.hpp"





class execution_engine
{
    private :
    Pager * main_pager;
    public :
    execution_engine()
    {
        main_pager = new Pager();
    }

    void generate_records(uint64_t data)
    {
        std::time_t start_time = std::time(nullptr);
        std::cout << "Start time : " <<  start_time << std::endl;

        std::string * table_name = new std::string("students");
        AST_NODE * action_node = new AST_NODE();
        action_node->PAYLOAD = table_name;

        std::vector<std::vector<AST_NODE *>> multi_data_records;
        uint64_t percentage_counter = 0;
        uint64_t percentage_chunk = data / 100;
        uint64_t counter = 0;
        for (uint64_t itr = 1 ; itr <= data ; itr++)
        {
            counter++;
            if (counter == percentage_chunk)
            {
                counter = 0;
                percentage_counter++;
                std::cout  << "\r" << percentage_counter << "%";

            }

            std::vector<AST_NODE *> buffer_vector;
            // int id , string name , int age
            AST_NODE * id_node = new AST_NODE();
            AST_NODE * name_node = new AST_NODE();
            AST_NODE * age_node = new AST_NODE();


            id_node->NODE_TYPE = NODE_INT;
            name_node->NODE_TYPE = NODE_STRING;
            age_node->NODE_TYPE = NODE_INT;

            std::string * id_value = new std::string(std::to_string(itr));
            std::string * name_value = new std::string("n" + std::to_string(itr));
            std::string * age_value = new std::string(std::to_string(rand() % 100));

            id_node->PAYLOAD = id_value;
            name_node->PAYLOAD = name_value;
            age_node->PAYLOAD = age_value;

            buffer_vector.push_back(id_node);
            buffer_vector.push_back(name_node);
            buffer_vector.push_back(age_node);

            multi_data_records.push_back(buffer_vector);
        }
        action_node->MULTI_DATA = multi_data_records;
        main_pager->add_to_heap(action_node);


        std::time_t end_time = std::time(nullptr);
        std::cout << "End time : " <<  end_time << std::endl;

        std::cout << "Time taken : " << end_time - start_time << " seconds "<< std::endl;

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
        std::time_t start_time = std::time(nullptr);

        bool status = main_pager->get_heap(action_node);
        if (!status)
            std::cout << "Error , the given table does not exist in the table ! " << std::endl;

        std::time_t end_time = std::time(nullptr);

        std::cout << "[*] Time taken : " << end_time - start_time << " seconds "<< std::endl;

    }
    bool handle_update(AST_NODE *& action_node)
    {

        std::cout << "this is from the new update block  : " << std::endl;
        /*
        update students :: id == 2 -> name = "KUMAR"
        */
        bool status = main_pager->update_heap(action_node);
        if (!status)
            std::cout << "There was some error while trying to update the database";

    }
    bool handle_remove(AST_NODE *& action_node)
    {
        main_pager->delete_from_heap(action_node);
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
