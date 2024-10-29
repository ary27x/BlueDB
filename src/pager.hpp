#ifndef __PAGER_H
#define __PAGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <string.h>
#include "frontend.hpp"
#include "execution_engine.hpp"
#define PAGE_SIZE 4096



enum 
{
    ERROR_TABLE_NOT_FOUND,
    ERROR_ATTRIBUTE_MISMATCH,
    ERROR_ATTRIBUTE_UNDERFLOW, 
    ERROR_ATTRIBUTE_OVERFLOW, 
} EXEC_CODE;


std::unordered_map <NODE_SET , char> schema_type_conversion = {
    {NODE_INT    , 'i'},
    {NODE_FLOAT  , 'f'},
    {NODE_STRING , 's'},
};

std::unordered_map <char , uint32_t> size_conversion = {
    {'i' , sizeof(int)},
    {'f' , sizeof(float)},
    {'s' , 10}, // change this behaviour in the parser
};


uint32_t get_attribute_count (std::string &table_schema)
{
    int semi_colon_count = 0;
    for (char current : table_schema)
        if (current == ';')
            semi_colon_count++;
    return semi_colon_count + 1;
}




struct Page
{

};

struct HeapFile_Metadata
{
    uint32_t total_offset;
    uint32_t page_count;
    uint32_t record_size;
    uint32_t record_count;
    uint32_t write_page_id;
    uint32_t attribute_count;
    uint32_t schema_offset;
    std::string schema;
    HeapFile_Metadata(){};
    HeapFile_Metadata(std::string _schema , uint32_t _record_size)
    {
        schema = _schema;
        schema_offset = _schema.length();
        record_size = _record_size;
        record_count = 0;
        page_count = 0;
        write_page_id = 0;
        attribute_count = get_attribute_count(_schema);
        total_offset = sizeof(uint32_t) * 6 + schema_offset;  
    }
    
};





class Pager
{
    private :
    bool heap_file_exists(std::string* table_name)
    {
        std::ifstream test_stream;
        std::string heap_name = *table_name + ".dat";
        test_stream.open(heap_name);
        bool status = test_stream.good();
        test_stream.close();
        std::cout << "this is the status : " << status << std::endl;
        return status;
    } 

    std::string construct_schema(std::vector<AST_NODE *>& table_attribute)
    {
        std::string schema = "";
        for (AST_NODE * current_attribute : table_attribute)
            schema.append(schema_type_conversion[current_attribute->NODE_TYPE] + *current_attribute->PAYLOAD + ';');
        schema.pop_back(); // removing the trailing ; to save a byte
        return schema;
    }

    

    uint32_t get_size(std::string& table_schema)
    {
        uint32_t record_size = 0;
        int iterator_counter = 0;
        record_size += size_conversion[table_schema[0]];
        while (iterator_counter < table_schema.length())
        {
            if (table_schema[iterator_counter] == ';')
                record_size += size_conversion[table_schema[iterator_counter + 1]];
            iterator_counter++; 
        }
        return record_size;
    }
    public :
    Pager()
    {

    }
    bool add_to_heap(AST_NODE *& action_node)
    {
        for (std::vector<AST_NODE *> records : action_node->MULTI_DATA)
        {
            std::cout << "record start : " << std::endl;
            for (AST_NODE * buffer : records)
            {
                std::cout << "type : " << nodeTypeToString(buffer->NODE_TYPE);
                std::cout << " value :  " << *buffer->PAYLOAD << std::endl;
            }
        }
        return true;
    }
    bool create_new_heap(AST_NODE *& action_node)
    {
        // working on the same database , change the behaviour for multi db
        std::cout << "this is the start of the create new heap function : " << std::endl;
        if (heap_file_exists(action_node->PAYLOAD))
            return false;
        std::cout << "creating the new heap : " << std::endl;
        std::string table_schema = construct_schema(action_node->CHILDREN);
        uint32_t record_size = get_size(table_schema);
        HeapFile_Metadata current_metadata (table_schema , record_size);
        
        std::ofstream heap_write_stream(*action_node->PAYLOAD + ".dat" , std::ios::binary);
        // writing the heap file header to the file
        heap_write_stream.write(reinterpret_cast<char *> (&current_metadata.total_offset) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&current_metadata.page_count) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&current_metadata.record_count) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&current_metadata.record_size) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&current_metadata.write_page_id) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&current_metadata.attribute_count) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&current_metadata.schema_offset) , sizeof(uint32_t));

        // attaching the dynamic schema to the header
        heap_write_stream.write(current_metadata.schema.c_str() , current_metadata.schema_offset);
        heap_write_stream.close();

        return true;
    }

    HeapFile_Metadata deserialize_heapfile_metadata (std::ifstream& heap_read_stream)
    {
        HeapFile_Metadata heapfile_headers;
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.total_offset) , sizeof(uint32_t));
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.page_count) , sizeof(uint32_t));
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.record_count) , sizeof(uint32_t));
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.record_size) , sizeof(uint32_t));
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.write_page_id) , sizeof(uint32_t));
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.attribute_count) , sizeof(uint32_t));
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.schema_offset) , sizeof(uint32_t));

        char * schema_buffer_pointer = (char *)malloc (heapfile_headers.schema_offset + 1);
        heap_read_stream.read(schema_buffer_pointer ,heapfile_headers.schema_offset);
        schema_buffer_pointer[heapfile_headers.schema_offset] = '\0';

        heapfile_headers.schema.assign(schema_buffer_pointer);                
        free(schema_buffer_pointer);
        return heapfile_headers;
    }




    bool get_heap(AST_NODE *& action_node)
    {

        if (!this->heap_file_exists(&action_node->DATA_LIST[0]))
            return false;
        std::cout << "[*] Table Name : " << action_node->DATA_LIST[0] << std::endl;
        std::cout << "[*] Heap File : " << action_node->DATA_LIST[0]+ ".dat" << std::endl;
        std::ifstream heap_read_stream (action_node->DATA_LIST[0] + ".dat" , std::ios::binary);

        HeapFile_Metadata current_heapfile_metadata = deserialize_heapfile_metadata(heap_read_stream);
        
        std::cout << "[*] Schema Offset : " << current_heapfile_metadata.schema_offset << std::endl;
        std::cout << "[*] Page Count : " << current_heapfile_metadata.page_count << std::endl;
        std::cout << "[*] Record Count : " << current_heapfile_metadata.record_count <<  std::endl;
        std::cout << "[*] Record Size : " << current_heapfile_metadata.record_size <<  std::endl;
        std::cout << "[*] Write Page ID : " << current_heapfile_metadata.write_page_id <<  std::endl;
        std::cout << "[*] Total Offset : " << current_heapfile_metadata.total_offset <<  std::endl;
        std::cout << "[*] Schema : " << current_heapfile_metadata.schema << std::endl;
        std::cout << "[*] Attribute Count : " << current_heapfile_metadata.attribute_count << std::endl;
        heap_read_stream.close(); 
        return true;
    }

    void serialize()
    {

    }
    void deserialize()
    {

    }



    bool insert_record(std::string table_name , void * record)
    {
        std::ofstream table_heap_file;
        table_heap_file.open(table_name + ".dat" , std::ios::binary);
        

    }
};

#endif