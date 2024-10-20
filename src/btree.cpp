#include <iostream>
#include <queue> // write own version of queue 
#include <string>
#define tree_type float
using namespace std;
/*

personal notes

insertion of data in b tree only happens in leaf nodes

take care of the edge case / bug in left most node splliting 

write a split function 


*/
template <typename DataType>
class BTreeNode
{
    public:
        DataType * keys;
        BTreeNode<DataType> ** children;
        bool leafNode;
        int order;
        int key_count;
    BTreeNode<DataType>(int order)
    {
        this->keys = new DataType[order];
        this->children = new BTreeNode<DataType> * [order + 1];
        for (int child_itr = 0 ; child_itr < order + 1 ; child_itr++)
            this->children[child_itr] = nullptr;
        this->leafNode = true; 
        this->order = order;
        this->key_count = 0;
    }
    void setLeafNode(bool leafNodeStatus)
    {
        this->leafNode = leafNodeStatus;
        return;
    }

    

    BTreeNode<DataType> * split()
    {
        BTreeNode<DataType> * new_node = new BTreeNode<DataType>(this->order);
        new_node->setLeafNode(this->leafNode);

        int medianIndex = this->order / 2;
        DataType medianValue = this->keys[medianIndex];

        int new_node_counter = 0;
        for (int key_itr = medianIndex + 1 ; key_itr < this->key_count ; key_itr++)
        {
            new_node->keys[new_node_counter] = this->keys[key_itr];
            new_node->children[new_node_counter] = this->children[key_itr];
            this->children[key_itr] = nullptr;
            new_node->key_count++;
            new_node_counter++;
        }

        new_node->children[new_node_counter] = this->children[this->key_count];
        this->children[this->key_count] = nullptr;

        // since , key_count -> key_count - (key_count - medianIndex)
        // => key_count -> medianIndex
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
                
                for (int itr = this->key_count - 1 ; itr >= childPointerIndex ; itr--)
                {
                        this->keys[itr + 1] = this->keys[itr];
                        this->children[itr + 2] = this->children[itr + 1];
                }
                this->keys[childPointerIndex] = medianValue;
                this->children[childPointerIndex + 1] = new_split_node;
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
        BTreeNode<DataType> * root;
        int order;
        int height;

    void renderKeys(BTreeNode<DataType> * current_node , std::queue<BTreeNode<DataType> *>& auxiliary_queue)
    {
        for (int itr = 0 ; itr < current_node->key_count ; itr++)
        {
            std::cout << current_node->keys[itr] << " ";
            if (current_node->children[itr] != nullptr)
                auxiliary_queue.push(current_node->children[itr]);
        }
        if (current_node->children[current_node->key_count] != nullptr)
                auxiliary_queue.push(current_node->children[current_node->key_count]);
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
    BTree<DataType>(int order)
    {
        this->order = order;
        this->root = new BTreeNode<DataType>(order);
        this->height = 1;
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

            this->height++;
            this->root = new_root;
        }
    }
    

};

int main()
{
    int order;
    std::cout << "Enter the order of b tree : " << std::endl;
    std::cin >> order;


    BTree<tree_type> * Tree = new BTree<tree_type>(order);
    while (true)
    {
        int choice;
        // std::cout << "1) enter a value : " << std::endl;
        // std::cout << "2) display the tree : " << std::endl;
        // std::cin >> choice;
        Tree->display();

        choice = 1;
        if (choice == 1)
        {
            tree_type value;
            std::cout << "Enter the value : " << std::endl;
            std::cin >> value;
            Tree->insert(value);
        }
        else    
            Tree->display();
    }
    return 0;
}