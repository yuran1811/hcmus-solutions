#include "avl.h"

template <class AVLNode, class K, class V>
AVLNode *AVL::newNode(K key, V value) {
  return new AVLNode(key, value);
}

template <class AVLNode, class K, class V>
AVLNode *AVL::insertNode(AVLNode *root, K key, V value) {
  if (!root) return newNode<AVLNode, K, V>(key, value);

  if (root->key < key)
    root->right = insertNode(root->right, key, value);
  else if (root->key > key)
    root->left = insertNode(root->left, key, value);
  else
    return root->value = value, root;

  root->height =
      1 + std::max(height<AVLNode>(root->left), height<AVLNode>(root->right));

  int balance = balanceFactor<AVLNode>(root);

  if (balance > 1 && key < root->left->key) return rightRotate<AVLNode>(root);
  if (balance < -1 && key > root->right->key) return leftRotate<AVLNode>(root);

  if (balance > 1 && key > root->left->key)
    return root->left = leftRotate<AVLNode>(root->left),
           rightRotate<AVLNode>(root);
  if (balance < -1 && key < root->right->key)
    return root->right = rightRotate<AVLNode>(root->right),
           leftRotate<AVLNode>(root);

  return root;
}

template <class AVLNode, class K>
AVLNode *AVL::searchNode(AVLNode *root, K key) {
  if (!root || root->key == key) return root;
  return searchNode(root->key < key ? root->right : root->left, key);
}

template <class AVLNode, class K>
AVLNode *AVL::deleteNode(AVLNode *root, K key) {
  if (!root) return nullptr;

  if (root->key < key)
    root->right = deleteNode(root->right, key), root;
  else if (root->key > key)
    root->left = deleteNode(root->left, key), root;
  else {
    if (root->right == nullptr || root->left == nullptr) {
      AVLNode *tmp = root->left ? root->left : root->right;
      if (!tmp) {
        tmp = root;
        root = nullptr;
      } else
        *root = *tmp;

      delete tmp;
    } else {
      AVLNode *tmp = findMin<AVLNode>(root->right);
      root->key = tmp->key;
      root->value = tmp->value;
      root->right = deleteNode(root->right, tmp->key);
    }
  }

  if (!root) return nullptr;

  root->height =
      1 + std::max(height<AVLNode>(root->left), height<AVLNode>(root->right));

  int balance = balanceFactor<AVLNode>(root);

  if (balance > 1 && balanceFactor<AVLNode>(root->left) >= 0)
    return rightRotate<AVLNode>(root);
  if (balance < -1 && balanceFactor<AVLNode>(root->right) <= 0)
    return leftRotate<AVLNode>(root);

  if (balance > 1 && balanceFactor<AVLNode>(root->left) < 0)
    return root->left = leftRotate<AVLNode>(root->left),
           rightRotate<AVLNode>(root);

  if (balance < -1 && balanceFactor<AVLNode>(root->right) > 0)
    return root->right = rightRotate<AVLNode>(root->right),
           leftRotate<AVLNode>(root);

  return root;
}

template <class AVLNode>
AVLNode *AVL::findMin(AVLNode *root) {
  if (!root) return nullptr;
  return root->left ? findMin(root->left) : root;
}

template <class AVLNode>
AVLNode *AVL::leftRotate(AVLNode *root) {
  AVLNode *rootRight = root->right;
  AVLNode *remainLeft = rootRight->left;

  rootRight->left = root;
  root->right = remainLeft;

  root->height =
      1 + std::max(height<AVLNode>(root->left), height<AVLNode>(root->right));
  rootRight->height = 1 + std::max(height<AVLNode>(rootRight->left),
                                   height<AVLNode>(rootRight->right));

  return rootRight;
}

template <class AVLNode>
AVLNode *AVL::rightRotate(AVLNode *root) {
  AVLNode *rootLeft = root->left;
  AVLNode *remainRight = rootLeft->right;

  rootLeft->right = root;
  root->left = remainRight;

  root->height =
      1 + std::max(height<AVLNode>(root->left), height<AVLNode>(root->right));
  rootLeft->height = 1 + std::max(height<AVLNode>(rootLeft->left),
                                  height<AVLNode>(rootLeft->right));

  return rootLeft;
}

template <class AVLNode>
int AVL::height(AVLNode *root) {
  return !root ? 0 : root->height;
}

template <class AVLNode>
int AVL::balanceFactor(AVLNode *root) {
  return !root ? 0 : height<AVLNode>(root->left) - height<AVLNode>(root->right);
}
