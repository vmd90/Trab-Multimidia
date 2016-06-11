/* 
 * File:   algs_decode.hpp
 * Author: victor
 *
 */

#ifndef ALGS_DECODE_HPP
#define ALGS_DECODE_HPP
#include <map>
/**
 * Decodifica o arquivo de entrada na codificacao BWT e escreve no de saida
 * @param block_size Tamanho do bloco
 */
void BWT_decode(int block_size);
/**
 * Decodifica o arquivo de entrada na codificacao Huffman e escreve no de saida
 * @param code Tabela de codigos
 * @param fsize Tamanho do arquivo original
 */
void Huffman_decode(const std::map<std::string, char>& code, unsigned int fsize);
/**
 * Decodifica o arquivo de entrada na codificacao Run-length e escreve no de saida
 */
void RunLength_decode();

#endif /* ALGS_DECODE_HPP */