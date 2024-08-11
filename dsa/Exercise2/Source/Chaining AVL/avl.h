#pragma once

#include <iostream>

namespace AVL {
template <class AVLNode, class K, class V>
AVLNode *newNode(K key, V value);

template <class AVLNode, class K, class V>
AVLNode *insertNode(AVLNode *root, K key, V value);

template <class AVLNode, class K>
AVLNode *searchNode(AVLNode *root, K key);

template <class AVLNode, class K>
AVLNode *deleteNode(AVLNode *root, K key);

template <class AVLNode>
AVLNode *findMin(AVLNode *root);

template <class AVLNode>
int height(AVLNode *root);

template <class AVLNode>
int balanceFactor(AVLNode *root);

template <class AVLNode>
AVLNode *leftRotate(AVLNode *root);

template <class AVLNode>
AVLNode *rightRotate(AVLNode *root);
}  // namespace AVL

#include "avl.cpp"