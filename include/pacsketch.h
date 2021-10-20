/*
 * Name: pacsketch.h
 * Description: Header file for pacsketch.cpp
 * Project: pacsketch project.
 * 
 * Author: Omar Ahmed
 * Date: October 17, 2021
 */

#ifndef  _PACSKETCH_H
#define  _PACSKETCH_H

#define FATAL_WARNING(x) do {std::fprintf(stderr, "Warning: %s\n", x); std::exit(1);} while (0)
#define THROW_EXCEPTION(x) do { throw x;} while (0)

/* Function Declarations */
bool is_file(const char* file_path);

#endif /* end of _PACSKETCH_H header */