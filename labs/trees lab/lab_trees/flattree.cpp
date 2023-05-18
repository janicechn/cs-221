using namespace std;

template <typename T>
void FlatTree<T>::printLeftToRight() const {
  const int n = data.size();
  function<void(int)> helper = [&](int i) {
    if (i >= n) return;
    helper(i*2);
    cout << data[i] << " ";
    helper(i*2+1);
  };
  helper(1);
  cout << endl;
}

template <typename T>
T& FlatTree<T>::getElement(const string& path) {
  int i=1;
  for (char c : path) {
    i *= 2;
    i += (c=='R');
  }
  return data[i];
}

template <typename T>
void FlatTree<T>::fromBinaryTree(const BinaryTree<T>& tree) {
  data.clear();
  using Node = typename BinaryTree<int>::Node;
  function<void(Node*,int)> helper = [&](Node* v,int i) {
    if (!v) return;
    if (i >= (int)data.size()) data.resize(i+1);
    data[i] = v->elem;
    helper(v->left,i*2);
    helper(v->right,i*2+1);
  };
  helper(tree.root,1);
}

template <typename T>
BinaryTree<T> FlatTree<T>::toBinaryTree() const {
  using Node = typename BinaryTree<T>::Node;
  const int n = data.size();
  BinaryTree<T> tree;
  function<void(int,Node*&)> helper = [&](int i, Node*& v) {
    if (i >= n) return;
    v = new Node(data[i]);
    helper(i*2,v->left);
    helper(i*2+1,v->right);
  };
  helper(1,tree.root);
  return tree;
}

