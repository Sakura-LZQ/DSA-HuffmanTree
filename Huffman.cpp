#include <bits/stdc++.h>
using namespace std;

//哈夫曼节点
struct Node {
    char data;//字符
    unsigned int freq;//出现的频率，即权重
    Node *left, *right;//指向左右儿子

    Node(char data, unsigned int freq) {//构造函数
        left = right = NULL;
        this->data = data;
        this->freq = freq;
    }
};

//传给priority_queue的谓词
struct compare {
    bool operator()(Node* l, Node* r) {
        return (l->freq > r->freq);
    }
};

//打印每个字符的哈夫曼编码，针对无法显示的字符做了优化
void printCodes(Node* root, string str = "") {
    if (!root)
        return;

    if (root->data != '\0')
        if (root->data == '\n')
            cout << "\\n" << ": " << str << endl;
        else if (root->data == ' ')
            cout << "sp" << ": " << str << endl;
        else if (root->data == '\t')
            cout << "\\t" << ": " << str << endl;
        else if (root->data == '\r')
            cout << "\\r" << ": " << str << endl;
        else 
            cout << root->data << ": " << str << endl;
    else {
        printCodes(root->left, str + "0");
        printCodes(root->right, str + "1");
    }
}

//给定data，返回他的哈夫曼编码
string getCode(Node* root, char data, string str = "") {
    if (!root)
        return "";

    if (root->data != '\0') {
        if (root->data == data) {
            return str;
        }
    } else {
        string left = getCode(root->left, data, str + "0");
        string right = getCode(root->right, data, str + "1");
        if (left != "") {
            return left;
        } else if (right != "") {
            return right;
        }
    }
    return "";
}

int main() {
    vector<char> v, v2;
    string str = "";
    priority_queue<Node*, vector<Node*>, compare> huffmanTree;
    char ch;
    unsigned int sizePre = 0, sizeAfter = 0;
    //按字符读取每一个字符
    ifstream fin("huffman.txt");
    while (fin.get(ch)) {
        v.push_back(ch);
        sizePre++;
    }
    fin.close();
    cout << "The content of the original file is: " << endl;
    for (auto &i : v) {
        cout << i;
    }
    cout << endl;
    cout << endl;
    v2 = v;//对v2操作，留存v作为原文章
    stable_sort(v2.begin(), v2.end());
    //记录每个字符的频率，然后构造Node
    auto it = v2.begin();
    while (it != v2.end()) {
        int count = 0;
        char ch = *it;
        while (it != v2.end() && *it == ch) {
            count++;
            it++;
        }
        Node* temp = new Node(ch, count);
        huffmanTree.push(temp);
    }
    v2.clear();
    //构造哈夫曼树，取权值最小的两个节点，最终构造一个最大堆
    if (huffmanTree.size() == 1) {
        Node* temp = new Node('\0', 0);
        temp->left = huffmanTree.top();
        huffmanTree.pop();
        huffmanTree.push(temp);
    }
    while (huffmanTree.size() != 1) {
        Node *left = huffmanTree.top();
        huffmanTree.pop();
        Node *right = huffmanTree.top();
        huffmanTree.pop();
        Node *top = new Node('\0', left->freq + right->freq);
        top->left = left;
        top->right = right;
        huffmanTree.push(top);
    }

    cout << "Huffman Codes are: " << endl;
    printCodes(huffmanTree.top());
    //先将原文件的编码存储下来，然后八位分割实现压缩
    ofstream outfile("huffmanCode.hf", ios::binary | ios::out);
    for (auto it = v.begin(); it != v.end(); it++) {
        str += getCode(huffmanTree.top(), *it);
    }
    for (int i = 0; i < str.size(); i += 8) {
        unsigned char c = 0;
        for (int j = 0; j < 8; j++) {
            if (i + j >= str.size()) {
                break;
            } else {
                if (str[i + j] == '1') {
                    c |= 1 << (7 - j);
                }
            }
        }
        outfile.write((char*)&c, sizeof(c));
        sizeAfter++;
    }
    outfile.close();
    //解码只需要对01进行左右孩子寻址找到叶节点即可
    ifstream infile("huffmanCode.hf", ios::binary | ios::in);
    outfile.open("huffmanDecode.txt", ios::out);
    cout << endl;
    cout << "Huffman Decode: " << endl;
    Node *root = huffmanTree.top();
    for (auto i = 0; i < str.size() / 8 + 1; i++) {
        unsigned char c = 0;
        infile.read((char*)&c, sizeof(c));
        for (int j = 0; j < 8; j++) {
            if (i * 8 + j >= str.size()) {
                break;
            }
            if (c & (1 << (7 - j))) {
                root = root->right;
            } else {
                root = root->left;
            }
            if (root->data != '\0') {
                outfile << root->data;
                v2.push_back(root->data);
                root = huffmanTree.top();
            }
        }
    }
    for (auto &i : v2) {
        cout << i;
    }
    cout << endl;
    cout << endl;
    //比对解码是否成功
    for (auto i = 0; i < v.size(); i++) {
        if (v.at(i) != v2.at(i)) {
            cout << "Decoding failed!" << endl;
            return 0;
        }
    }
    cout << "Decoding succeeded!" << endl;
    cout << endl;
    cout << "The size of the original file is: " << sizePre << " bytes" << endl;
    cout << "The size of the compressed file is: " << sizeAfter << " bytes" << endl;
    cout << "Compression ratio = " << double(sizeAfter) / sizePre * 100 << "%" << endl;
    return 0;
}
