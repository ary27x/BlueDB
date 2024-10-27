#include <iostream>


/*

updating pages : dirty pages and write_back_manager :
-----------------------------------------------------
suppose we need to update page number 5 , and we have a cached version of it in the 
memory. we would then just make the updates in the cached version, instead of 
the disk page. this approach would help us in the sense that suppose we are making
10 changes to the page number 5 , writing all the 10 changes back to the disk pages
one after the other , which would involve 10 disk io operation , we would just
make the changes to the cached counterpart, and would set the isDirtyPage flag to 
be true, which would indicate that its disk counterpart is not updated.

now we would need a write_back_manager to handle updation

when we are evicting a page from the lru_cache , before the eviction, we would need
to see if the isDirtyPage is set to true or not. if it is , we would need to first
update the actual disk page with the dirty page, and then evict the dirty page
from the cache.

to handle application closure or cache flushing , 
we would append the write_back_manager to the start of the lru_cache destructor and
flush method respectively , so that the write_back_manager would first iterate 
over all the cached pages and update the ones that are dirty , before getting rid
of them all.
*/
template <typename NodeType>
class DLL_Node
{
    public:
    DLL_Node * next;
    DLL_Node * previous;
    NodeType payload;

    DLL_Node(NodeType value)
    {
        this->payload = value;
        this->next = nullptr;
        this->previous = nullptr;
    }
};


template <typename NodeType>
class LL_Node
{
    public:
    LL_Node<NodeType> * next;
    NodeType payload;

    LL_Node(NodeType value)
    {
        this->payload = value;
        this->next = nullptr;
    }
};


template <typename NodeType>
class DLL
{
    public:
    int length;
    DLL_Node<NodeType> * head;
    DLL_Node<NodeType> * tail;
    int cache_size;

    DLL(int _cache_size)
    {
        this->cache_size = _cache_size;
        this->length = 0;
        this->head = nullptr;
        this->tail = nullptr;
    }

    ~DLL()
    {
        std::cout << "DLL Destructor : " << std::endl;
        DLL_Node<NodeType> * iterator = this->head;
        DLL_Node<NodeType> * next_pointer = nullptr;
        while(iterator)
        {
            next_pointer = iterator->next;
            delete iterator;
            iterator = next_pointer;
        }
        return ;
    }
 
    void insertToDLLHead(NodeType data)
    {
        DLL_Node<NodeType> * new_node = new DLL_Node<NodeType>(data);
        if (this->length == this->cache_size) 
            this->removeDLLTail();
        new_node->next = this->head;
        if (this->head) // if the head already exists , then overwrite it
            this->head->previous = new_node;
        else
            this->tail = new_node;
        this->head = new_node;
        this->length++;
    } 

    bool removeDLLTail()
    {
        if (this->length == 0)
            return false;
        DLL_Node<NodeType> * buffer_pointer = this->tail;
        this->tail = this->tail->previous;
        this->tail->next = nullptr;
        this->length--;
        delete buffer_pointer;
        return true;
    }

    void render_list()
    {
        DLL_Node<NodeType> * itr = this->head;
        while (itr)
        {
            std::cout << itr->payload << "  ";
            itr = itr->next;
        }
        std::cout << std::endl;
    }


};

template <typename entry_type>
class HashTable
{
    private:
    int size;
    int key_count;
    LL_Node<entry_type> ** main_table;
    public:
    HashTable(int _size)
    {
        this->size = _size;
        this->key_count = 0;
        this->main_table = new LL_Node<entry_type> * [this->size];
        for (int itr = 0 ; itr < this->size ; itr++)
            this->main_table[itr] = nullptr;
    }

    ~HashTable()
    {
        std::cout << "HashTable destructor : " << std::endl;
        delete [] main_table;
        return;
    }

    entry_type lookup(entry_type value) // could maybe return the pointer itself
    {
        int hash_code = this->hash_function(value);
        LL_Node<entry_type> * current_head = this->main_table[hash_code];
        while (true)
        {
            if (!current_head)
                return 0; // this is the dummy failure return value
                // when properly implemented , could return a nullptr instead
            if (current_head->payload == value)
                return current_head->payload;
            else 
                current_head = current_head->next;
        }
    }

    int hash_function(entry_type new_entry)
    {
    return new_entry % this->size; 
    }

    bool remove(entry_type to_be_removed)
    {
        int hash_code = this->hash_function(to_be_removed);
        LL_Node<entry_type> * required_head = this->main_table[hash_code];
        LL_Node<entry_type> * previous = nullptr;
        while (true)
        {
            if (required_head == nullptr) return false;

            if (required_head->payload == to_be_removed)
            {
                if (!previous) 
                    this->main_table[hash_code] = this->main_table[hash_code]->next;
                else  
                    previous->next = required_head->next;
                delete required_head;
                this->key_count--;
                return true;
            }
            else
            {
                previous = required_head;
                required_head = required_head->next;
            }
        }
        
    }

    /*
    the hashtable alone would not have any kind of limit , that is the size per say
    but the reason that we have a this->size member and the fact that when the hashmap
    gets full , we return the false from the insert() method since we would not be 
    able to insert any kind of new record into the hashmap is the hashmap would be
    used in accordance with the lru_cache , which would have a limit , and the limit
    of the lru_cache would be the limit of the hashmap , and hence when the lru cache 
    is full , the hashmap should not take in any new values.
    */
    bool insert(entry_type new_entry) 
    {
        if (this->key_count == this->size) 
            return false;
        LL_Node<entry_type> * new_node = new LL_Node<entry_type>(new_entry);
        int hash_code = this->hash_function(new_entry);
        LL_Node<entry_type> * address_header = this->main_table[hash_code];
        if (!address_header)
            this->main_table[hash_code] = new_node;
        else
        {
            while(address_header->next)
                address_header = address_header->next;
            address_header->next = new_node;
        }
        this->key_count++;
        return true;
    }

    void renderHashMap()
    {
        std::cout << "Hash Map : " << std::endl;
        std::cout << "----------" << std::endl;
        std::cout << "Key\tValues" << std::endl;
        for (int itr = 0 ; itr < this->size ; itr++)
        {
            std::cout << itr << "\t";
            LL_Node <entry_type> * current_head = this->main_table[itr];
            while (current_head)
            {
                std::cout << current_head->payload << " ";
                current_head = current_head->next;
            }
            std::cout << std::endl;
        }
        std::cout << "----------" << std::endl;
    }


};

/*
key : 5
value : 

*/

template <typename cache_item>
class LRU_Cache
{
    private:
    int cache_size;
    DLL<cache_item> * cache_entries;
    HashTable<cache_items> * hash_table;
    
    public:
    LRU_Cache(int _cache_size)
    {
        this->cache_size = _cache_size;
        this->cache_entries = new DLL<cache_item>(_cache_size);
        this->hash_table = new HashTable<cache_item>(_cache_size);
    }

    bool cache_entry_exists()
    {

    }


    cache_item retrieve(cache_item key)
    {
        cache_item retrieve = this->hash_table->lookup(key);
        
 
    }

    void cache(cache_item new_key)
    {

    }

    void flush()
    {

    }   

    void resize()
    {

    }


};


int main()
{

    HashTable<int> * main_table = new HashTable<int>(10);
    for (int i = 0 ; i < 10 ; i++)
        main_table->insert(rand());
    while(true)
    {
        main_table->renderHashMap();
        std::cout << "1)Insert : \n2)Delete \n3)Lookup: " << std::endl;
        int choice;
        std::cin >> choice;
        if (choice == 1)
        {
            std::cout << "enter the value to insert : " << std::endl;
            int value;
            std::cin >> value;
            bool status = main_table->insert(value);
            if (!status)
                std::cout << "The hashmap is full , cannot insert more keys : " << std::endl;
        
        }
        else if (choice == 2)
        {
            std::cout << "enter the value to delete : " << std::endl;
            int value;
            std::cin >> value;
            bool status = main_table->remove(value);
            if (status)
                std::cout << "the given value was removed from the hashtable : " << std::endl;
            else 
                std::cout << "error : the given value was not found in the hashtable : " << std::endl;
        }
        else 
        {
            std::cout << "Enter the key to lookup for : " << std::endl;
            int key;
            std::cin >> key;
            int recv = main_table->lookup(key);
            if (recv)
                std::cout << "The lookup was successfull , this is the value which is retrieved : " << recv << std::endl;
            else 
                std::cout << "the lookup was unsuccessfull, the value was not found in the hash map : " << std::endl;

        }
    }



    return 0;
}



