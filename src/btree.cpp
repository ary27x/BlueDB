#include <iostream>
#include <queue> 
#include <string>
#include <cmath>
#include <utility>
#define tree_type float
using namespace std;
template <typename DataType> // leftmost node splitting bug ?
class BTreeNode
{
    public:
        DataType * keys;
        BTreeNode<DataType> ** children;
        BTreeNode<DataType> * parent;
        bool leafNode;
        int order;
        int key_count;

    BTreeNode<DataType>(int order)
    {
        this->keys = new DataType[order];
        this->children = new BTreeNode<DataType> * [order + 1];
        for (int child_itr = 0 ; child_itr < order + 1 ; child_itr++)
            this->children[child_itr] = nullptr;
        this->parent = nullptr;
        this->leafNode = true;
        this->order = order;
        this->key_count = 0;
    }

    void setLeafNode(bool leafNodeStatus)
    {
        this->leafNode = leafNodeStatus;
        return;
    }

    bool isRoot()
    {
        return !this->parent ? true : false;
    }
    BTreeNode<DataType> * split()
    {
        BTreeNode<DataType> * new_node = new BTreeNode<DataType>(this->order);
        new_node->setLeafNode(this->leafNode);
        new_node->parent = this->parent;

        int medianIndex = this->order / 2;
        DataType medianValue = this->keys[medianIndex];

        int new_node_counter = 0;
        for (int key_itr = medianIndex + 1 ; key_itr < this->key_count ; key_itr++)
        {
            new_node->keys[new_node_counter] = this->keys[key_itr];
            new_node->children[new_node_counter] = this->children[key_itr];

            if (new_node->children[new_node_counter] != nullptr) // setting up the parent pointer
                new_node->children[new_node_counter]->parent = new_node;
            this->children[key_itr] = nullptr;
            
            new_node->key_count++;
            new_node_counter++;
        }

        new_node->children[new_node_counter] = this->children[this->key_count];
        if (new_node->children[new_node_counter] != nullptr) // setting up the parent pointer
            new_node->children[new_node_counter]->parent = new_node;
        this->children[this->key_count] = nullptr;
        /*
        since , key_count -> key_count - (key_count - medianIndex)
        => key_count -> medianIndex
        */
        this->key_count = medianIndex;
        return new_node;

    }

    DataType medianValue()
    {
        return this->keys[this->key_count / 2];
    }

    bool insert (DataType new_data)
    {
        if (this->leafNode) // this is the case when we would insert the data
        {
            int new_key_position = 0;
            while (new_key_position < this->key_count && new_data > this->keys[new_key_position])
                new_key_position++;

            // right shift the element from the end to new_key_position to accomadate
            for (int key_index = key_count - 1 ; key_index >= new_key_position ; key_index--)
                this->keys[key_index + 1] = this->keys[key_index];

            this->keys[new_key_position] = new_data;
            this->key_count++;

            return this->key_count == this->order ? true : false;
        }
        else // we would just go down and then insert the data
        {
            int childPointerIndex = -1;
            for (int key_itr = 0 ; key_itr < this->key_count ; key_itr++)
            {
                DataType current_key_data = this->keys[key_itr];
                if (new_data < current_key_data)
                {
                    childPointerIndex = key_itr;
                    break;
                }
            }
            if (childPointerIndex == -1)
                childPointerIndex = this->key_count;
            bool isFull = this->children[childPointerIndex]->insert(new_data);
            if (isFull) //  we need to split the node
            {
                BTreeNode<DataType> * current_node = this->children[childPointerIndex];
                DataType medianValue = current_node->medianValue();

                BTreeNode<DataType> * new_split_node = current_node->split();
                // below we are adjusting the pointers and the keys in the parent node
                for (int itr = this->key_count - 1 ; itr >= childPointerIndex ; itr--)
                {
                        this->keys[itr + 1] = this->keys[itr];
                        this->children[itr + 2] = this->children[itr + 1];
                }
                this->keys[childPointerIndex] = medianValue;
                this->children[childPointerIndex + 1] = new_split_node; // also check this for pointer copy error
                this->key_count++;
            }
            return this->order == this->key_count ? true : false;
        }
    }
};

template <typename DataType>
class BTree
{
    private:
        int order;
        int min_keys;
        int max_keys;

    void renderKeys(BTreeNode<DataType> *& current_node , std::queue<BTreeNode<DataType> *>& auxiliary_queue)
    {
        for (int itr = 0 ; itr < current_node->key_count ; itr++)
        {
            std::cout << current_node->keys[itr] << " ";
            if (current_node->children[itr] != nullptr)
                auxiliary_queue.push(current_node->children[itr]);
        }
        if (current_node->children[current_node->key_count] != nullptr)
                auxiliary_queue.push(current_node->children[current_node->key_count]);
        // if (current_node->leafNode) std::cout << "[LEAF]";        else             std::cout << "[NOT LEAF]";        if (current_node->parent == nullptr)            std::cout << "[NO PARENT]";        else             std::cout << "[PARENT]";
        std::cout << "\t";
    }
    void renderQueue(std::queue<BTreeNode<DataType> *>& main_queue)
    {
        std::queue<BTreeNode<DataType> *> auxiliary_queue;
        while (!main_queue.empty())
        {
            this->renderKeys(main_queue.front() , auxiliary_queue);
            main_queue.pop();
        }
        std::cout << std::endl;
        if (!auxiliary_queue.empty())
            renderQueue(auxiliary_queue);
    }

    public:
        BTreeNode<DataType> * root;
        int height;

    BTree<DataType>(int order)
    {
        this->order = order;
        this->root = new BTreeNode<DataType>(order);
        this->height = 1;
        this->max_keys = this->order - 1;
        this->min_keys = ceil((float)this->order / 2) - 1;
    }
    void display()
    {
        std::cout << "BTree : " << std::endl;
        std::queue<BTreeNode<DataType> *> main_queue;
        main_queue.push(this->root);
        this->renderQueue(main_queue);
    }
    void insert (DataType new_data)
    {
        bool isFull = this->root->insert(new_data);
        if (isFull)
        {
            BTreeNode<DataType> * new_root = new BTreeNode<DataType>(this->order);
            DataType medianValue = this->root->medianValue();
            BTreeNode<DataType> * new_sibling_node = this->root->split();

            new_sibling_node->setLeafNode(this->root->leafNode);
            new_root->setLeafNode(false);

            new_root->children[0] = this->root;
            new_root->children[1] = new_sibling_node;
            new_root->keys[new_root->key_count++] = medianValue;

            this->root->parent = new_root;
            new_sibling_node->parent = new_root;

            this->height++;
            this->root = new_root;
        }
    }

    int left_sibling_key_count (BTreeNode<DataType> *& current)
    {
        if (!current->parent || current->parent->children[0] == current) return 0;
        for (int itr = 0 ; itr < current->parent->key_count ; itr++)
            if (current->parent->children[itr + 1] == current)
                return current->parent->children[itr]->key_count;
        return -1;
    }

    int right_sibling_key_count (BTreeNode<DataType> *& current)
    {
        if (!current->parent || current->parent->children[current->parent->key_count] == current) return 0;
        for (int itr = current->parent->key_count ; itr > 0 ; itr--)
            if (current->parent->children[itr - 1] == current)
                return current->parent->children[itr]->key_count;
        return 0;
    }

    int predecessor_key_count(BTreeNode<DataType> *& current , int position)
    {
        BTreeNode<DataType> * predecessor = current->children[position];
        if (!predecessor) return 0;
        while (predecessor->children[predecessor->key_count])
            predecessor = predecessor->children[predecessor->key_count];
        return predecessor->key_count;
    }
    int successor_key_count(BTreeNode<DataType> *& current , int position)
    {
        BTreeNode<DataType> * successor = current->children[position + 1];
        while (successor->children[0])
            successor = successor->children[0];
        return successor->key_count;
    }

    BTreeNode<DataType> * get_predecessor(BTreeNode<DataType> *& current , int position)
    {
        BTreeNode<DataType> * predecessor = current->children[position];
        while (predecessor->children[predecessor->key_count])
            predecessor = predecessor->children[predecessor->key_count];
        return predecessor;
    }

    BTreeNode<DataType> * get_successor(BTreeNode<DataType> *& current , int position)
    {
        BTreeNode<DataType> * successor = current->children[position + 1];
        while (successor->children[0])
            successor = successor->children[0];
        return successor;
    }

    int get_position (BTreeNode <DataType> *& current)
    {
        for (int itr = 0 ; itr <= current->parent->key_count ; itr++)
            if (current->parent->children[itr] == current)
                return itr;
    }

    void borrow_left (BTreeNode <DataType> *& current)
    {
        int position = get_position(current);
        current->insert(current->parent->keys[position - 1]);
        BTreeNode <DataType> * left_sibling = current->parent->children[position - 1];
        current->parent->keys[position - 1] = left_sibling->keys[left_sibling->key_count - 1];
        left_sibling->key_count--;
    }
    void borrow_right (BTreeNode <DataType> *& current)
    {
        int position = get_position(current);
        current->insert(current->parent->keys[position]);
        BTreeNode <DataType> * right_sibling = current->parent->children[position + 1];
        current->parent->keys[position] = right_sibling->keys[0];
        for (int itr = 0 ; itr < right_sibling->key_count - 1 ; itr++)
            right_sibling->keys[itr] = right_sibling->keys[itr + 1];
        right_sibling->key_count--;
    }

    void handle_leaf_underflow (BTreeNode <DataType> *& current)
    {
        if (left_sibling_key_count(current) > this->min_keys) // borrow from left
            borrow_left(current);
        else if (right_sibling_key_count(current) > this->min_keys) // borrow from right
            borrow_right(current);
        else // merge the node 
            merge(current);
    }

    void handle_root_deletion(BTreeNode <DataType> *& root_node)
    {
        BTreeNode <DataType> * buffer_pointer = root_node;
        this->root = root_node->children[0];
        this->root->parent = nullptr;
        delete buffer_pointer;
        this->height--;
    }

    void merge (BTreeNode<DataType> *& current)
    {
        BTreeNode<DataType> * parent = current->parent;
        int original_parent_kc = parent->key_count;
        int position = get_position(current);
        int child_shift_position = position;
        int key_shift_position;
        BTreeNode<DataType> * sibling;
        tree_type parent_value;
        bool mergingToRight = false;
        if (position == 0) // merge into the right sibling
        {
            mergingToRight = true;
            key_shift_position = position;
            sibling = parent->children[position + 1];
            parent_value = parent->keys[position];
        }
        else // merge into the left sibling
        {
            mergingToRight = false;
            key_shift_position = position - 1;
            sibling = parent->children[position - 1];
            parent_value = parent->keys[position - 1];
        }
        int original_sibling_kc = sibling->key_count;
        // inserting the pointers first
        
        if (!mergingToRight) // insertion case for merging to left
        {
            for (int itr = 0 ; itr <= current->key_count ; itr++)
                sibling->children[++sibling->key_count] = current->children[itr];
            // resetting the sibling key count to insert keys
            sibling->key_count = original_sibling_kc;
            // inserting the paretnt value
            sibling->keys[sibling->key_count++] = parent_value;
            // inserting the values off the current node
            for (int itr = 0 ; itr < current->key_count ; itr++)
                sibling->keys[sibling->key_count++] = current->keys[itr];  
        }
        else // insertion case for merging to right
        {
            int shiftCount = 1 + current->key_count; // number of new keys added
            // first we need to shift the keys fo the right sibling
            for (int itr = sibling->key_count - 1 ; itr >= 0 ; itr--)
                sibling->keys[itr + shiftCount] = sibling->keys[itr];
            // then we need to shift the pointers 
            for (int itr = sibling->key_count ; itr >= 0 ; itr--)
                sibling->children[itr + shiftCount] = sibling->children[itr];
            // then we need to copy the new keys and the pointers
            for (int itr = 0 ; itr < shiftCount ; itr++)
            {
                sibling->keys[itr] = (itr == shiftCount - 1) ? parent_value : current->keys[itr];
                sibling->children[itr] = current->children[itr];
                sibling->key_count++;
            }
        }
        
        delete current;

        // adjust the parent pointers
        for (int itr = key_shift_position ; itr < original_parent_kc - 1 ; itr++)
            parent->keys[itr] = parent->keys[itr + 1];
        for (int itr = child_shift_position ; itr < original_parent_kc  ; itr++)
            parent->children[itr] = parent->children[itr + 1];
        parent->key_count--;
    }

    void search(BTreeNode<DataType> *& current ,  tree_type value)
    {
        if (current == nullptr)
        {
            std::cout << "[!] The given value was not found in the B Tree " << std::endl;
            return;
        }
        BTreeNode<DataType> * next_block = current->children[current->key_count];
        for (int itr = 0 ; itr < current->key_count ; itr++)
        {
            if (value < current->keys[itr])
            {
                next_block = current->children[itr];
                break;
            }
            if (value == current->keys[itr])
            {
                std::cout << "[*] The given value was FOUND in the B Tree !!! " << std::endl;
                return ;
            }
        }
        search(next_block , value);
    }

    void remove(BTreeNode<DataType> *& current ,  tree_type value)
    {
        if (current == nullptr)
            return;
        BTreeNode<DataType> * next_block = current->children[current->key_count];
        for (int itr = 0 ; itr < current->key_count ; itr++)
        {
            if (value < current->keys[itr])
            {
                next_block = current->children[itr];
                break;
            }
            if (value == current->keys[itr]) // the value is found in the b tree
            {
                if (current->leafNode) // deletion from leaf node
                {
                    for (int remove_itr = itr ; remove_itr < current->key_count - 1 ; remove_itr++)
                        current->keys[remove_itr] = current->keys[remove_itr + 1];
                    current->key_count--;
                    if (current->key_count < this->min_keys && !current->isRoot())
                        handle_leaf_underflow(current);
                }
                else // deletion from non leaf node
                {
                    if (predecessor_key_count(current , itr) >  this->min_keys)
                    {
                        BTreeNode<DataType> * predecessor = get_predecessor(current , itr);
                        tree_type data_buffer = predecessor->keys[predecessor->key_count - 1];
                        this->remove(current , data_buffer);
                        current->keys[itr] = data_buffer;
                    }
                    else if (successor_key_count(current , itr) >  this->min_keys)
                    {
                        std::cout << "replacing with successor : " << std::endl;
                        BTreeNode<DataType> * successor = get_successor(current , itr);
                        tree_type data_buffer = successor->keys[0];
                        std::cout << "value : " << data_buffer << std::endl;
                        this->remove(current , data_buffer);
                        current->keys[itr] = data_buffer;
                    }
                    else
                    {
                        /*
                        i strongly feel that there is a bug here , 
                        because after forcibly taking key from the predecessor node,
                        we are handling the underflow for the predecessor node , 
                        but then we are not moving bottom up from the predecessor node , 
                        that is , while handling the underflow in the pred node, if an underflow
                        occurs in a node between the pred node and "current" node , that would not be
                        fixed. 
                         */
                        BTreeNode<DataType> * predecessor = get_predecessor(current , itr);
                        tree_type data_buffer = predecessor->keys[predecessor->key_count - 1];
                        current->keys[itr] = data_buffer;
                        predecessor->key_count--;
                        if (predecessor->key_count < this->min_keys)
                            handle_leaf_underflow(predecessor);
                    }

                    if (current->isRoot() && current->key_count == 0)
                        handle_root_deletion(current);    
                }
                return;
            }
        }
        remove(next_block , value);
        if (current->key_count < this->min_keys && !current->isRoot())
            merge(current);
        if (current->isRoot() && current->key_count == 0)
            handle_root_deletion(current);
    }

};

int main()
{
    int order;
    order = 5;
    BTree<tree_type> * Tree = new BTree<tree_type>(order);
    // fix the root , leaf , not leaf pointers
    for (int i = 1 ; i <= 17 ; i++) 
        Tree->insert(i);
    while (true)
    {
        std::cout << "--------------------\n";
        Tree->display();
        std::cout << "Height : " << Tree->height << std::endl;
        std::cout << "********************\n";
        int choice;
        std::cout << "1) enter a value : " << std::endl;
        std::cout << "2) search the tree : " << std::endl;
        std::cout << "3) delete : " << std::endl;
        std::cin >> choice;
        tree_type value;
        if (choice == 1)
        {
            std::cout << "Enter the value : " << std::endl;
            std::cin >> value;
            Tree->insert(value);
        }
        else if (choice == 2)
        {
            std::cout << "Enter the value to search for in the tree : " << std::endl;
            std::cin >> value;
            Tree->search(Tree->root , value);
        }
        else
        {
            std::cout << "Enter the value to delete from the tree : " << std::endl;
            std::cin >> value;
            Tree->remove(Tree->root , value);
        }
    }
    return 0;
}