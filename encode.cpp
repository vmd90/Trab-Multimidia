#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <stdlib.h>
#include <vector>
#include <string.h>
#include "algs_encode.hpp"
#include "utils.hpp"
using namespace std;

// Parametros de entrada
// ./encode -i arq_entrada.txt -o arq_saida.bin --bwt=X --txtblck=N --huffman=X --runl=X
// onde N eh numero, X eh true ou false

void help();

char algs[3];  // algoritmos usados em ordem, ex: 'bhr' (BWT, Huffman, RLE)
fstream in_file, out_file; // arquivos de entrada e saida
fstream header_file;  // arquivo para compor a header do arquivo de saida
string in_file_name, out_file_name; // nome dos arquivos

int main(int argc, char const *argv[])
{
    // Para poder executar, eh preciso mais de 6 parametros de entrada
    if (argc < 6) {
        help();
        return 0;
    }

    int txtblck;
    bool use_bwt = false, use_huffman = false, use_runl = false;
    bool error = false;
    string arg;
    int j = 0;
    
    ::memset(algs, 'n', 3*sizeof(char));

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
        else if (arg.find("--bwt") != string::npos) {
            // Extrai o parametro bwt, true ou false
            istringstream ss(arg);
            string s;
            std::getline(ss, s, '=');
            std::getline(ss, s, '='); // Duas vezes para pegar a ultima string
            if (s.empty()) {
                cerr << "Incomplete argument: " << arg << endl;
                error = true;
                break;
            }
            if (s != "true" && s != "false") {
                cerr << "Error found: " << arg << " in argument " << i << endl;
                error = true;
                break;
            }
            else {
                // Pega o valor bool
                use_bwt = s == "true" ? true : false;
            }
            algs[j++] = 'b';
        }
        else if (arg.find("--txtblck") != string::npos) {
            // Extrai o parametro txtblck, true ou false
            istringstream ss(arg);
            string s;
            std::getline(ss, s, '=');
            std::getline(ss, s, '='); // Duas vezes para pegar a ultima string
            if (s.empty()) {
                cerr << "Incomplete argument: " << arg << endl;
                error = true;
                break;
            }
            // Pega o valor numerico
            txtblck = atoi(s.c_str());
        }
        else if (arg.find("--huffman") != string::npos) {
            // Extrai o parametro huffman, true ou false
            istringstream ss(arg);
            string s;
            std::getline(ss, s, '=');
            std::getline(ss, s, '='); // Duas vezes para pegar a ultima string
            if (s.empty()) {
                cerr << "Incomplete argument: " << arg << endl;
                error = true;
                break;
            }
            if (s != "true" && s != "false") {
                cerr << "Error found: " << arg << " in argument " << i << endl;
                error = true;
                break;
            }
            else {
                // Pega o valor bool
                use_huffman = s == "true" ? true : false;
            }
            algs[j++] = 'h';
        }
        else if (arg.find("--runl") != string::npos) {
            // Extrai o parametro runl, true ou false
            istringstream ss(arg);
            string s;
            std::getline(ss, s, '=');
            std::getline(ss, s, '='); // Duas vezes para pegar a ultima string
            if (s.empty()) {
                cerr << "Incomplete argument: " << arg << endl;
                error = true;
                break;
            }
            if (s != "true" && s != "false") {
                cerr << "Error found: " << arg << " in argument " << i << endl;
                error = true;
                break;
            }
            else {
                // Pega o valor bool
                use_runl = s == "true" ? true : false;
            }
            algs[j++] = 'r';
        }
        else {
            cerr << "Error found: " << arg << " in argument " << i << endl;
            error = true;
            break;
        }
    }

    // Caso erro, abortar imediatamente
    if (error) {
        return 1;
    }

    // Se todos as flags (bwt, huffman, runl) sao false, nao pode executar
    if (!use_bwt && !use_huffman && !use_runl) {
        help();
        return 1;
    }
    
    // Header basica do arquivo codificado
    {
        header_file.open(".header", fstream::out | fstream::binary);
        if(!header_file.is_open()) {
            cerr << "Error opening file" << endl;
            return 1;
        }
        header_file.write(&algs[0], 3*sizeof(char));  // ordem dos algoritmos usados
    }
    // arquivo temporario identico ao de entrada para o processo de codificacao
    {
        fstream temp(".temp", fstream::out | fstream::binary);
        fstream in_file(in_file_name, fstream::in | fstream::binary);
        copy_file(temp, in_file);
    }
    // Compressao dos dados
    for (int i = 0; i < 3; ++i) {
        if (algs[i] == 'b') {
            BWT_encode(txtblck);
        }
        else if (algs[i] == 'h') {
            Huffman_encode();
        }
        else if (algs[i] == 'r') {
            RunLength_encode();
        }
    }
    // Escrita de todos os dados codificados para o arquivo de saida
    {
        header_file.close();
        fstream out_file(out_file_name, fstream::out | fstream::binary);
        header_file.open(".header", fstream::in | fstream::binary);
        // copia a header para o arquivo de saida
        copy_file(out_file, header_file);
        // copia do arquivo temporario para o arquivo de saida
        fstream temp(".temp", fstream::in | fstream::binary);
        copy_file(out_file, temp);
        temp.close();
        // deletar temporarios
        std::remove(".header");
        std::remove(".temp");
    }
    return 0;
}

/**
 * Mensagem de ajuda em como usar o programa
 */
void help()
{
    cout << "\nHow to use:\n"
            "     ./encode -i arq_entrada.txt -o arq_saida.bin --bwt=X --txtblck=N --huffman=X --runl=X\n"
            "Options:\n"
            " -i <arq>    - input file\n"
            " -o <arq>    - output file\n"
            " --bwt=X     - enable BWT algorithm (true or false)\n"
            " --txtblck=N - block size (integer)\n"
            " --huffman=X - enable Huffman compression (true or false)\n"
            " --runl=X    - enable Run-length compression (true or false)\n" << endl;
}
