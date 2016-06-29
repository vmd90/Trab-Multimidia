/* 
 * File:   utils.hpp
 * Author: victor
 *
 */
#ifndef UTILS_HPP
#define UTILS_HPP

/**
 * Copia o conteudo inteiro de um arquivo para o outro
 * @param out Arquivo de saida
 * @param in Arquivo de entrada
 */
void copy_file(std::fstream&, std::fstream&);
/**
 * Calcula a potencia
 * @param b Base
 * @param e Expoente
 * @return potencia calculada
 */
char power(char b, char e);

#endif /* UTILS_HPP */

