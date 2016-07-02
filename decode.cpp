#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <ctime>
#include "algs_decode.hpp"
#include "utils.hpp"
using namespace std;

void help();

fstream header_file;
string in_file_name, out_file_name;

int main(int argc, char** argv)
{
    // Para poder executar, eh preciso 5 argumentos
    if (argc != 5) {
        help();
        return 0;
    }

    string arg;
    bool error = false;

    // leitura dos parametros do programa
    for (int i = 1; i < argc; ++i) {
        arg = argv[i];
        if (string("-i") == arg) {
            // Abrindo arquivo de entrada
            ++i;
            in_file_name = argv[i];
        }
        else if (string("-o") == arg) {
            // Abrindo arquivo de saida
            ++i;
            out_file_name = argv[i];
        }
        else {
            cerr << "Error found: " << arg << " in argument " << i << endl;
            error = true;
            break;
        }
    }
    // Erro, abortar
    if (error) {
        return 1;
    }
    
    int txtblck = 0;
    std::map<string,char> codes;
    unsigned int fsize;  // tamanho do arquivo original

    // Header do arquivo comprimido
    char algs[3]; // algoritmos usados em ordem, ex: 'bhr' (BWT, Huffman, RLE)
    fstream in_file(in_file_name, fstream::in | fstream::binary);
    {
        in_file.read(&algs[0], 3*sizeof(char));
        
        // separando a header do conteudo
        header_file.open(".header", fstream::out | fstream::binary);
        header_file.write(&algs[0], 3*sizeof(char));
        for(int i = 0; i < 3; ++i) {
            if(algs[i] == 'b') {
                in_file.read(reinterpret_cast<char*>(&txtblck), sizeof(txtblck));
                header_file.write(reinterpret_cast<const char*>(&txtblck), sizeof(txtblck));
            }
            else if(algs[i] == 'h') {
                unsigned char num_symbols;
                in_file.read(reinterpret_cast<char*>(&fsize), sizeof(fsize));
                in_file.read(reinterpret_cast<char*>(&num_symbols), sizeof(num_symbols));
                header_file.write(reinterpret_cast<const char*>(&fsize), sizeof(fsize));
                header_file.write(reinterpret_cast<const char*>(&num_symbols), sizeof(num_symbols));
                // lendo os codigos
                for(unsigned char i = 0; i < num_symbols; ++i) {
                    char ch;
                    string str;
                    in_file.read(&ch, sizeof(ch));
                    if(in_file.eof()) {
                        break;
                    }
                    char cd;
                    while(true) {
                        in_file.read(&cd, sizeof(cd));
                        if(in_file.eof() || cd == '\0') {
                            break;
                        }
                        str.push_back(cd);
                    }
                    codes[str] = ch;
                }
            }
        }
    }
    // arquivo temporario identico ao de entrada para o processo de codificacao
    {
        fstream temp(".temp", fstream::out | fstream::binary);
        copy_file(temp, in_file);
    }
    // Tempo
    time_t start, end;
    std::time(&start);
    // Decodificacao
    for (int i = 2; i >= 0; --i) {
        if (algs[i] == 'b') {
            BWT_decode(txtblck);
        }
        else if (algs[i] == 'h') {
            Huffman_decode(codes, fsize);
        }
        else if (algs[i] == 'r') {
            RunLength_decode();
        }
    }
    // Tempo 
    std::time(&end);
    cout << "Elapsed time: " << end - start << "s" << endl;
    // Escrita de todos os dados decodificados para o arquivo de saida
    {
        header_file.close();
        
        fstream out_file(out_file_name, fstream::out | fstream::binary);
        // copia do arquivo temporario para o arquivo de saida
        fstream temp(".temp", fstream::in | fstream::binary);
        copy_file(out_file, temp);
        temp.close();
        out_file.close();
        // deletar temporarios
        std::remove(".header");
        std::remove(".temp");
    }
    return 0;
}

// Mensagem de ajuda em como usar o programa
void help()
{
    cout << "\nHow to use:\n"
            "     ./decode -i arq_entrada.txt -o arq_saida.bin\n"
            "Options:\n"
            " -i <arq>    - input file\n"
            " -o <arq>    - output file\n" << endl;
}
