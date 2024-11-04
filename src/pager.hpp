
#ifndef __PAGER_H
#define __PAGER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstring>
#include "frontend.hpp"
#define PAGE_SIZE 4096



struct search_constraint
{
    uint32_t attribute_offset;
    uint32_t read_size;
    TOKEN_SET data_type;
    NODE_SET relational_operation;
    std::string value;
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

std::unordered_map<TOKEN_SET , uint32_t> token_set_size_converter = {
    {TOKEN_INT_DATA , sizeof(uint64_t)},
    {TOKEN_FLOAT_DATA , sizeof(float)},
    {TOKEN_STRING_DATA , 20}
};

std::unordered_map <char , uint32_t> size_conversion = {
    {'i' , sizeof(uint64_t)},
    {'f' , sizeof(float)},
    {'s' , 20}, // change this behaviour in the parser
};


uint32_t get_attribute_count (std::string &table_schema)
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
}



struct page_header
{
    uint16_t page_record_count;
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
        total_offset = sizeof(uint32_t) * 7 + schema_offset;  
    }
    
};

class Pager
{
    private :


    void partial_deserialization();

    bool heap_file_exists(std::string* table_name)
    {
        std::ifstream test_stream;
        std::string heap_name = *table_name + ".dat";
        test_stream.open(heap_name);
        bool status = test_stream.good();
        test_stream.close();
        return status;
    } 

    void render_constraint(search_constraint & current_constraint)
    {
    std::cout << "offset : " << current_constraint.attribute_offset << std::endl;
    std::cout << " read size : " << current_constraint.read_size << std::endl;
    std::cout << " data_type : " << tokenTypeToString(current_constraint.data_type)  << std::endl;
    std::cout << " relational operator type : " << nodeTypeToString(current_constraint.relational_operation)  << std::endl;
    std::cout << " value : " << current_constraint.value << std::endl;
    std::cout << "-----------------------------" << std::endl;
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

    void * serialize (std::vector<AST_NODE *>& record , uint32_t record_size , std::vector<std::string>& schema_chunks) 
    {
        if (record.size() != schema_chunks.size())
        {
            // THIS ERROR IS SOMEHOW CORRUPTING THE HEAP HEADERS 
            // FIX THIS
            std::cout << "Attribute Size Error ! could not insert the data : " << std::endl;
            exit(1);
        }
        void * serialized_block = malloc(record_size);
        uint32_t offset_accumalator = 0;
        for (int itr = 0 ; itr < record.size() ; itr++)
        {
            AST_NODE * current_attribute = record[itr];
            std::string current_schema_chunk = schema_chunks[itr];
            if (schema_type_conversion[current_attribute->NODE_TYPE] != current_schema_chunk[0])
            {
                // THIS ERROR IS SOMEHOW CORRUPTING THE HEAP HEADERS 
                // FIX THIS
                std::cout << "Attribute mismatch error ! " << std::endl; 
                exit(1);
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
                    std::string padded_string = *current_attribute->PAYLOAD;
                    uint32_t required_padding_size = size_conversion[current_schema_chunk[0]] - padded_string.length(); 
                    for (int i = 0 ; i < required_padding_size ; i++)
                        padded_string.push_back('\0');
                    memcpy(static_cast<char *> (serialized_block) + offset_accumalator , padded_string.data() , size_conversion[current_schema_chunk[0]]);
                    break;
                } 

            }
            offset_accumalator += size_conversion[current_schema_chunk[0]];
        }
        return serialized_block;
    }

    void write_heapfile_metadata(std::ofstream& heap_write_stream , HeapFile_Metadata & required_headers)
    {
        heap_write_stream.seekp(0 , std::ios::beg);

        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.total_offset) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.page_count) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.record_count) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.record_size) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.write_page_id) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.attribute_count) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.schema_offset) , sizeof(uint32_t));

        // attaching the dynamic schema to the header
        heap_write_stream.write(required_headers.schema.c_str() , required_headers.schema_offset);
    }
    
    void update_heapfile_metadata(std::fstream& heap_write_stream , HeapFile_Metadata & required_headers)
    {
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.total_offset) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.page_count) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.record_count) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.record_size) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.write_page_id) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.attribute_count) , sizeof(uint32_t));
        heap_write_stream.write(reinterpret_cast<char *> (&required_headers.schema_offset) , sizeof(uint32_t));

        // attaching the dynamic schema to the header
        heap_write_stream.write(required_headers.schema.c_str() , required_headers.schema_offset);
    }

    void update_page_to_disk(std::fstream& heap_write_stream , void * new_page_block , uint16_t record_count , uint32_t page_number , uint32_t total_offset)
    {
        memcpy(new_page_block , &record_count , sizeof(uint16_t)); // first we are copying the new header
        heap_write_stream.seekg(total_offset + (page_number - 1) * PAGE_SIZE , std::ios::beg); // setting up the pointer
        heap_write_stream.write(reinterpret_cast <char *> (new_page_block) , PAGE_SIZE);
    }
   
    uint16_t get_page_record_count (void * page_block)
    {
        uint16_t page_record_count;
        memcpy(&page_record_count , page_block , sizeof(uint16_t));
        return page_record_count;
    }

    bool add_to_heap(AST_NODE *& action_node)
    {
        if (!heap_file_exists(action_node->PAYLOAD))
        {
            std::cout << "Error , the given table does not exist , cannot insert data to it ! " << std::endl;
            return false;
        }

        std::ifstream heap_reader_stream(*action_node->PAYLOAD + ".dat" , std::ios::binary);
        HeapFile_Metadata current_heap_metadata =  deserialize_heapfile_metadata(heap_reader_stream);
        heap_reader_stream.close();
        
        std::vector<std::string> schema_chunks = split_schema(current_heap_metadata.schema);
        uint16_t max_record_per_page = (PAGE_SIZE - sizeof(page_header)) / current_heap_metadata.record_size;
        
        std::fstream heap_write_stream(*action_node->PAYLOAD + ".dat" , std::ios::in | std::ios::out | std::ios::binary );
        
        // heap_write_stream.seekp(current_heap_metadata.total_offset +  current_heap_metadata.record_size * current_heap_metadata.record_count);


        void * page_read_buffer = malloc(PAGE_SIZE);
        heap_write_stream.seekp(current_heap_metadata.total_offset + (current_heap_metadata.page_count - 1) * PAGE_SIZE , std::ios::beg);
        heap_write_stream.read(reinterpret_cast <char *> (page_read_buffer) , PAGE_SIZE);
        uint16_t page_record_count = get_page_record_count(page_read_buffer);

        
        for (std::vector<AST_NODE *> record : action_node->MULTI_DATA)
        {
            if (page_record_count == max_record_per_page)
            {
                update_page_to_disk(heap_write_stream , page_read_buffer , page_record_count , current_heap_metadata.page_count , current_heap_metadata.total_offset);
                create_new_page<std::fstream> (heap_write_stream , current_heap_metadata);
                heap_write_stream.seekp(current_heap_metadata.total_offset + (current_heap_metadata.page_count - 1) * PAGE_SIZE , std::ios::beg);
                heap_write_stream.read(reinterpret_cast <char *> (page_read_buffer) , PAGE_SIZE);
                page_record_count = get_page_record_count(page_read_buffer);
            }
            void * serialized_block = serialize(record , current_heap_metadata.record_size , schema_chunks);
            uint16_t write_offset = sizeof(page_header) + (page_record_count * current_heap_metadata.record_size);
            memcpy(reinterpret_cast <char *> (page_read_buffer) + write_offset , serialized_block , current_heap_metadata.record_size);
            current_heap_metadata.record_count++;
            page_record_count++;
        }

        heap_write_stream.seekp(0 , std::ios::beg); // resetting the pointer to the start of the file 
        // to write back the updated heap file metadata
        update_heapfile_metadata(heap_write_stream , current_heap_metadata);
        // update the page count 
        update_page_to_disk(heap_write_stream , page_read_buffer , page_record_count , current_heap_metadata.page_count , current_heap_metadata.total_offset);
        std::cout << "closing the stream : " << std::endl;
        heap_write_stream.close();
        free(page_read_buffer);
        return true;
    }
   
    template <typename stream_type>
    bool create_new_page(stream_type& heap_write_stream , HeapFile_Metadata & current_meta)
    {
        void * new_page_block = malloc (PAGE_SIZE);
        uint16_t start_record_count = 0;
        memcpy(new_page_block , &start_record_count , sizeof(uint16_t));
        heap_write_stream.write(reinterpret_cast <char *> (new_page_block) , PAGE_SIZE);
        current_meta.page_count++;
        free(new_page_block);
        return true;
    }

    bool create_new_heap(AST_NODE *& action_node)
    {
        // working on the same database , change the behaviour for multi db
        if (heap_file_exists(action_node->PAYLOAD))
            return false;
        std::string table_schema = construct_schema(action_node->CHILDREN);
        uint32_t record_size = get_size(table_schema);
        HeapFile_Metadata current_metadata (table_schema , record_size);
        
        std::ofstream heap_write_stream(*action_node->PAYLOAD + ".dat" , std::ios::binary);
        // writing the heap file header to the file
        write_heapfile_metadata(heap_write_stream , current_metadata);
        create_new_page<std::ofstream>(heap_write_stream , current_metadata);
        write_heapfile_metadata(heap_write_stream , current_metadata); // writing the updated version of hte heapfile meta
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
   
    uint32_t get_attribute_offset (std::string * attribute , std::vector<std::string> & schema_chunks)
    {
        uint32_t attribute_offset = 0;
        for (const std::string & chunk : schema_chunks)
        {
            if (*attribute == chunk.substr(1))
                return attribute_offset;
            else
                attribute_offset += size_conversion[chunk[0]];
        }
        std::cout << "Error :: could not find the following attribute : " << *attribute << std::endl;
        exit(1);
        return attribute_offset;
    }
    
    std::vector<search_constraint> get_search_constraints (AST_NODE *& condition_node , std::vector<std::string> & schema_chunks)
    {
        std::vector<search_constraint> search_constraints;
        AST_NODE * buffer = condition_node->CHILDREN[0];
        for (AST_NODE * current_condition : condition_node->CHILDREN)
        {
            search_constraint new_constraint;

            new_constraint.attribute_offset = get_attribute_offset(current_condition->PAYLOAD , schema_chunks);
            new_constraint.relational_operation = current_condition->NODE_TYPE;
            new_constraint.data_type = current_condition->HELPER_TOKEN;
            new_constraint.value = *current_condition->SUB_PAYLOAD;
            new_constraint.read_size = token_set_size_converter[current_condition->HELPER_TOKEN]; 

            search_constraints.push_back(new_constraint);
        }
        return search_constraints;
   }

    bool match_search_constraints(std::vector<search_constraint> & data_constraints , void * page_block , uint16_t record_count , uint32_t record_size)
    {
        uint16_t record_offset = sizeof(page_header) + (record_count - 1) * record_size;
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
                    uint32_t current_size = current_search_constraint.read_size;
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

        uint16_t max_record_per_page = (PAGE_SIZE - sizeof(page_header)) / current_heapfile_metadata.record_size;
        std::cout << "[*] Max number of records per page : " << max_record_per_page << std::endl;


        std::vector<std::string> schema_chunks = split_schema(current_heapfile_metadata.schema);
        std::vector<search_constraint> data_constraints;
        bool constraint_flag = false;
        if (action_node->CHILD) // condition node is attached
        {
            constraint_flag = true;
            data_constraints = get_search_constraints(action_node->CHILD , schema_chunks);
        }

        for (std::string attribute : schema_chunks)
            std::cout << attribute.substr(1) << "\t";
        std::cout << std::endl;

        void * page_read_buffer = malloc(PAGE_SIZE);
        for (int page_counter = 1 ; page_counter <= current_heapfile_metadata.page_count ; page_counter++)
        {
            heap_read_stream.read(reinterpret_cast <char *> (page_read_buffer) , PAGE_SIZE);
            uint16_t page_record_count;
            memcpy(&page_record_count , page_read_buffer , sizeof(uint16_t));
            uint16_t read_offset = sizeof(page_header);
            for (int record_itr = 1 ; record_itr <= page_record_count ; record_itr++)
            {
                if (constraint_flag)
                {
                    if (match_search_constraints(data_constraints , page_read_buffer , record_itr , current_heapfile_metadata.record_size))
                        deserialize(page_read_buffer , read_offset , schema_chunks);
                }
                else
                {
                    deserialize(page_read_buffer , read_offset , schema_chunks);
                }
                read_offset += current_heapfile_metadata.record_size;
            }
        }
        
        heap_read_stream.close(); 
        return true;
    }

    void deserialize(void * page_block , uint16_t read_offset ,  std::vector<std::string>& schema_chunks)
    {
        uint16_t attribute_read_offset = read_offset;
        for (std::string attribute : schema_chunks)
        {
            switch(attribute[0])
            {
                case 'i':
                {
                    uint64_t read_buffer;
                    memcpy(&read_buffer , reinterpret_cast <char *> (page_block) + attribute_read_offset , sizeof(uint64_t));
                    attribute_read_offset += sizeof(uint64_t);
                    std::cout << read_buffer << "\t";
                    break;
                }
                case 'f':
                {
                    float read_buffer;
                    memcpy(&read_buffer , reinterpret_cast <char *> (page_block) + attribute_read_offset , sizeof(float));
                    attribute_read_offset += sizeof(float);
                    std::cout << read_buffer << "\t";
                    break;
                }
                case 's':
                {
                    uint32_t current_size = size_conversion[attribute[0]];
                    char * read_buffer = (char*) malloc(current_size + 1);
                    memcpy(read_buffer , reinterpret_cast <char *> (page_block) + attribute_read_offset , current_size);
                    attribute_read_offset += current_size;
                    read_buffer[current_size] = '\0';
                    std::cout << read_buffer << "\t";
                    free(read_buffer);
                    break;                
                }
            }
        }
        std::cout << std::endl;
    }

};

#endif