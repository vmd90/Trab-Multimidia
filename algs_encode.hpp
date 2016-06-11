
/* 
 * File:   algs_encode.hpp
 * Author: victor
 *
 */

#ifndef ALGS_ENCODE_HPP
#define ALGS_ENCODE_HPP

/**
 * Codifica o arquivo de entrada para a codificacao BWT e escreve no de saida
 * @param block_size Tamanho do bloco
 */
void BWT_encode(int);
/**
 * Codifica o arquivo de entrada para a codificacao Huffman e escreve no de saida
 */
void Huffman_encode();
/**
 * Codifica o arquivo de entrada para a codificacao Run-length e escreve no de saida
 */
void RunLength_encode();

#endif /* ALGS_ENCODE_HPP */

