#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "utils.hpp"
using namespace std;

extern std::fstream header_file;
extern string in_file_name, out_file_name;

// Cada simbolo tem um codeword que sera escrito no arquivo de saida
struct Codeword
{
    unsigned char num; // numero de vezes que o simbolo se repete
    char symbol; // simbolo
};

/**
 * Executa o algoritmo Run-Length sobre um arquivo e escreve no
 * arquivo de saida
 */
void RunLength_encode()
{
    // cria um arquivo temporario de saida para armazenar a transformada de BWT
    fstream out_file(".temp.runl", fstream::out | fstream::binary);
    fstream in_file(".temp", fstream::in | fstream::binary);
    Codeword cw;
    while (true) {

        cw.num = 1;
        // simbolo para contar o numero de repeticoes
        in_file.read(&cw.symbol, sizeof (cw.symbol));
        
        // iteracao para um novo simbolo
        while (true) {
            char c = in_file.peek();
            if (cw.symbol != c) {
                break;
            }
            cw.num++;
            in_file.read(&c, sizeof(c));
            if (in_file.eof()) { // fim de arquivo
                break;
            }
        }
        // escreve no arquivo de saida
        out_file.write(reinterpret_cast<const char*> (&cw), sizeof (cw));
        if (in_file.eof()) { // fim de arquivo
            break;
        }
    }
    out_file.close();
    in_file.close();
    // copia tudo para o arquivo .temp
    in_file.open(".temp.runl", fstream::in | fstream::binary);
    out_file.open(".temp", fstream::out | fstream::binary);
    copy_file(out_file, in_file);
    in_file.close();
    std::remove(".temp.runl"); // deleta arquivo temporario
}
/**
 * Decodifica arquivo com a codificacao Run-Length e escreve num arquivo de saida 
 */
void RunLength_decode()
{
    // cria um arquivo temporario de saida para armazenar a transformada de BWT
    fstream out_file(".temp.runl", fstream::out | fstream::binary);
    fstream in_file(".temp", fstream::in | fstream::binary);
    Codeword cw;
    while (!in_file.eof()) {
        
        in_file.read(reinterpret_cast<char*>(&cw), sizeof(cw));
        if(in_file.eof()) {
            break;
        }
        char c = cw.symbol;
        for(int i = 0; i < cw.num; ++i) {
            out_file.write(&c, sizeof(c));
        }
    }
    in_file.close();
    out_file.close();
    
    // copia tudo para o arquivo .temp
    in_file.open(".temp.runl", fstream::in | fstream::binary);
    out_file.open(".temp", fstream::out | fstream::binary);
    copy_file(out_file, in_file);
    in_file.close();
    std::remove(".temp.runl"); // deleta arquivo temporario
}