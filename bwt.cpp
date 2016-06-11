#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <string.h>
#include <stdlib.h>
#include "utils.hpp"
using namespace std;

extern std::fstream header_file;
extern string in_file_name, out_file_name;

// Classe usada para encapsular a matriz de permutacoes ordenada usado no algoritmo BWT
class PermutationMatrix
{
public:
    char **mStrings;
    int mSize;

    PermutationMatrix(int block_size)
    {
        mSize = block_size;
        mStrings = new char*[mSize];
        for (int i = 0; i < mSize; ++i) {
            mStrings[i] = new char[mSize];
        }
    }

    virtual ~PermutationMatrix()
    {
        for (int i = 0; i < mSize; ++i) {
            delete[] mStrings[i];
        }
        delete[] mStrings;
    }
    /**
     * Rotacao das mStrings (BWT Forward Transform)
     * @param block String block
     */
    void forward(string& block)
    {
        string aux = block;
        // primeira string
        for (int i = 0; i < mSize; ++i) {
            mStrings[0][i] = aux[i];
        }
        for (int i = 1; i < mSize; ++i) {
            aux = mStrings[i - 1];
            for (int j = 1; j < mSize; ++j) {
                mStrings[i][j - 1] = aux[j];
            }
            mStrings[i][mSize - 1] = aux[0]; // passando a primeira letra para a ultima posicao
        }
    }
    /**
     * Transformada BWT reversa
     * @param block Bloco a ser decodificado
     * @param index Indice do EOF ($)
     */
    //function inverseBWT (string s)
//   create empty table
//   repeat length(s) times
//       // first insert creates first column
//       insert s as a column of table before first column of the table
//       sort rows of the table alphabetically
//   return (row that ends with the 'EOF' character)
    void reverse(string& block, int index)
    {
        for(int i = 0; i < mSize; ++i) {  // limpando os buffers
            ::memset(&mStrings[i][0], 127, mSize*sizeof(char));
        }
        
        for(int i = mSize-1; i >= 0; --i) {
            for(int j = 0; j < mSize; ++j) {
                mStrings[j][i] = block[j];
            }
            sort();
        }
        
    }
    /**
     * Funcao de comparacao usada para ordenacao
     * @param p1 String 1
     * @param p2 String 2
     * @return 0 se strings sao iguais, maior que 0 se p1 < p2 e menor que 0
     * se p1 > p2 
     */
    static int compare(const void *p1, const void *p2)
    {
        const char *a = *(char* const*) p1;
        const char *b = *(char* const*) p2;
        return ::strcmp(a, b);
    }
    /**
     * Funcao de ordenacao das strings
     */
    void sort()
    {
        // ordenacao usando qsort
        ::qsort(&mStrings[0], mSize, sizeof (char*), compare);
    }
    /**
     * Imprime na tela a matriz de permutacao (para teste)
     */
    void print()
    {
        cout << "Strings:\n";
        for (int i = 0; i < mSize; ++i) {
            for (int j = 0; j < mSize; ++j) {
                cout << mStrings[i][j];
            }
            cout << endl;
        }
    }
};

void BWT_encode(int block_size)
{
    // escreve na header para o algoritmo BWT
    header_file.write(reinterpret_cast<const char*>(&block_size), sizeof(block_size));
    
    // cria um arquivo temporario de saida para armazenar a transformada de BWT
    fstream out_file(".temp.bwt", fstream::out | fstream::binary);
    fstream in_file(".temp", fstream::in | fstream::binary);
    
    while (true) {
        string block(block_size, 0);
        in_file.read(&block[0], block_size*sizeof(char)); // leitura de um bloco no arquivo
        if (in_file.eof()) { // fim de arquivo
            break;
        }
        
        PermutationMatrix m(block_size); // criacao da matriz de permutacoes
        m.forward(block);
        m.sort();
        //m.print();
        string L; // ultima coluna da matriz de permutacoes
        for (int i = 0; i < m.mSize; ++i) {
            L += m.mStrings[i][m.mSize - 1]; // preenchendo L
        }

        unsigned short index = -1; // indice do simbolo $ (2 bytes)
        for (unsigned short i = 0; i < m.mSize; ++i) {
            if (block.find(m.mStrings[i]) != string::npos) {
                index = i;
                break;
            }
        }
        //cout <<"L = " << L << "\nindex = " << index << endl;
        // escreve no arquivo de saida
        out_file.write(&L[0], L.size()*sizeof(char));
        out_file.write(reinterpret_cast<const char*> (&index), sizeof (index));
    }
    out_file.close();
    in_file.close();
    
    // copia tudo para o arquivo .temp
    in_file.open(".temp.bwt", fstream::in | fstream::binary);
    out_file.open(".temp", fstream::out | fstream::binary);
    copy_file(out_file, in_file);
    in_file.close();
    out_file.close();
    std::remove(".temp.bwt"); // deleta arquivo temporario
}

void BWT_decode(int block_size)
{
    // cria um arquivo temporario de saida para armazenar a transformada de BWT
    fstream out_file(".temp.bwt", fstream::out | fstream::binary);
    fstream in_file(".temp", fstream::in | fstream::binary);
    
    while (true) {
        unsigned short index = -1;
        string block(block_size, 0);
        in_file.read(&block[0], block_size*sizeof(char)); // leitura de um bloco no arquivo
        in_file.read(reinterpret_cast<char*>(&index), sizeof(index));
        if (in_file.eof()) {
            break;
        }

        //cout << "L = " << block << "\nindex = " << index;
        PermutationMatrix m(block_size); // criacao da matriz de permutacoes
        m.reverse(block, index);
        m.sort();
        //m.print();
        //cout << "String decod: " << m.mStrings[index];
        // escreve a decodificacao
        out_file.write(&m.mStrings[index][0], m.mSize*sizeof(char));
    }
    out_file.close();
    in_file.close();
    
    // copia tudo para o arquivo .temp
    in_file.open(".temp.bwt", fstream::in | fstream::binary);
    out_file.open(".temp", fstream::out | fstream::binary);
    copy_file(out_file, in_file);
    in_file.close();
    std::remove(".temp.bwt"); // deleta arquivo temporario
}