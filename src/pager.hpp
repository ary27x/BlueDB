#ifndef __PAGER_H
#define __PAGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstring>
#include <algorithm>
#include <sstream>

#include "frontend.hpp"
#include "btree.hpp"

#define PAGE_SIZE 4096
#define PRIMARY_INDEX_DEGREE 100
#define TREE_KEY_UPPER_BOUND 2 * PRIMARY_INDEX_DEGREE - 1
 
struct search_constraint
{
    uint64_t attribute_offset;
    uint64_t read_size;
    TOKEN_SET data_type;
    NODE_SET relational_operation;
    std::string value;
};

struct update_constraint
{
    uint64_t attribute_offset;
    uint64_t operation_size;
    TOKEN_SET data_type;
    std::string new_value;
};

enum
{
    EXEC_SUCCESS,
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

std::unordered_map<TOKEN_SET , uint64_t> token_set_size_converter = {
    {TOKEN_INT_DATA , sizeof(uint64_t)},
    {TOKEN_FLOAT_DATA , sizeof(float)},
    {TOKEN_STRING_DATA , 40}
};

std::unordered_map <char , uint64_t> size_conversion = {
    {'i' , sizeof(uint64_t)},
    {'f' , sizeof(float)},
    {'s' , 40}, // change this behaviour in the parser
};

uint64_t get_attribute_count (std::string &table_schema)
{
    int semi_colon_count = 0;
    for (char current : table_schema)
        if (current == ';')
            semi_colon_count++;
    return semi_colon_count + 1;
}

template <typename datatype>
bool compare_raw_values (datatype& lhs , datatype& rhs , NODE_SET OP_CODE)
{
    switch (OP_CODE)
    {
        case NODE_CONDITION_EQUALS : return lhs == rhs;
        case NODE_CONDITION_GREATER_THAN : return lhs > rhs;
        case NODE_CONDITION_GREATER_THAN_EQUALS : return lhs >= rhs;
        case NODE_CONDITION_LESS_THAN : return lhs < rhs;
        case NODE_CONDITION_LESS_THAN_EQUALS : return lhs <= rhs;
        case NODE_CONDITION_NOT_EQUALS : return lhs != rhs;
    }
    return false;
}

struct page_header
{
    uint64_t page_record_count;
};

struct HeapFile_Metadata
{
    uint64_t total_offset;
    uint64_t page_count;
    uint64_t record_size;
    uint64_t record_count;
    uint64_t write_page_id;
    uint64_t attribute_count;
    uint64_t schema_offset;
    uint64_t primary_key_string_offset;
    std::string schema;
    std::string primary_key_string;
    HeapFile_Metadata(){};
    HeapFile_Metadata(std::string _schema , uint64_t _record_size , std::string _primary_key_string)
    {
        schema = _schema;
        primary_key_string = _primary_key_string;
        schema_offset = _schema.length();
        primary_key_string_offset = _primary_key_string.length();
        record_size = _record_size;
        record_count = 0;
        page_count = 0;
        write_page_id = 0;
        attribute_count = get_attribute_count(_schema);
        total_offset = sizeof(uint64_t) * 8 + schema_offset + primary_key_string_offset;
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
        return status;
    }

    std::string construct_schema(std::vector<AST_NODE *>& table_attribute)
    {
        std::string schema = "";
        for (AST_NODE * current_attribute : table_attribute)
        {
            if (current_attribute->NODE_TYPE == NODE_STRING)
                schema.append(schema_type_conversion[current_attribute->NODE_TYPE]  + *current_attribute->SUB_PAYLOAD+ *current_attribute->PAYLOAD + ';');
            else
                schema.append(schema_type_conversion[current_attribute->NODE_TYPE] + *current_attribute->PAYLOAD + ';');

        }
        schema.pop_back(); // removing the trailing ; to save a byte
        return schema;
    }

    int extract_string_size(std::string table_schema , int offset)
    {
        offset++;
        std::string attribute_size = "";
        while (isdigit(table_schema[offset]))
        {
            attribute_size += table_schema[offset];
            offset++;
        }
        return std::stoi(attribute_size);
    }

    uint64_t get_size(std::string& table_schema)
    {
        uint64_t record_size = 0;
        int iterator_counter = 0;

        if (table_schema[0] == 's')
            record_size += extract_string_size(table_schema , 0);
        else
            record_size += size_conversion[table_schema[0]];
        while (iterator_counter < table_schema.length())
        {
            if (table_schema[iterator_counter] == ';')
            {
                if (table_schema[iterator_counter + 1] == 's')
                    record_size += extract_string_size(table_schema , iterator_counter + 1);
                else
                    record_size += size_conversion[table_schema[iterator_counter + 1]];
            }
            iterator_counter++;
        }

        return record_size;
    }
    
    public :
    std::unordered_map<std::string ,  BTree<key_container<int> , int, PRIMARY_INDEX_DEGREE> *> index_cache;

    Pager() {}

    void load_index_files()
    {
        std::ifstream index_read_stream("bluedb_index_list.txt");
        std::string index_file;
        while (std::getline(index_read_stream , index_file))
        {
            BTree<key_container<int> , int, PRIMARY_INDEX_DEGREE> * current_b_tree_index = new BTree<key_container<int> , int, PRIMARY_INDEX_DEGREE>;
            current_b_tree_index->load(index_file);
            index_cache[index_file] = current_b_tree_index;
        }
        return;
    }

    std::vector<std::string> split_schema(std::string & table_schema)
    {
        std::vector<std::string> schema_chunks;
        std::string construct = "";
        for (char current : table_schema)
        {
            if (current == ';')
            {
                schema_chunks.push_back(construct);
                construct = "";
            }
            else
                construct += current;
        }
        schema_chunks.push_back(construct);
        return schema_chunks;
    }

    void * serialize (std::vector<AST_NODE *>& record , uint64_t record_size , std::vector<std::string>& schema_chunks)
    {
        if (record.size() != schema_chunks.size())
        {
            std::cout << FAIL << "\n[!] ERROR : Record Does Not Follow The Table Schema " << std::endl;
            return nullptr;            
        }
        void * serialized_block = malloc(record_size);
        uint64_t offset_accumalator = 0;
        for (uint64_t itr = 0 ; itr < record.size() ; itr++)
        {
            AST_NODE * current_attribute = record[itr];
            std::string current_schema_chunk = schema_chunks[itr];
            if (schema_type_conversion[current_attribute->NODE_TYPE] != current_schema_chunk[0])
            {
                std::cout << FAIL << "\n[!] ERROR : Datatype Mismatch In Record " << std::endl;
                return nullptr; 
            }
            switch(current_attribute->NODE_TYPE)
            { 
                case NODE_INT :
                {
                    uint64_t type_cast_buffer = std::stoi(*current_attribute->PAYLOAD);
                    memcpy(static_cast <char *> (serialized_block) + offset_accumalator , &type_cast_buffer , sizeof(uint64_t));
                    break;
                }
                case NODE_FLOAT :
                {
                    float type_cast_buffer = std::stof(*current_attribute->PAYLOAD);
                    memcpy(static_cast <char *> (serialized_block) + offset_accumalator , &type_cast_buffer , sizeof(float));
                    break;

                }
                case NODE_STRING :
                {
                    int string_size = get_string_size_from_chunk(current_schema_chunk);
                    std::string padded_string = *current_attribute->PAYLOAD;
                    if (padded_string.size() > string_size) // overflow
                    {
                        uint64_t removal_length = padded_string.size() - string_size;
                        while(--removal_length)
                            padded_string.pop_back(); // trimming the string to handle the overflow 
                    }
                    else if (padded_string.size() < string_size) // underflow
                    {
                        uint64_t required_padding_size = string_size - padded_string.length();
                        for (int i = 0 ; i < required_padding_size ; i++)
                            padded_string.push_back('\0'); // padding the string with null bytes
                    }
                    memcpy(static_cast<char *> (serialized_block) + offset_accumalator , padded_string.data() , string_size);
                    break;
                }

            }
            if (current_attribute->NODE_TYPE == NODE_STRING)
                offset_accumalator += get_string_size_from_chunk(current_schema_chunk);
            else
                offset_accumalator += size_conversion[current_schema_chunk[0]];
        }
        return serialized_block;
    }

    void write_heapfile_metadata(std::ofstream& heap_write_stream , HeapFile_Metadata & required_headers)
    {
        heap_write_stream.seekp(0 , std::ios::beg);

        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.total_offset) , sizeof(uint64_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.page_count) , sizeof(uint64_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.record_count) , sizeof(uint64_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.record_size) , sizeof(uint64_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.write_page_id) , sizeof(uint64_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.attribute_count) , sizeof(uint64_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.schema_offset) , sizeof(uint64_t));

        // attaching the dynamic schema to the header
        heap_write_stream.write(required_headers.schema.c_str() , required_headers.schema_offset);
        
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.primary_key_string_offset) , sizeof(uint64_t));
        heap_write_stream.write(required_headers.primary_key_string.c_str() , required_headers.primary_key_string_offset);

    }

    void update_heapfile_metadata(std::fstream& heap_write_stream , HeapFile_Metadata & required_headers)
    {
        heap_write_stream.seekp(0 , std::ios::beg);

        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.total_offset) , sizeof(uint64_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.page_count) , sizeof(uint64_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.record_count) , sizeof(uint64_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.record_size) , sizeof(uint64_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.write_page_id) , sizeof(uint64_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.attribute_count) , sizeof(uint64_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.schema_offset) , sizeof(uint64_t));

        // attaching the dynamic schema to the header
        heap_write_stream.write(required_headers.schema.c_str() , required_headers.schema_offset);

        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.primary_key_string_offset) , sizeof(uint64_t));
        heap_write_stream.write(required_headers.primary_key_string.c_str() , required_headers.primary_key_string_offset);

    }

    void update_page_to_disk(std::fstream& heap_write_stream , void * new_page_block , uint64_t record_count , uint64_t page_number , uint64_t total_offset)
    {
        memcpy(new_page_block , &record_count , sizeof(uint64_t)); // first we are copying the new header
        heap_write_stream.seekg(total_offset + (page_number - 1) * PAGE_SIZE , std::ios::beg); // setting up the pointer
        heap_write_stream.write(reinterpret_cast <char *> (new_page_block) , PAGE_SIZE);
    }

    uint64_t get_page_record_count (void * page_block)
    {
        uint64_t page_record_count;
        memcpy(&page_record_count , page_block , sizeof(uint64_t));
        return page_record_count;
    }

    int get_primary_index_operation_size(std::vector<std::string> schema_chunks , std::string primary_key_string)
    {
        for (int i = 0 ; i < primary_key_string.size() ; i++)
        {
            if (primary_key_string[i] == '1')
            {
                std::string primary_chunk = schema_chunks[i];
                if (primary_chunk[0] == 's')
                    return get_string_size_from_chunk(primary_chunk);
                else 
                    return size_conversion[primary_chunk[0]];
            }
        }

        return -1;
    }
    
    bool add_to_heap(AST_NODE *& action_node)
    {
        if (!heap_file_exists(action_node->PAYLOAD))
        {
            std::cout << FAIL << "\n[!] ERROR : The Given Table Does Not Exist : " << *action_node->PAYLOAD << std::endl;
            return false;
        }
       
        std::ifstream heap_reader_stream(*action_node->PAYLOAD + ".dat" , std::ios::binary);
        HeapFile_Metadata current_heap_metadata =  deserialize_heapfile_metadata<std::ifstream>(heap_reader_stream);
        heap_reader_stream.close();

        std::vector<std::string> schema_chunks = split_schema(current_heap_metadata.schema);
        uint64_t max_record_per_page = (PAGE_SIZE - sizeof(page_header)) / current_heap_metadata.record_size;

        std::fstream heap_write_stream(*action_node->PAYLOAD + ".dat" , std::ios::in | std::ios::out | std::ios::binary );

        bool isIndexed = false;
        std::string primary_index_name = "";
        std::string primary_index_stream_name = "";
        int primary_index_offset = 0;
        int primary_index_operation_size = 0;
        BTree<key_container<int> , int, PRIMARY_INDEX_DEGREE> * index_tree;
        



        primary_index_name = get_primary_index_name(current_heap_metadata.primary_key_string , schema_chunks);
        if (primary_index_name != "")
        {
            isIndexed = true;
            primary_index_stream_name = *action_node->PAYLOAD + ".index." + primary_index_name + ".idx";
            if (index_cache.find(primary_index_stream_name) == index_cache.end())
            {
                std::cout << FAIL << "\n[~] Index Tree Is Still Loading , Please Try After A While " << *action_node->PAYLOAD << std::endl;
                return false;
            }
            primary_index_offset = get_attribute_offset(&primary_index_name , schema_chunks);
            primary_index_operation_size = get_primary_index_operation_size(schema_chunks , current_heap_metadata.primary_key_string);
            index_tree = index_cache[primary_index_stream_name];
        }
        

        void * page_read_buffer = malloc(PAGE_SIZE);
        heap_write_stream.seekp(current_heap_metadata.total_offset + (current_heap_metadata.page_count - 1) * PAGE_SIZE , std::ios::beg);
        heap_write_stream.read(reinterpret_cast <char *> (page_read_buffer) , PAGE_SIZE);
        uint64_t page_record_count = get_page_record_count(page_read_buffer);

        for (std::vector<AST_NODE *> record : action_node->MULTI_DATA)
        {
            if (page_record_count == max_record_per_page)
            {
                current_heap_metadata.write_page_id++;
                update_page_to_disk(heap_write_stream , page_read_buffer , page_record_count , current_heap_metadata.page_count , current_heap_metadata.total_offset);
                create_new_page<std::fstream> (heap_write_stream , current_heap_metadata);
                heap_write_stream.seekp(current_heap_metadata.total_offset + (current_heap_metadata.page_count - 1) * PAGE_SIZE , std::ios::beg);
                heap_write_stream.read(reinterpret_cast <char *> (page_read_buffer) , PAGE_SIZE);
                page_record_count = get_page_record_count(page_read_buffer);
            }

            void * serialized_block = serialize(record , current_heap_metadata.record_size , schema_chunks);
            if (serialized_block == nullptr)
                return false;
            uint64_t write_offset = sizeof(page_header) + (page_record_count * current_heap_metadata.record_size);
            memcpy(reinterpret_cast <char *> (page_read_buffer) + write_offset , serialized_block , current_heap_metadata.record_size);
            
            
            if (isIndexed) // this means that we need to push the record
            {
                // this is assuming that the key is always of integer type , 
                // COULD CHANGE THIS BEHAVIOUR TO ALSO ALLOW STRING TYPE PRIMARY INDEXING AND FLOAT TYPE PRIMARY INDEXING
                uint64_t key_value; 
                uint64_t total_offset;
                memcpy(&key_value , reinterpret_cast<char *> (serialized_block) + primary_index_offset , primary_index_operation_size);
                total_offset = (current_heap_metadata.write_page_id * PAGE_SIZE) + sizeof(page_header) + (current_heap_metadata.record_size * page_record_count);
                index_tree->insert(key_container<int> (key_value , total_offset));
            }
            
            

            current_heap_metadata.record_count++;
            page_record_count++;
        }

        heap_write_stream.seekp(0 , std::ios::beg); // resetting the pointer to the start of the file
        // to write back the updated heap file metadata
        update_heapfile_metadata(heap_write_stream , current_heap_metadata);
        // update the page count
        update_page_to_disk(heap_write_stream , page_read_buffer , page_record_count , current_heap_metadata.page_count , current_heap_metadata.total_offset);
        heap_write_stream.close();
        free(page_read_buffer);

        if (isIndexed) // writing back the index file changes
            index_tree->disk_serialize(primary_index_stream_name);

        return true;
    }

    template <typename stream_type>
    bool create_new_page(stream_type& heap_write_stream , HeapFile_Metadata & current_meta)
    {
        void * new_page_block = malloc (PAGE_SIZE);
        uint64_t start_record_count = 0;
        memcpy(new_page_block , &start_record_count , sizeof(uint64_t));
        heap_write_stream.write(reinterpret_cast <char *> (new_page_block) , PAGE_SIZE);
        current_meta.page_count++;
        free(new_page_block);

        return true;
    }

    std::string construct_primary_key_string(std::string table_schema , AST_NODE *& action_node)
    {
        std::string primary_key_string = "";
        for (AST_NODE *& current_attribute : action_node->CHILDREN)
        {
            if (current_attribute->isPrimary)   
                primary_key_string += "1";
            else
                primary_key_string += "0";
        }
        return primary_key_string;
    }

    std::string get_primary_index_stream_name(std::string pre_padding , std::string primary_key_string , std::vector<std::string> schema_chunks)
    {
        std::string full_index_name = "";
        for (int i = 0 ; i < primary_key_string.size() ; i++)
        {
            if (primary_key_string[i] == '1')
            {
                std::string primary_index_name;
                if (schema_chunks[i][0] == 's')
                    primary_index_name = get_string_name_from_chunk(schema_chunks[i]);
                else
                    primary_index_name = schema_chunks[i].substr(1);
                
                full_index_name = pre_padding + ".index." + primary_index_name;
                break;
            }
        }
        return full_index_name;
    }

    std::string get_primary_index_name(std::string primary_key_string , std::vector<std::string> schema_chunks)
    {
        for (int i = 0 ; i < primary_key_string.size() ; i++)
        {
            if (primary_key_string[i] == '1')
            {
                std::string primary_index_name;
                if (schema_chunks[i][0] == 's')
                    primary_index_name = get_string_name_from_chunk(schema_chunks[i]);
                else
                    primary_index_name = schema_chunks[i].substr(1);
                
                return primary_index_name;
                break;
            }
        }
        return "";
    }
    
    bool create_new_heap(AST_NODE *& action_node)
    {
        if (heap_file_exists(action_node->PAYLOAD))
            return false;
        
        std::string table_schema = construct_schema(action_node->CHILDREN);
        std::vector<std::string> schema_chunks = split_schema(table_schema);
        uint64_t record_size = get_size(table_schema);

        std::string primary_key_string = construct_primary_key_string(table_schema , action_node);

        HeapFile_Metadata current_metadata (table_schema , record_size , primary_key_string);

        std::ofstream heap_write_stream(*action_node->PAYLOAD + ".dat" , std::ios::binary);
        // writing the heap file header to the file
        write_heapfile_metadata(heap_write_stream , current_metadata);
        create_new_page<std::ofstream>(heap_write_stream , current_metadata);
        write_heapfile_metadata(heap_write_stream , current_metadata); // writing the updated version of hte heapfile meta
        heap_write_stream.close();

        std::string primary_index_name = get_primary_index_stream_name(*action_node->PAYLOAD , primary_key_string , schema_chunks);
        if (primary_index_name != "") // this means that the table has a primary index
        {
            // just assuming the that the btree is of type INT , change this behavious
            BTree<key_container<int> , int, PRIMARY_INDEX_DEGREE> index_tree(sizeof(int));
            index_tree.disk_serialize(primary_index_name + ".idx");
            // writing the name of the index tree to the index list in the reverse order
            std::ifstream index_read_stream("bluedb_index_list.txt");
            std::stringstream file_buffer;
            file_buffer << index_read_stream.rdbuf();
            index_read_stream.close();


            std::ofstream index_write_stream("bluedb_index_list.txt" , std::ios::trunc);
            index_write_stream << primary_index_name << ".idx"<< "\n";
            index_write_stream << file_buffer.str();
            index_write_stream.close();

            // load and save the btree in the index list vector

            BTree<key_container<int> , int, PRIMARY_INDEX_DEGREE> * current_b_tree_index = new BTree<key_container<int> , int, PRIMARY_INDEX_DEGREE>;
            std::string index_stream_name = primary_index_name + ".idx";
            current_b_tree_index->load(index_stream_name);
            index_cache[index_stream_name] = current_b_tree_index;
            
        }
        return true;
    }

    template <typename stream_type>
    HeapFile_Metadata deserialize_heapfile_metadata (stream_type& heap_read_stream)
    {
        HeapFile_Metadata heapfile_headers;
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.total_offset) , sizeof(uint64_t));
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.page_count) , sizeof(uint64_t));
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.record_count) , sizeof(uint64_t));
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.record_size) , sizeof(uint64_t));
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.write_page_id) , sizeof(uint64_t));
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.attribute_count) , sizeof(uint64_t));
        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.schema_offset) , sizeof(uint64_t));

        char * schema_buffer_pointer = (char *)malloc (heapfile_headers.schema_offset + 1);
        heap_read_stream.read(schema_buffer_pointer ,heapfile_headers.schema_offset);
        schema_buffer_pointer[heapfile_headers.schema_offset] = '\0';

        heapfile_headers.schema.assign(schema_buffer_pointer);
        free(schema_buffer_pointer);

        heap_read_stream.read(reinterpret_cast <char *> (&heapfile_headers.primary_key_string_offset) , sizeof(uint64_t));

        char * primary_key_string_buffer_pointer = (char *)malloc (heapfile_headers.primary_key_string_offset + 1);
        heap_read_stream.read(primary_key_string_buffer_pointer ,heapfile_headers.primary_key_string_offset);
        primary_key_string_buffer_pointer[heapfile_headers.primary_key_string_offset] = '\0';

        heapfile_headers.primary_key_string.assign(primary_key_string_buffer_pointer);
        free(primary_key_string_buffer_pointer);

        return heapfile_headers;
    }

    uint64_t get_attribute_offset (std::string * attribute , std::vector<std::string> & schema_chunks)
    {
        uint64_t attribute_offset = 0;
        for (const std::string & chunk : schema_chunks)
        {
            std::string current_attribute_name = "";
            if (chunk[0] == 's')
                current_attribute_name = get_string_name_from_chunk(chunk);
            else 
                current_attribute_name = chunk.substr(1);

            if (*attribute == current_attribute_name)
                return attribute_offset;
            else
            {
                if (chunk[0] == 's')
                    attribute_offset += get_string_size_from_chunk(chunk);
                else
                    attribute_offset += size_conversion[chunk[0]];

            }
        }
        std::cout << FAIL << "\n[!] ERROR : The Following Attribute Does Not Exist : " << *attribute << std::endl;
        return -1;
    }

    int get_string_size_from_chunk(std::string chunk)
    {
        std::string attribute_size = "";
        int offset = 1;
        while (isdigit(chunk[offset]))
        {
            attribute_size += chunk[offset];
            offset++;
        }
        return std::stoi(attribute_size);
    }

    std::string get_string_name_from_chunk(std::string chunk)
    {
        std::string attribute_name = "";
        int offset = 1;
        while (isdigit(chunk[offset]))
            offset++;
        while (offset < chunk.size())
        {
            attribute_name += chunk[offset];
            offset++;
        }
        return attribute_name;
    }

    int get_read_size(TOKEN_SET token_type , AST_NODE *& current_condition_node , std::vector<std::string> & schema_chunks)
    {
        if (token_type != TOKEN_STRING_DATA)
            return token_set_size_converter[token_type];
        // our job here becomes a bit too complex
        for (std::string attribute : schema_chunks)
        {
            if (attribute[0] == 's')
            {
                int attribute_size = get_string_size_from_chunk(attribute);
                std::string attribute_name = get_string_name_from_chunk(attribute);
                if (attribute_name == *current_condition_node->PAYLOAD)
                    return attribute_size;
            }
        }
        return -1;
    }

    std::string get_search_string(AST_NODE *& condition_node , std::vector<std::string> & schema_chunks)
    {
        std::string search_string = "";
        std::vector<std::string> condition_attribute_vector;
        
        for (AST_NODE * current_condition : condition_node->CHILDREN)
            condition_attribute_vector.push_back(*current_condition->PAYLOAD);
        
        for (int i = 0 ; i < schema_chunks.size() ; i++)
        {
            std::string table_attribute;
            if (schema_chunks[i][0] == 's')
                table_attribute = get_string_name_from_chunk(schema_chunks[i]);
            else
                table_attribute = schema_chunks[i].substr(1);
            
            if (std::find(condition_attribute_vector.begin() , condition_attribute_vector.end() , table_attribute) == condition_attribute_vector.end())
                search_string += "0";
            else
                search_string += "1";
        }
        return search_string;
    }

    std::vector<search_constraint> get_search_constraints (AST_NODE *& condition_node , std::vector<std::string> & schema_chunks)
    {
        std::vector<search_constraint> search_constraints;
        AST_NODE * buffer = condition_node->CHILDREN[0];
        for (AST_NODE * current_condition : condition_node->CHILDREN)
        {
            search_constraint new_constraint;

            new_constraint.attribute_offset = get_attribute_offset(current_condition->PAYLOAD , schema_chunks);
            if (new_constraint.attribute_offset == -1)
                return {};
            new_constraint.relational_operation = current_condition->NODE_TYPE;
            new_constraint.data_type = current_condition->HELPER_TOKEN;
            new_constraint.value = *current_condition->SUB_PAYLOAD;
            new_constraint.read_size = get_read_size(current_condition->HELPER_TOKEN , current_condition , schema_chunks);

            search_constraints.push_back(new_constraint);
        }
        return search_constraints;
   }

   std::vector<update_constraint> get_update_constraints(std::vector<AST_NODE *>& update_vector_node , std::vector<std::string> & schema_chunks)
   {
       std::vector<update_constraint> update_vector;
       for (AST_NODE *& update_value_nodes : update_vector_node)
        {
            update_constraint new_update_constraint;

            new_update_constraint.attribute_offset = get_attribute_offset(update_value_nodes->PAYLOAD , schema_chunks);
            if (new_update_constraint.attribute_offset == -1)
                return {};
            new_update_constraint.data_type = update_value_nodes->HELPER_TOKEN;
            new_update_constraint.new_value = *update_value_nodes->SUB_PAYLOAD;
            new_update_constraint.operation_size = get_read_size(update_value_nodes->HELPER_TOKEN , update_value_nodes , schema_chunks);


            update_vector.push_back(new_update_constraint);
        }
        return update_vector;
   }

    bool match_search_constraints(std::vector<search_constraint> & data_constraints , void * page_block , uint64_t record_count , uint64_t record_size)
    {
        uint64_t record_offset = sizeof(page_header) + (record_count - 1) * record_size;
        for (const search_constraint& current_search_constraint : data_constraints)
        {
            switch (current_search_constraint.data_type) // write better code for this using function templates
            {
                case TOKEN_INT_DATA :
                {
                    uint64_t read_buffer;
                    memcpy(&read_buffer , reinterpret_cast <char *> (page_block) + (record_offset + current_search_constraint.attribute_offset) , current_search_constraint.read_size);
                    uint64_t compare_value = std::stoi(current_search_constraint.value);
                    if (!compare_raw_values<uint64_t>(read_buffer , compare_value , current_search_constraint.relational_operation))
                        return false;
                    break;
                }
                case TOKEN_FLOAT_DATA :
                {
                    float read_buffer;
                    memcpy(&read_buffer , reinterpret_cast <char *> (page_block) + (record_offset + current_search_constraint.attribute_offset) , current_search_constraint.read_size);
                    float compare_value = std::stof(current_search_constraint.value);
                    if (!compare_raw_values<float>(read_buffer , compare_value , current_search_constraint.relational_operation))
                        return false;
                    break;
                }
                case TOKEN_STRING_DATA :
                {
                    uint64_t current_size = current_search_constraint.read_size;
                    char * read_buffer = (char*) malloc(current_size + 1);
                    memcpy(read_buffer , reinterpret_cast <char *> (page_block) + (record_offset + current_search_constraint.attribute_offset) , current_search_constraint.read_size);
                    read_buffer[current_size] = '\0';

                    std::string buffer_data;
                    buffer_data.assign(read_buffer);
                    std::string rhs_value = current_search_constraint.value;

                    bool comparison_result = compare_raw_values<std::string>(buffer_data , rhs_value , current_search_constraint.relational_operation);
                    free(read_buffer);
                    if (!comparison_result) // the comparison has failed
                        return false;
                    break;
                }
            }
        }
        return true;
    }

    void export_deserialization(std::string table_name , std::ofstream& mysql_write_stream)
    {
        std::ifstream heap_read_stream (table_name + ".dat" , std::ios::binary);
        HeapFile_Metadata current_heapfile_metadata = deserialize_heapfile_metadata<std::ifstream>(heap_read_stream);
        std::vector<std::string> schema_chunks = split_schema(current_heapfile_metadata.schema);

        void * page_read_buffer = malloc(PAGE_SIZE);
        uint64_t global_record_counter = 1;
        for (int page_counter = 1 ; page_counter <= current_heapfile_metadata.page_count ; page_counter++)
        {
            heap_read_stream.read(reinterpret_cast <char *> (page_read_buffer) , PAGE_SIZE);
            uint64_t page_record_count;
            memcpy(&page_record_count , page_read_buffer , sizeof(uint64_t));
            uint64_t read_offset = sizeof(page_header);
            for (int record_itr = 1 ; record_itr <= page_record_count ; record_itr++)
            {
                mysql_write_stream << "(";
                deserialize(page_read_buffer , read_offset , schema_chunks , true , &mysql_write_stream);
                if (global_record_counter == current_heapfile_metadata.record_count)
                    mysql_write_stream << ");\n";
                else
                    mysql_write_stream << "),\n";

                global_record_counter++;
                read_offset += current_heapfile_metadata.record_size;
            }
        }
        heap_read_stream.close();
        
    }
    
    bool get_heap(AST_NODE *& action_node)
    {
        // std::cout << "~"; // denotes we are printing a table for the gui client 
        if (!this->heap_file_exists(&action_node->DATA_LIST[0]))
        {
            std::cout << FAIL << "\n[!] ERROR : The Given Table Does Not Exist : " << action_node->DATA_LIST[0] << std::endl;
            return false;
        }
        std::ifstream heap_read_stream (action_node->DATA_LIST[0] + ".dat" , std::ios::binary);
        HeapFile_Metadata current_heapfile_metadata = deserialize_heapfile_metadata<std::ifstream>(heap_read_stream);

        // std::cout << action_node->DATA_LIST[0] << std::endl;
        
        
        // std::cout << "Table Name : " << action_node->DATA_LIST[0] << std::endl;
        // std::cout << "Total records : " << current_heapfile_metadata.record_count << std::endl;
        // std::cout << "Record Size : " << current_heapfile_metadata.record_size << std::endl;
        // std::cout << "Total number of pages : " << current_heapfile_metadata.page_count << std::endl;
        // std::cout << "primary key string : " << current_heapfile_metadata.primary_key_string << std::endl;
        // std::cout << "current_schema" <<  current_heapfile_metadata.schema << std::endl;


        // std::cout << "this is page write id : " << current_heapfile_metadata.write_page_id << std::endl;
        // // // std::cout << current_heapfile_metadata.schema << std::endl;
        
        uint64_t max_record_per_page = (PAGE_SIZE - sizeof(page_header)) / current_heapfile_metadata.record_size;

        std::vector<std::string> schema_chunks = split_schema(current_heapfile_metadata.schema);
        std::vector<search_constraint> data_constraints;
        std::string search_string = "";
        bool index_lookup = false;

        bool constraint_flag = false;
        if (action_node->CHILD) // condition node is attached
        {
            constraint_flag = true;
            data_constraints = get_search_constraints(action_node->CHILD , schema_chunks);
            if (data_constraints.size() == 0)
                return false;
            search_string = get_search_string(action_node->CHILD , schema_chunks);
            index_lookup = true;
            for (int i = 0 ; i < search_string.size() ; i++)
            {
                if (search_string[i] == '1')
                {
                    if (current_heapfile_metadata.primary_key_string[i] != '1') // if the attribute is not primary 
                    {
                        index_lookup = false;
                        break;
                    }
                }
            }
        }

        bool isIndexed = false;
        std::string primary_index_name = get_primary_index_stream_name(action_node->DATA_LIST[0] , current_heapfile_metadata.primary_key_string , schema_chunks);


        if (primary_index_name != "") 
            isIndexed = true;
        

        std::cout << std::endl;
        std::cout << YELLOW;
        for (std::string attribute : schema_chunks)
        {
            if (attribute[0] == 's')
                std::cout << get_string_name_from_chunk(attribute) << "\t\t";
            else
                std::cout << attribute.substr(1) << "\t\t";
        }
        std::cout << DEFAULT << std::endl;


        if (isIndexed && search_string == current_heapfile_metadata.primary_key_string)
        {

            // just assuming the that the btree is of type INT , change this behavious
            
            if (index_cache.find(primary_index_name + ".idx") == index_cache.end())
            {
                std::cout << "The index file is not loaded yet , please try again later after some time " << std::endl;
                return true;
            }

            BTree<key_container<int> , int, PRIMARY_INDEX_DEGREE> * current_index_tree = index_cache[primary_index_name + ".idx"];

            int search_key_value = std::stoi(data_constraints[0].value);
            
            key_container<int> search_result = current_index_tree->search(current_index_tree->root , key_container<int> (search_key_value , -1));
            if (search_result.main_key != -1)
            {

                uint64_t page_count = search_result.key_offset / PAGE_SIZE;
                int buffer_offset = search_result.key_offset - (page_count * PAGE_SIZE);
                int record_id = (buffer_offset - sizeof(page_header))/current_heapfile_metadata.record_size;

                void * page_read_buffer = malloc(PAGE_SIZE);

                std::streampos offset = static_cast<std::streampos>(PAGE_SIZE) * page_count;
                heap_read_stream.seekg(offset + heap_read_stream.tellg() , std::ios::beg);
                heap_read_stream.read(reinterpret_cast <char *> (page_read_buffer), PAGE_SIZE);
                int read_offset = sizeof(page_header) + (current_heapfile_metadata.record_size * record_id);
                deserialize(page_read_buffer , read_offset , schema_chunks);
                free(page_read_buffer);
                
            }    

            heap_read_stream.close();
            return true;
        }


        void * page_read_buffer = malloc(PAGE_SIZE);
        for (int page_counter = 1 ; page_counter <= current_heapfile_metadata.page_count ; page_counter++)
        {
            heap_read_stream.read(reinterpret_cast <char *> (page_read_buffer) , PAGE_SIZE);
            uint64_t page_record_count;
            memcpy(&page_record_count , page_read_buffer , sizeof(uint64_t));
            uint64_t read_offset = sizeof(page_header);
            for (int record_itr = 1 ; record_itr <= page_record_count ; record_itr++)
            {
                if (constraint_flag)
                {
                    if (match_search_constraints(data_constraints , page_read_buffer , record_itr , current_heapfile_metadata.record_size))
                        deserialize(page_read_buffer , read_offset , schema_chunks);
                }
                else
                    deserialize(page_read_buffer , read_offset , schema_chunks);

                read_offset += current_heapfile_metadata.record_size;
            }
        }
        free (page_read_buffer);
        heap_read_stream.close();
        return true;
    }

    void update_page_record(void * page_block , uint64_t record_count , uint64_t record_size , std::vector<update_constraint> & update_values)
    {
        uint64_t update_base_offset = sizeof(page_header) + ((record_count - 1) * record_size);
        for (update_constraint current_update_constraint : update_values)
        {
            switch(current_update_constraint.data_type)
            {
                case TOKEN_INT_DATA :
                {
                    uint64_t update_value_buffer = std::stoi(current_update_constraint.new_value);
                    memcpy(reinterpret_cast <char *> (page_block) + (update_base_offset + current_update_constraint.attribute_offset) , &update_value_buffer , current_update_constraint.operation_size);
                    break;
                }
                case TOKEN_FLOAT_DATA :
                {
                    float update_value_buffer = std::stof(current_update_constraint.new_value);
                    memcpy(reinterpret_cast <char *> (page_block) + (update_base_offset + current_update_constraint.attribute_offset) , &update_value_buffer , current_update_constraint.operation_size);
                    break;
                }
                case TOKEN_STRING_DATA :
                {
                    std::string padded_string = current_update_constraint.new_value;


                    if (padded_string.size() > current_update_constraint.operation_size) // overflow
                    {
                        uint64_t removal_length = padded_string.size() - current_update_constraint.operation_size;
                        while(--removal_length)
                            padded_string.pop_back(); // trimming the string to handle the overflow 
                    }
                    else if (padded_string.size() < current_update_constraint.operation_size) // underflow
                    {
                        uint64_t required_padding_size = current_update_constraint.operation_size - padded_string.length();
                        for (int i = 0 ; i < required_padding_size ; i++)
                            padded_string.push_back('\0'); // padding the string with null bytes
                    }
                    memcpy(reinterpret_cast <char *> (page_block) + (update_base_offset + current_update_constraint.attribute_offset), padded_string.data() , current_update_constraint.operation_size);
                    break;
                }
            }
        }
    }

    void delete_page_record(void * page_block , uint64_t record_number , uint64_t record_size , uint64_t total_record)
    {
        int64_t deletion_offset = sizeof(page_header) + ((record_number - 1) * record_size);
        for (uint64_t itr = record_number; itr < total_record ; itr++)
        {
            memcpy(reinterpret_cast <char *> (page_block) + (deletion_offset) , reinterpret_cast <char *> (page_block) + (deletion_offset + record_size) , record_size);
            deletion_offset += record_size;
        }
    }
    
    bool delete_from_heap(AST_NODE *& action_node)
    {
        if (!this->heap_file_exists(action_node->PAYLOAD))
        {
            std::cout << FAIL << "\n[!] ERROR : The Given Table Does Not Exist : " << *action_node->PAYLOAD << std::endl;
            return false;
        }

        std::fstream heap_read_stream (*action_node->PAYLOAD + ".dat" , std::ios::in | std::ios::out | std::ios::binary);
        HeapFile_Metadata current_heapfile_metadata = deserialize_heapfile_metadata<std::fstream>(heap_read_stream);

        uint64_t max_record_per_page = (PAGE_SIZE - sizeof(page_header)) / current_heapfile_metadata.record_size;

        std::vector<std::string> schema_chunks = split_schema(current_heapfile_metadata.schema);
        std::vector<search_constraint> data_constraints;
        std::string search_string;
        bool index_lookup = false;

        bool condition_attached = false;
        if (action_node->CHILD) // condition node is attached
        {
            condition_attached = true;
            data_constraints = get_search_constraints(action_node->CHILD , schema_chunks);
            if (data_constraints.size() == 0)
                return false;
            search_string = get_search_string(action_node->CHILD , schema_chunks);
            index_lookup = true;
            for (int i = 0 ; i < search_string.size() ; i++)
            {
                if (search_string[i] == '1')
                {
                    if (current_heapfile_metadata.primary_key_string[i] != '1') // if the attribute is not primary 
                    {
                        index_lookup = false;
                        break;
                    }
                }
            }

        }

        void * page_read_buffer = malloc(PAGE_SIZE);
        for (int page_counter = 1 ; page_counter <= current_heapfile_metadata.page_count ; page_counter++)
        {
            heap_read_stream.read(reinterpret_cast <char *> (page_read_buffer) , PAGE_SIZE);
            uint64_t page_record_count;
            memcpy(&page_record_count , page_read_buffer , sizeof(uint64_t));
            uint64_t read_offset = sizeof(page_header);

            bool changes_made = false;
            uint64_t new_page_record_count = page_record_count;
            for (int record_itr = 1 ; record_itr <= new_page_record_count ;)
            {
                if (condition_attached)
                {
                    if (match_search_constraints(data_constraints , page_read_buffer , record_itr , current_heapfile_metadata.record_size))
                    {
                        delete_page_record(page_read_buffer , record_itr , current_heapfile_metadata.record_size , new_page_record_count);
                        changes_made = true;
                        new_page_record_count--;
                        current_heapfile_metadata.record_count--;
                        continue; //
                    }
                    // else
                    //     read_offset += current_heapfile_metadata.record_size;
                }
                else // unconditional deletion
                {
                    delete_page_record(page_read_buffer , record_itr , current_heapfile_metadata.record_size , new_page_record_count);
                    changes_made = true;
                    new_page_record_count--;
                    current_heapfile_metadata.record_count--;
                    continue;
                }
                record_itr++;
            }
            if (changes_made)
            {
                uint64_t stream_offset_copy = heap_read_stream.tellg();
                update_page_to_disk(heap_read_stream , page_read_buffer , new_page_record_count , page_counter , current_heapfile_metadata.total_offset);
                heap_read_stream.seekg(stream_offset_copy , std::ios::beg);
            }
        }

        update_heapfile_metadata(heap_read_stream , current_heapfile_metadata);
        heap_read_stream.close();
        return true;
    }

    bool update_heap(AST_NODE *& action_node)
    {
        if (!this->heap_file_exists(action_node->PAYLOAD))
        {
            std::cout << FAIL << "\n[!] ERROR : The Given Table Does Not Exist : " << *action_node->PAYLOAD << std::endl;
            return false;
        }

        std::fstream heap_read_stream (*action_node->PAYLOAD + ".dat" , std::ios::in | std::ios::out | std::ios::binary);
        HeapFile_Metadata current_heapfile_metadata = deserialize_heapfile_metadata<std::fstream>(heap_read_stream);

        uint64_t max_record_per_page = (PAGE_SIZE - sizeof(page_header)) / current_heapfile_metadata.record_size;

        std::vector<std::string> schema_chunks = split_schema(current_heapfile_metadata.schema);
        std::vector<search_constraint> data_constraints;
        std::string search_string;
        bool index_lookup = false;


        bool condition_attached = false;
        if (action_node->CHILD) // condition node is attached
        {
            condition_attached = true;
            data_constraints = get_search_constraints(action_node->CHILD , schema_chunks);
            if (data_constraints.size() == 0)
                return false;
            search_string = get_search_string(action_node->CHILD , schema_chunks);
            index_lookup = true;
            for (int i = 0 ; i < search_string.size() ; i++)
            {
                if (search_string[i] == '1')
                {
                    if (current_heapfile_metadata.primary_key_string[i] != '1') // if the attribute is not primary 
                    {
                        index_lookup = false;
                        break;
                    }
                }
            }
        }
        std::vector<update_constraint> update_values = get_update_constraints(action_node->CHILDREN , schema_chunks);
        if (update_values.size() == 0)
            return false; 

        void * page_read_buffer = malloc(PAGE_SIZE);
        for (int page_counter = 1 ; page_counter <= current_heapfile_metadata.page_count ; page_counter++)
        {
            heap_read_stream.read(reinterpret_cast <char *> (page_read_buffer) , PAGE_SIZE);
            uint64_t page_record_count;
            memcpy(&page_record_count , page_read_buffer , sizeof(uint64_t));
            uint64_t read_offset = sizeof(page_header);

            bool changes_made = false;
            for (int record_itr = 1 ; record_itr <= page_record_count ; record_itr++)
            {
                if (condition_attached)
                {
                    if (match_search_constraints(data_constraints , page_read_buffer , record_itr , current_heapfile_metadata.record_size))
                    {
                        update_page_record(page_read_buffer , record_itr , current_heapfile_metadata.record_size , update_values);
                        changes_made = true;
                    }
                }
                else
                {
                    update_page_record(page_read_buffer , record_itr , current_heapfile_metadata.record_size , update_values);
                    changes_made = true;
                }
                read_offset += current_heapfile_metadata.record_size;
            }
            if (changes_made)
            {
                uint64_t stream_offset_copy = heap_read_stream.tellg();
                update_page_to_disk(heap_read_stream , page_read_buffer , page_record_count , page_counter , current_heapfile_metadata.total_offset);
                heap_read_stream.seekg(stream_offset_copy , std::ios::beg);
            }
        }
        heap_read_stream.close();
        return true;
    }

    void deserialize(void * page_block , uint64_t read_offset ,  std::vector<std::string>& schema_chunks , bool exporting = false , std::ofstream * export_stream = nullptr)
    {
        uint64_t attribute_read_offset = read_offset;
        int counter = 0;
        for (std::string attribute : schema_chunks)
        {
            switch(attribute[0])
            {
                case 'i':
                {
                    uint64_t read_buffer;
                    memcpy(&read_buffer , reinterpret_cast <char *> (page_block) + attribute_read_offset , sizeof(uint64_t));
                    attribute_read_offset += sizeof(uint64_t);
                    if (exporting)
                    {
                        *export_stream << read_buffer;
                        if (counter != schema_chunks.size() - 1)
                            *export_stream << ", ";
                    }
                    else
                        std::cout << read_buffer << "\t\t";
                    break;
                }
                case 'f':
                {
                    float read_buffer;
                    memcpy(&read_buffer , reinterpret_cast <char *> (page_block) + attribute_read_offset , sizeof(float));
                    attribute_read_offset += sizeof(float);
                    if (exporting)
                    {
                        *export_stream << read_buffer;
                        if (counter != schema_chunks.size() - 1)
                            *export_stream << ", ";
                    }
                    else
                        std::cout << read_buffer << "\t\t";
                    break;
                }
                case 's':
                {
                    uint64_t current_size = get_string_size_from_chunk(attribute);
                    char * read_buffer = (char*) malloc(current_size + 1);
                    memcpy(read_buffer , reinterpret_cast <char *> (page_block) + attribute_read_offset , current_size);
                    attribute_read_offset += current_size;
                    read_buffer[current_size] = '\0';
                    if (exporting)
                    {
                        *export_stream << "\'" << read_buffer << "\'";
                        if (counter != schema_chunks.size() - 1)
                            *export_stream << ", ";
                    }
                    else
                        std::cout << read_buffer << "\t\t";
                    free(read_buffer);
                    break;
                }
            }
            counter++;
        }
        if (!exporting)
            std::cout << std::endl;
    }

};

#endif
