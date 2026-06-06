#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <list>
#include "passenger.h"

using namespace std;

//Линейный поиск
vector<Passenger> linearSearch(const vector<Passenger>& arr, const string& key) {
    vector<Passenger> result;
    for (const auto& p : arr) {
        if (p.fio == key) result.push_back(p);
    }
    return result;
}

//Бинарное дерево поиска (BST)
class BST {
private:
    struct Node {
        string key;
        vector<Passenger> values;
        Node* left;
        Node* right;
        Node(string k, Passenger p) : key(k), left(nullptr), right(nullptr) { values.push_back(p); }
    };
    Node* root = nullptr;

    void insertRec(Node*& node, const string& key, const Passenger& p) {
        if (!node) node = new Node(key, p);
        else if (key == node->key) node->values.push_back(p); // Дубликаты идут в вектор
        else if (key < node->key) insertRec(node->left, key, p);
        else insertRec(node->right, key, p);
    }

    vector<Passenger> searchRec(Node* node, const string& key) {
        if (!node) return {};
        if (node->key == key) return node->values;
        if (key < node->key) return searchRec(node->left, key);
        return searchRec(node->right, key);
    }

public:
    void insert(const string& key, const Passenger& p) { insertRec(root, key, p); }
    vector<Passenger> search(const string& key) { return searchRec(root, key); }
};

//Хэш-таблица (Метод цепочек)
class HashTable {
private:
    struct HashNode {
        string key;
        vector<Passenger> values;
    };
    vector<list<HashNode>> table;
    int capacity;
    int collisions;

    // Полиномиальный хэш
    int hashFunction(const string& key) const {
        unsigned long hash = 5381;
        for (char c : key) {
            hash = ((hash << 5) + hash) + c;
        }
        return hash % capacity;
    }

public:
    HashTable(int cap) : capacity(cap), collisions(0) { table.resize(capacity); }

    void insert(const string& key, const Passenger& p) {
        int index = hashFunction(key);
        bool keyExists = false;
        
        for (auto& node : table[index]) {
            if (node.key == key) {
                node.values.push_back(p);
                keyExists = true;
                break;
            }
        }
        if (!keyExists) {
            if (!table[index].empty()) collisions++;
            table[index].push_back({key, {p}});
        }
    }

    vector<Passenger> search(const string& key) const {
        int index = hashFunction(key);
        for (const auto& node : table[index]) {
            if (node.key == key) return node.values;
        }
        return {};
    }

    int getCollisions() const { return collisions; }
};

//Красно-черное дерево
enum Color { RED, BLACK };
struct RBTNode {
    string key;
    vector<Passenger> values;
    Color color;
    RBTNode *left, *right, *parent;
    RBTNode(string k, Passenger p) : key(k), color(RED), left(nullptr), right(nullptr), parent(nullptr) { values.push_back(p); }
};

class RedBlackTree {
private:
    RBTNode* root = nullptr;

    void rotateLeft(RBTNode*& root, RBTNode*& pt) {
        RBTNode* pt_right = pt->right;
        pt->right = pt_right->left;
        if (pt->right != nullptr) pt->right->parent = pt;
        pt_right->parent = pt->parent;
        if (pt->parent == nullptr) root = pt_right;
        else if (pt == pt->parent->left) pt->parent->left = pt_right;
        else pt->parent->right = pt_right;
        pt_right->left = pt;
        pt->parent = pt_right;
    }

    void rotateRight(RBTNode*& root, RBTNode*& pt) {
        RBTNode* pt_left = pt->left;
        pt->left = pt_left->right;
        if (pt->left != nullptr) pt->left->parent = pt;
        pt_left->parent = pt->parent;
        if (pt->parent == nullptr) root = pt_left;
        else if (pt == pt->parent->left) pt->parent->left = pt_left;
        else pt->parent->right = pt_left;
        pt_left->right = pt;
        pt->parent = pt_left;
    }

    void fixInsert(RBTNode*& root, RBTNode*& pt) {
        RBTNode* parent_pt = nullptr;
        RBTNode* grand_parent_pt = nullptr;
        while ((pt != root) && (pt->color != BLACK) && (pt->parent->color == RED)) {
            parent_pt = pt->parent;
            grand_parent_pt = pt->parent->parent;
            if (parent_pt == grand_parent_pt->left) {
                RBTNode* uncle_pt = grand_parent_pt->right;
                if (uncle_pt != nullptr && uncle_pt->color == RED) {
                    grand_parent_pt->color = RED;
                    parent_pt->color = BLACK;
                    uncle_pt->color = BLACK;
                    pt = grand_parent_pt;
                } else {
                    if (pt == parent_pt->right) {
                        rotateLeft(root, parent_pt);
                        pt = parent_pt;
                        parent_pt = pt->parent;
                    }
                    rotateRight(root, grand_parent_pt);
                    swap(parent_pt->color, grand_parent_pt->color);
                    pt = parent_pt;
                }
            } else {
                RBTNode* uncle_pt = grand_parent_pt->left;
                if ((uncle_pt != nullptr) && (uncle_pt->color == RED)) {
                    grand_parent_pt->color = RED;
                    parent_pt->color = BLACK;
                    uncle_pt->color = BLACK;
                    pt = grand_parent_pt;
                } else {
                    if (pt == parent_pt->left) {
                        rotateRight(root, parent_pt);
                        pt = parent_pt;
                        parent_pt = pt->parent;
                    }
                    rotateLeft(root, grand_parent_pt);
                    swap(parent_pt->color, grand_parent_pt->color);
                    pt = parent_pt;
                }
            }
        }
        root->color = BLACK;
    }

public:
    void insert(const string& key, const Passenger& p) {
        RBTNode* curr = root;
        while (curr != nullptr) {
            if (curr->key == key) {
                curr->values.push_back(p);
                return;
            }
            if (key < curr->key) curr = curr->left;
            else curr = curr->right;
        }
        
        RBTNode* pt = new RBTNode(key, p);
        root = BSTInsert(root, pt);
        fixInsert(root, pt);
    }

    RBTNode* BSTInsert(RBTNode* root, RBTNode* pt) {
        if (root == nullptr) return pt;
        if (pt->key < root->key) {
            root->left = BSTInsert(root->left, pt);
            root->left->parent = root;
        } else if (pt->key > root->key) {
            root->right = BSTInsert(root->right, pt);
            root->right->parent = root;
        }
        return root;
    }

    vector<Passenger> search(const string& key) {
        RBTNode* curr = root;
        while (curr != nullptr) {
            if (curr->key == key) return curr->values;
            if (key < curr->key) curr = curr->left;
            else curr = curr->right;
        }
        return {};
    }
};