#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <queue>
#include <stack>
#include <string.h>
#include <exception>
#include "utils.hpp"

#define MAX_BUFFER      1024
using namespace std;

extern std::fstream header_file;
extern string in_file_name, out_file_name;

// Classe responsavel por armazenar todos os simbolos do arquivo analisado para
// aplicar o algoritmo de compressao Huffman

class SymbolTable
{
public:
    // Tabela de todos os simbolos que aparecem no arquivo.
    // Caractere encontrado e sua frequencia
    map<char, int> mSymbols;

    SymbolTable()
    {
    }

    virtual ~SymbolTable()
    {
    }
    // Busca na tabela de simbolos pelo simbolo 'c'

    bool hasSymbol(const char& c)
    {
        return mSymbols.find(c) != mSymbols.end();
    }

    void addSymbol(char c)
    {
        // simbolo nao inserido na tabela, entao insere
        if (!hasSymbol(c)) {
            mSymbols[c] = 1;
        }
        else {
            // Senao, incrementa sua frequencia
            mSymbols[c]++;
        }
    }

    void print()
    {
        cout << "Map:\n";
        for (auto i = mSymbols.begin(); i != mSymbols.end(); ++i) {
            char c = i->first;
            switch (c)
            {
            case '\n':
                cout << "\\n";
                break;
            case '\r':
                cout << "\\r";
                break;
            case ' ':
                cout << "space";
                break;
            default:
                if (c >= 0x0 && c <= 0x20) { // caracteres de controle
                    cout << "control " << int(c);
                }
                else {
                    cout << c;
                }
            }
            cout << ": " << i->second << endl;
        }
    }
};

// Struct usada para a construcao da arvore de Huffman

struct Node
{
    string symbols; // conjunto de simbolos
    int frequency; // frequencia
    bool is_leaf; // eh leaf node
    Node *left, *right;

    Node(string s, int f, bool l) : symbols(s), frequency(f), is_leaf(l), left(nullptr), right(nullptr)
    {
    }
};

class HuffmanTree
{
private:
    // raiz da arvore
    Node *mRoot;
public:

    HuffmanTree() : mRoot(nullptr)
    {
    }

    virtual ~HuffmanTree()
    {
        deleteNode(mRoot);
    }

    /**
     * Limpa a arvore
     * @param n No a ser deletado
     */
    void deleteNode(Node *n)
    {
        if (n) {
            deleteNode(n->left);
            deleteNode(n->right);
            delete n;
        }
    }
    // Usado para a ordenacao de vetor

    struct PCompare
    {

        bool operator()(const Node* left, const Node* right) const
        {
            return left->frequency > right->frequency;
        }
    };

    /**
     * Gera a arvore de Huffman a partir das frequencias dos simbolos
     * @param st Tabela de simbolos
     */
    void generate(SymbolTable& st)
    {
        // vetor ordenado pela frequencia maior
        priority_queue<Node*, vector<Node*>, PCompare> q;
        // preenchendo a fila
        for (auto i = st.mSymbols.begin(); i != st.mSymbols.end(); ++i) {
            string s;
            s.push_back(i->first);
            q.push(new Node(s, i->second, true));
        }
        // montando a arvore
        Node *n, *m;
        while (q.size() > 0) {
            // remove dois nos
            n = q.top();
            q.pop();
            m = q.top();
            q.pop();
            // cria um no pai
            string s("*");
            int f = n->frequency + m->frequency;
            Node *parent = new Node(s, f, false);
            parent->left = n;
            parent->right = m;
            mRoot = parent;
            // fila vazia
            if (q.size() == 0) {
                break;
            }
            // reinsere em q o pai e reordena
            q.push(parent);
        }
    }

    /**
     * Gera os codigos de Huffman
     * @param codes Tabela com os codigos gerados
     */
    void getCodes(map<char, string>& codes)
    {
        Node *n;
        // duas pilhas, uma acompanha a outra
        stack<string> sCode; // pilha de string usada para manter os codigos de cada no
        stack<Node*> sNode; // pilha usada para 

        sCode.push("");
        sNode.push(mRoot);
        while (!sNode.empty()) {
            n = sNode.top();
            sNode.pop();

            if (n->is_leaf) {
                char c = n->symbols[0];
                codes[c] = sCode.top();
                sCode.pop();
            }
            else {
                string sl = sCode.top(); // string do codigo a esquerda
                string sr(sl); // string do codigo a direita

                sCode.pop();
                sl.push_back('0'); // anexa 0 no codigo
                sr.push_back('1'); // anexa 1 no codigo
                // insercao dos nos a direita e a esquerda nas duas pilhas
                sCode.push(sr);
                sCode.push(sl);
                sNode.push(n->right);
                sNode.push(n->left);
            }
        }
    }
};

/**
 * Escreve no arquivo o buffer de tamanho 8 (converte de string para 1 byte)
 */
void write_file(fstream& out_file, const char *buffer)
{
    char num = 0;
    for (int j = 0, k = 6; k >= 0; j++, k--) {
        if (buffer[k] == '1') {
            num += pow(2, j);
        }
    }
    out_file.write(reinterpret_cast<const char*> (&num), sizeof (num));
}
/**
 * Converte um byte e retorna um buffer de tamanho 8 (caracteres)
 * @param byte 
 * @param buffer
 */
void convert_int2str(char num, string& buffer)
{
    string str(7, '0');
    for(int i = 0; i < buffer.size(); ++i) {
        char m = num % 2;
        if(num < 2) {
            buffer[i] = num == '\0' ? '0' : '1';
            break;
        }
        buffer[i] = m == '\0' ? '0' : '1';
        num /= 2;
    }
    str = buffer;
    for(int i = buffer.size()-1, j = 0; i >= 0 && j < str.size(); i--, j++) {
        buffer[i] = str[j];
    }
}

void Huffman_encode()
{
    unsigned char num_symbols = 0; // numero de simbolos do alfabeto
    unsigned int fsize; // tamanho do arquivo de entrada
    // cria um arquivo temporario de saida para armazenar a transformada de BWT
    fstream out_file(".temp.huff", fstream::out | fstream::binary);
    fstream in_file(".temp", fstream::in | fstream::binary);

    in_file.seekg(0, ios::end);
    fsize = in_file.tellg();
    in_file.seekg(0, ios::beg);
    // escreve fsize em header
    header_file.write(reinterpret_cast<const char*>(&fsize), sizeof(fsize));

    // ler arquivo e analisar a frequencia de cada simbolo
    SymbolTable st;
    while (true) {
        char c;
        in_file.read(&c, sizeof(c)); // lendo byte a byte
        st.addSymbol(c); // insere novo simbolo na tabela
        if (in_file.eof()) { // fim de arquivo
            break;
        }
    }
    in_file.close();

    map<char, string> codes;
    {
        HuffmanTree tree;
        tree.generate(st);
        tree.getCodes(codes);
    }
    num_symbols = codes.size();
    // escreve hsize no arquivo
    header_file.write(reinterpret_cast<const char*>(&num_symbols), sizeof(num_symbols));
    // escreve no arquivo a tabela de codigos
    // simbolo + codigo + \0
    for (auto i = codes.begin(); i != codes.end(); ++i) {
        char cz = 0;
        header_file.write(&i->first, sizeof(i->first));
        header_file.write(reinterpret_cast<const char*>(&i->second[0]), i->second.size()*sizeof(char));
        header_file.write(&cz, sizeof (cz));
    }

    char c;
    string code;
    char buffer[8] = {0}; // escreve conteudo codificado (8bits a 8bits)
    int index = 0; // posicao no buffer
    bool overflow = false;
    int ov_size = 0; // tamanho de overflow
    int ov_index = 0;
    in_file.open(".temp", fstream::in | fstream::binary);
    while (true) {
        if (overflow) {
            overflow = false;
            if (ov_size > 8) {
                overflow = true;
            }
            code = codes[c].substr(ov_index);
        }
        else {
            in_file.read(&c, sizeof (c)); // lendo byte a byte
            if (in_file.eof()) { // fim de arquivo
                // escrever o ultimo buffer
                write_file(out_file, buffer);
                break;
            }
            code = codes[c];
        }

        if (code.size() <= (8 - index)) { // buffer nao cheio
            // copia codigo em buffer
            for (int i = index, j = 0; i < 8 && j < code.size(); ++i, ++j) {
                buffer[i] = code[j];
            }
            index += code.size();
        }
        else {
            if (index == 8) { // buffer cheio, escreve no arquivo
                // escreve buffer cheio
                write_file(out_file, buffer);
                // inicia um novo buffer como overflow
                overflow = true;
                ov_index = 0;
                ov_size = code.size();                        
                ::memset(&buffer[0], 0, 8 * sizeof (char));
                index = 0;
            }
            else { // buffer nao cheio, o codigo nao cabe mais
                // escreve uma parte do codigo no buffer e passa para o arquivo
                overflow = true;
                int j = 0;
                for (int i = index; i < 8 && j < code.size(); ++i, ++j) {
                    buffer[i] = code[j];
                }
                ov_index = j;
                ov_size = code.size() - j;
                write_file(out_file, buffer);
                ::memset(&buffer[0], 0, 8 * sizeof (char));
                index = 0;
            }
            out_file.flush();
        }
    }
    out_file.close();
    in_file.close();

    // copia tudo para o arquivo .temp
    in_file.open(".temp.huff", fstream::in | fstream::binary);
    out_file.open(".temp", fstream::out | fstream::binary);
    copy_file(out_file, in_file);
    in_file.close();
    out_file.close();
    std::remove(".temp.huff"); // deleta arquivo temporario
}

void Huffman_decode(const map<string, char>& codes, unsigned int fsize)
{
    fstream out_file(".temp.huff", fstream::out | fstream::binary);
    fstream in_file(".temp", fstream::in | fstream::binary);
    unsigned int nbytes = 0;  // numero de bytes do arquivo original
    unsigned char c, symbol;
    string buffer(8, '0');
    string str;

    while( nbytes < fsize )
    {
        in_file.read(reinterpret_cast<char*>(&c), sizeof(c));
        if(in_file.eof()) {
            break;
        }
        
        convert_int2str(c, buffer);
        
        for(int i = 0; i < buffer.size(); ++i) {  // Le todo o buffer para extrair o simbolo
            try {
                str.push_back(buffer[i]);

                symbol = codes.at(str);
                
                out_file.write(reinterpret_cast<const char*>(&symbol), sizeof(symbol));
                nbytes++;
                str.clear();
            }
            catch(exception&) {
                continue;
            }
        }
    }

    out_file.close();
    in_file.close();

    // copia tudo para o arquivo .temp
    in_file.open(".temp.huff", fstream::in | fstream::binary);
    out_file.open(".temp", fstream::out | fstream::binary);
    copy_file(out_file, in_file);
    in_file.close();
    out_file.close();
    std::remove(".temp.huff"); // deleta arquivo temporario
}
