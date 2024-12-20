#ifndef __B_TREE_H
#define __B_TREE_H

#include <iostream>
#include <queue>
#include <vector>
#include <fstream>

template <typename key_type>
class key_container
{
    public:
    key_type main_key;
    uint64_t key_offset;


    key_container(key_type value , uint64_t _key_offset)
    {
        main_key = value;
        key_offset = _key_offset;
    }

    bool operator==(const key_container<key_type>& rhs)  
    {
        return this->main_key == rhs.main_key;
    }

    bool operator!=(const key_container<key_type>& rhs)  
    {
        return this->main_key != rhs.main_key;
    }

    bool operator<(const key_container<key_type>& rhs)  
    {
        return this->main_key < rhs.main_key;
    }

    bool operator<=(const key_container<key_type>& rhs)  
    {
        return this->main_key <= rhs.main_key;
    }

    bool operator>(const key_container<key_type>& rhs)  
    {
        return this->main_key > rhs.main_key;
    }

    bool operator>=(const key_container<key_type>& rhs)  
    {
        return this->main_key >= rhs.main_key;
    }


    bool operator==(const key_type& rhs)  
    {
        return this->main_key == rhs;
    }

    bool operator!=(const key_type& rhs)  
    {
        return this->main_key != rhs;
    }

    bool operator<(const key_type& rhs)  
    {
        return this->main_key < rhs;
    }

    bool operator<=(const key_type& rhs)  
    {
        return this->main_key <= rhs;
    }

    bool operator>(const key_type& rhs)  
    {
        return this->main_key > rhs;
    }

    bool operator>=(const key_type& rhs)  
    {
        return this->main_key >= rhs;
    }


    template <typename ostream_key_type>
    friend std::ostream& operator << (std::ostream & os , const key_container<ostream_key_type>& operand);

};

template <typename overloadT>
std::ostream& operator << (std::ostream& os , const key_container<overloadT>& operand)
{
    os << operand.main_key << "(" << operand.key_offset << ")";
    return os;
}


template <typename tree_type, int t = 3>
class BTreeNode {
public:
    std::vector<tree_type> keys;
    std::vector<BTreeNode*> children;
    bool leaf;
    int n; 

    BTreeNode(bool isLeaf = true) 
    {
        leaf = isLeaf;
        n = 0;
        keys.reserve(2 * t - 1);
        children.reserve(2 * t);
    }

    int findKey(tree_type k) 
    {
        int idx = 0;
        while (idx < n && keys[idx] < k)
            ++idx;
        return idx;
    }

    void splitChild(int i, BTreeNode * y) 
    {
        BTreeNode * z = new BTreeNode(y->leaf);
        z->n = t - 1;

        for (int j = 0; j < t - 1; j++)
            z->keys.push_back(y->keys[j + t]);
        
        if (!y->leaf) {
            for (int j = 0; j < t; j++)
                z->children.push_back(y->children[j + t]);
        }

        y->n = t - 1;

        for (int j = n; j >= i + 1; j--)
            children[j + 1] = children[j];

        children[i + 1] = z;

        for (int j = n - 1; j >= i; j--)
            keys[j + 1] = keys[j];

        keys[i] = y->keys[t - 1];

        n = n + 1;
    }

    void insertNonFull(tree_type k) {
        int i = n - 1;

        if (leaf) 
        {
            while (i >= 0 && keys[i] > k) {
                keys[i + 1] = keys[i];
                i--;
            }
            keys[i + 1] = k;
            n = n + 1;
        } else {
            while (i >= 0 && keys[i] > k)
                i--;
            i++;

            if (children[i]->n == 2 * t - 1) {
                splitChild(i, children[i]);
                if (keys[i] < k)
                    i++;
            }
            children[i]->insertNonFull(k);
        }
    }

    void remove(tree_type k) {
        int idx = findKey(k);

        if (idx < n && keys[idx] == k) {
            if (leaf)
                removeFromLeaf(idx);
            else
                removeFromNonLeaf(idx);
        } else {
            if (leaf) {
                std::cout << "The key " << k << " does not exist in the tree\n";
                return;
            }

            bool flag = (idx == n);

            if (children[idx]->n < t)
                fill(idx);

            if (flag && idx > n)
                children[idx - 1]->remove(k);
            else
                children[idx]->remove(k);
        }
    }

    void removeFromLeaf(int idx) {
        for (int i = idx + 1; i < n; ++i)
            keys[i - 1] = keys[i];
        n--;
    }

    void removeFromNonLeaf(int idx) {
        tree_type k = keys[idx];

        if (children[idx]->n >= t) {
            tree_type pred = getPred(idx);
            keys[idx] = pred;
            children[idx]->remove(pred);
        }
        else if (children[idx + 1]->n >= t) {
            tree_type succ = getSucc(idx);
            keys[idx] = succ;
            children[idx + 1]->remove(succ);
        }
        else {
            merge(idx);
            children[idx]->remove(k);
        }
    }

    tree_type getPred(int idx) {
        BTreeNode* cur = children[idx];
        while (!cur->leaf)
            cur = cur->children[cur->n];
        return cur->keys[cur->n - 1];
    }

    tree_type getSucc(int idx) {
        BTreeNode* cur = children[idx + 1];
        while (!cur->leaf)
            cur = cur->children[0];
        return cur->keys[0];
    }

    void fill(int idx) {
        if (idx != 0 && children[idx - 1]->n >= t)
            borrowFromPrev(idx);
        else if (idx != n && children[idx + 1]->n >= t)
            borrowFromNext(idx);
        else {
            if (idx != n)
                merge(idx);
            else
                merge(idx - 1);
        }
    }

    void borrowFromPrev(int idx) {
        BTreeNode* child = children[idx];
        BTreeNode* sibling = children[idx - 1];

        for (int i = child->n - 1; i >= 0; --i)
            child->keys[i + 1] = child->keys[i];

        if (!child->leaf) {
            for (int i = child->n; i >= 0; --i)
                child->children[i + 1] = child->children[i];
        }

        child->keys[0] = keys[idx - 1];

        if (!child->leaf)
            child->children[0] = sibling->children[sibling->n];

        keys[idx - 1] = sibling->keys[sibling->n - 1];

        child->n += 1;
        sibling->n -= 1;
    }

    void borrowFromNext(int idx) {
        BTreeNode* child = children[idx];
        BTreeNode* sibling = children[idx + 1];

        child->keys[child->n] = keys[idx];

        if (!child->leaf)
            child->children[child->n + 1] = sibling->children[0];

        keys[idx] = sibling->keys[0];

        for (int i = 1; i < sibling->n; ++i)
            sibling->keys[i - 1] = sibling->keys[i];

        if (!sibling->leaf) {
            for (int i = 1; i <= sibling->n; ++i)
                sibling->children[i - 1] = sibling->children[i];
        }

        child->n += 1;
        sibling->n -= 1;
    }

    void merge(int idx) {
        BTreeNode* child = children[idx];
        BTreeNode* sibling = children[idx + 1];

        child->keys[t - 1] = keys[idx];

        for (int i = 0; i < sibling->n; ++i)
            child->keys[i + t] = sibling->keys[i];

        if (!child->leaf) {
            for (int i = 0; i <= sibling->n; ++i)
                child->children[i + t] = sibling->children[i];
        }

        for (int i = idx + 1; i < n; ++i)
            keys[i - 1] = keys[i];

        for (int i = idx + 2; i <= n; ++i)
            children[i - 1] = children[i];

        child->n += sibling->n + 1;
        n--;

        delete sibling;
    }
};

template <typename tree_type, typename inner_type , int t = 3> 
class BTree {

private:
    uint64_t tree_key_count;
    uint16_t key_size; 

public:
    BTreeNode<tree_type, t>* root;

    BTree()
    {
       root = nullptr;
       tree_key_count = 0;
       key_size = sizeof(int); 
    } 


    BTree(uint16_t _key_size) 
    {
        root = nullptr;
        tree_key_count = 0;
        key_size = _key_size;
    }

    template <typename type>
    void write_key_containter(std::ofstream & b_tree_write_stream , key_container<type>& current_key)
    {
        b_tree_write_stream.write(reinterpret_cast <char *> (&current_key.main_key) , this->key_size);
        b_tree_write_stream.write(reinterpret_cast <char *> (&current_key.key_offset) , sizeof(uint64_t));
    }

    void disk_serialize(std::string file_name)
    {
        std::ofstream b_tree_write_stream (file_name , std::ios::binary);

        b_tree_write_stream.write(reinterpret_cast <char *> (&tree_key_count) , sizeof(uint64_t));
        b_tree_write_stream.write(reinterpret_cast <char *> (&this->key_size) , sizeof(uint16_t));

        // we dont even need the key size

        std::queue<BTreeNode<tree_type, t>*> q;
        
        if (root)
            q.push(root);

        while (!q.empty()) {
            int size = q.size();
            for (int i = 0; i < size; i++) {
                BTreeNode<tree_type, t>* current = q.front();
                q.pop();

                for (int j = 0; j < current->n; j++) 
                    write_key_containter(b_tree_write_stream , current->keys[j]);
                

                if (!current->leaf) {
                    for (int j = 0; j <= current->n; j++) {
                        if (current->children[j])
                            q.push(current->children[j]);
                    }
                }
            }
        }
        b_tree_write_stream.close();
    }

    void insert(tree_type k) {
        tree_key_count++;
        if (root == nullptr) 
        {
            root = new BTreeNode<tree_type, t>();
            root->keys.push_back(k);
            root->n = 1;
        } else {
            if (root->n == 2 * t - 1) {
                BTreeNode<tree_type, t>* s = new BTreeNode<tree_type, t>(false);
                s->children.push_back(root);
                s->splitChild(0, root);
                int i = 0;
                if (s->keys[0] < k)
                    i++;
                s->children[i]->insertNonFull(k);
                root = s;
            } else {
                root->insertNonFull(k);
            }
        }
    }

    void load(std::string index_file_name)
    {
        std::ifstream tree_read_stream(index_file_name , std::ios::binary);

        uint16_t read_key_size;
        uint64_t read_key_count;

        tree_read_stream.read(reinterpret_cast <char*> (&read_key_count) , sizeof(uint64_t));
        tree_read_stream.read(reinterpret_cast <char*> (&read_key_size) , sizeof(uint16_t));

        while (read_key_count--)
        {
            inner_type main_key;
            uint64_t key_offset;

            tree_read_stream.read(reinterpret_cast <char *> (&main_key) , read_key_size);
            tree_read_stream.read(reinterpret_cast <char *> (&key_offset) , sizeof(uint64_t));
            
            // avoiding the insert now 
            this->insert(key_container<inner_type>(main_key , key_offset));
        }
        
    }

    tree_type search(BTreeNode<tree_type, t> *& current, tree_type value)
    {
        if (current == nullptr)
        {
            key_container<inner_type> not_found(-1,-1);
            return not_found;
        }
        
        int i = 0;
        while (i < current->n && value > current->keys[i])
        {
            i++;
        }
        
        if (i < current->n && value == current->keys[i])
            return current->keys[i];

        if (current->leaf)
        {
            key_container<inner_type> not_found(-1,-1);
            return not_found;
        }
        
        return search(current->children[i], value);
    }

    void remove(tree_type k) 
    {
        if (!root) 
        {
            std::cout << "The tree is empty\n";
            return;
        }
        root->remove(k);

        if (root->n == 0) {
            BTreeNode<tree_type, t>* tmp = root;
            if (root->leaf)
                root = nullptr;
            else
                root = root->children[0];
            delete tmp;
        }
    }

    void levelOrderTraversal() {
        if (root == nullptr) {
            std::cout << "Tree is empty\n";
            return;
        }

        std::queue<BTreeNode<tree_type, t>*> q;
        q.push(root);

        while (!q.empty()) {
            int size = q.size();
            for (int i = 0; i < size; i++) {
                BTreeNode<tree_type, t>* current = q.front();
                q.pop();

                // Print keys of current node
                for (int j = 0; j < current->n; j++) {
                    std::cout << current->keys[j] << " ";
                }
                std::cout << "\t\t";

                // Add children to queue if not leaf
                if (!current->leaf) {
                    for (int j = 0; j <= current->n; j++) {
                        if (current->children[j])
                            q.push(current->children[j]);
                    }
                }
            }
            std::cout << "\n"; // New line for each level
        }
    }

    ~BTree() {
        if (root != nullptr) {
            deleteTree(root);
        }
    }

private:
    void deleteTree(BTreeNode<tree_type, t>* node) {
        if (node == nullptr) return;
        
        if (!node->leaf) {
            for (size_t i = 0; i <= node->n; i++) {
                deleteTree(node->children[i]);
            }
        }
        
        delete node;
    }
};

#endif
