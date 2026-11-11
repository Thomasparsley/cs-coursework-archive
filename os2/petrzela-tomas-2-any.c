//
// author: Tomáš Petržela
// e-mail: tomas.petrzela02 at upol.cz
//
// pgmtoascii - convert a PGM image to ASCII art
//
// pgmtoascii is a simple program that converts a PGM image to ASCII art. The program
// takes three arguments: the name of the input PGM file and the name of the output
// ASCII file. The third argument is a string of ASCII characters that will be used to
// represent the different gray levels in the image.
//
// pgm file support comments. Comments are lines that start with #.
//
// The program is first written to work in linux. Then it is modified to work in
// windows.
//
// Header file of pgm contains the following definitions:
//     P2                   - PGM file marker
//     x y                  - resolution of the image
//     n                    - number of gray levels
//
// Usage:
//      ./pgmtoascii [input file] [output file] [character set]
//
// Usage example:
//      ./pgmtoascii input.pgm output.txt " .-+=o*O#@"
//
// Example of input:
//      P2
//      32 37
//      9
//      0 0 0 0 0 0 0 0 0 0 0 0 0 1 1 2 2 2 1 0 0 0 0 0 0 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 0 0 0 1 2 2 2 2 2 2 2 1 0 0 0 0 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 0 0 1 2 2 2 2 2 2 2 2 2 0 0 0 0 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 0 0 2 2 2 2 2 2 2 2 2 2 1 0 0 0 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 0 0 2 2 2 2 2 2 2 2 2 2 2 0 0 0 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 0 0 2 5 5 2 2 5 8 7 2 2 2 0 0 0 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 0 0 3 8 8 4 2 8 7 8 5 2 2 1 0 0 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 0 0 4 6 5 6 2 7 1 6 6 2 2 1 0 0 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 0 0 3 7 5 7 6 7 2 7 5 2 2 1 0 0 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 0 0 2 7 7 7 7 7 7 7 3 2 2 1 0 0 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 0 0 5 7 7 7 7 7 6 6 5 2 2 1 0 0 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 0 0 4 6 6 6 6 6 7 7 7 2 2 2 0 0 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 0 0 2 7 7 7 7 7 8 8 8 3 2 2 1 0 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 0 1 2 8 8 8 8 8 8 8 8 6 2 2 2 1 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 1 2 4 8 8 8 8 8 8 8 8 8 2 2 2 2 0 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 0 2 2 6 8 8 8 8 8 8 8 8 8 4 2 2 2 1 0 0 0 0 0 0
//      0 0 0 0 0 0 0 0 1 2 3 8 8 8 8 8 8 8 8 8 8 7 2 2 2 2 1 0 0 0 0 0
//      0 0 0 0 0 0 0 1 2 2 7 8 8 8 8 8 8 8 8 8 8 8 3 3 2 2 2 0 0 0 0 0
//      0 0 0 0 0 0 0 2 2 5 8 8 8 8 8 8 8 8 8 8 8 8 6 3 5 2 2 1 0 0 0 0
//      0 0 0 0 0 0 1 2 4 8 8 8 8 8 8 8 8 8 8 8 8 8 8 2 5 5 2 2 0 0 0 0
//      0 0 0 0 0 0 1 6 5 8 8 8 8 8 8 8 8 8 8 8 8 8 8 4 2 7 2 2 1 0 0 0
//      0 0 0 0 0 1 2 6 7 8 8 8 8 8 8 8 8 8 8 8 8 8 8 6 2 7 2 2 1 0 0 0
//      0 0 0 0 0 1 2 6 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 6 2 7 2 2 2 0 0 0
//      0 0 0 0 0 1 2 7 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 7 5 7 2 2 2 0 0 0
//      0 0 0 0 0 4 4 7 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 7 8 6 2 2 2 0 0 0
//      0 0 0 0 5 7 7 4 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 8 6 3 2 2 1 0 0 0
//      0 0 0 4 7 7 7 7 4 7 8 8 8 8 8 8 8 8 8 8 8 8 7 6 3 3 4 6 7 1 0 0
//      2 4 6 7 7 7 7 7 6 2 6 8 8 8 8 8 8 8 8 8 8 8 7 7 3 4 7 7 7 3 0 0
//      6 7 7 7 7 7 7 7 7 5 2 5 8 8 8 8 8 8 8 8 8 8 7 7 7 7 7 7 7 5 0 0
//      6 7 7 7 7 7 7 7 7 7 3 2 6 8 8 8 8 8 8 8 8 7 7 7 7 7 7 7 7 7 4 0
//      4 7 7 7 7 7 7 7 7 7 6 3 7 8 8 8 8 8 8 8 8 7 7 7 7 7 7 7 7 7 7 6
//      4 7 7 7 7 7 7 7 7 7 7 5 8 8 8 8 8 8 8 7 4 7 7 7 7 7 7 7 7 7 7 7
//      6 7 7 7 7 7 7 7 7 7 7 7 2 5 7 8 7 6 4 2 3 7 7 7 7 7 7 7 7 7 6 2
//      7 7 7 7 7 7 7 7 7 7 7 7 5 2 2 2 2 2 2 2 4 7 7 7 7 7 7 7 6 3 0 0
//      4 6 7 7 7 7 7 7 7 7 7 7 5 1 1 1 1 1 1 1 4 7 7 7 7 7 7 5 0 0 0 0
//      0 0 1 3 5 6 7 7 7 7 7 7 3 0 0 0 0 0 0 0 3 7 7 7 7 7 3 0 0 0 0 0
//      0 0 0 0 0 0 2 5 6 7 7 5 0 0 0 0 0 0 0 0 0 4 6 6 5 2 0 0 0 0 0 0
//
// Example of output:
//                   ..---.
//                  .-------.
//                 .---------
//                 ----------.
//                 -----------
//                 -oo--o#O---
//                 +##=-#O#o--.
//                 =*o*-O.**--.
//                 +OoO*O-Oo--.
//                 -OOOOOOO+--.
//                 oOOOOO**o--.
//                 =*****OOO---
//                 -OOOOO###+--.
//                .-########*---.
//               .-=#########----
//               --*#########=---.
//              .-+##########O----.
//             .--O###########++---
//             --o############*+o--.
//            .-=##############-oo--
//            .*o##############=-O--.
//           .-*O##############*-O--.
//           .-*###############*-O---
//           .-O###############OoO---
//           ==O###############O#*---
//          oOO=################*+--.
//         =OOOO=O############O*++=*O.
//      -=*OOOOO*-*###########OO+=OOO+
//      *OOOOOOOOo-o##########OOOOOOOo
//      *OOOOOOOOO+-*########OOOOOOOOO=
//      =OOOOOOOOO*+O########OOOOOOOOOO*
//      =OOOOOOOOOOo#######O=OOOOOOOOOOO
//      *OOOOOOOOOOO-oO#O*=-+OOOOOOOOO*-
//      OOOOOOOOOOOOo-------=OOOOOOO*+
//      =*OOOOOOOOOOo.......=OOOOOOo
//       .+o*OOOOOO+       +OOOOO+
//           -o*OOo         =**o-
//
#define STD_OUT false

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef linux
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif

#define P2_MARKER "P2"

#define ARG_COUNT 4
#define ARG_COMMAND 0
#define ARG_INPUT 1
#define ARG_OUTPUT 2
#define ARG_CHAR_SET 3

#define MAX_LEN_SIZE 70

#define SYMBOL_COMMENT "#"
#define SYMBOL_SPACE " "
#ifdef linux
#define SYMBOL_NEW_LINE "\n"
#define SYMBOL_NEW_LINE_LEN 1
#endif
#ifdef _WIN32
#define SYMBOL_NEW_LINE "\r\n"
#define SYMBOL_NEW_LINE_LEN 2
#endif

#define char_is_newline(c) ((c) == '\n' || (c) == '\r')
#define char_is_comment(c) ((c) == '#')
#define char_is_space(c) ((c) == ' ' || (c) == '\t' || (c) == '\v' || (c) == '\f')

#define is_marrker_correct(marker) (strcmp(marker, P2_MARKER) == 0)

#ifdef linux
#define error(message) \
    printf(message);   \
    exit(1);
#define errorf(message, ...)      \
    printf(message, __VA_ARGS__); \
    exit(1);
#endif
#ifdef _WIN32
#define error(message) \
    _tprintf(message); \
    exit(1);
#define errorf(message, ...)            \
    _tprintf(_T(message), __VA_ARGS__); \
    exit(1);
#endif

typedef unsigned short uint16_t;

typedef struct
{
    char *data;
    size_t size;
} file;

typedef struct
{
    char *marker;
    int x;
    uint16_t y;
    uint16_t n;
} pgm_header;

typedef struct
{
    pgm_header *header;
    file file;
} pgm;

// If usage of the program is wrong, print the correct usage and exit
void usage_is_wrong(char *program_name)
{
    errorf("Usage: %s [input file] [output file] [character set] \n", program_name);
}

// If file marker is wrong, print the correct marker and exit
void file_marker_is_wrong(char *marker)
{
    errorf("The marker of the input file is not correct. It should be %s \n", marker);
}

// If the number of gray levels is wrong, print the correct number and exit
void char_set_is_wrong(uint16_t n)
{
    errorf(
        "The length of char ser is not correct with input file. It should be %d \n",
        n);
}

// Access to charset is over scale
void over_scale_charset()
{
    error("Access to charset is over scale\n");
}

// Count char set of input
size_t count_char_set_input(char *char_set)
{
    return strlen(char_set);
}

bool is_charset_length_correct(uint16_t n, char *char_set)
{
    return n + 1 == count_char_set_input(char_set);
}

double length_of_number(uint16_t number)
{
    return floor(log10(number)) + 1;
}

file read_file(char *input_file)
{
    file file;

#ifdef linux
    // Open the file
    int fd = open(input_file, O_RDONLY);

    // Get the size of the file
    size_t size = lseek(fd, 0, SEEK_END);

    // Map file to memory
    char *buffer = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buffer == MAP_FAILED)
    {
        error("Error: Could not map file to memory \n");
    }

    // Copy the file to a char array
    char *data = malloc(size * sizeof(char));
    if (data == NULL)
    {
        error("Error: Could not allocate memory \n");
    }

    memcpy(data, buffer, size);

    // Close the file
    munmap(buffer, size);
    close(fd);
#endif
#ifdef _WIN32
    TCHAR *inputFile = _T(input_file);

    // Open the file for read
    HANDLE hFile = CreateFile(
        inputFile,
        GENERIC_READ,
        FILE_SHARE_READ,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        errorf("Unable to open: %s\n", inputFile);
    }

    // Get the file size
    DWORD dwFileSize = GetFileSize(hFile, NULL);

    // Create a file mapping object
    HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
    if (hFileMapping == NULL)
    {
        CloseHandle(hFile);
        errorf("Unable to create file mapping for file: %s\n", inputFile);
    }
    LPVOID lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, dwFileSize);
    if (lpFileBase == NULL)
    {
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        errorf("Unable to map view of file: %s\n", inputFile);
    }

    // Malloc buffer
    char *data = malloc(dwFileSize * sizeof(char));
    if (data == NULL)
    {
        UnmapViewOfFile(lpFileBase);
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        errorf("Unable to allocate memory for file: %s\n", inputFile);
    }
    int size = dwFileSize;

    // Copy file to a char array
    memcpy(data, lpFileBase, size);

    // Close file
    UnmapViewOfFile(lpFileBase);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);
#endif

    file.data = data;
    file.size = size;

    return file;
}

// Read the header of the pgm file
pgm_header *read_pgm_header(file input_file)
{
    pgm_header *header = (pgm_header *)malloc(sizeof(pgm_header));
    if (header == NULL)
    {
        error("Error: Could not allocate memory for header \n");
    }

    // read the marker
    char *marker = strtok(strdup(input_file.data), SYMBOL_NEW_LINE);

    // check if the marker is correct
    if (marker == NULL || !is_marrker_correct(marker))
        file_marker_is_wrong(P2_MARKER);
    else
        header->marker = marker;

    // read the x
    char *x = strtok(NULL, SYMBOL_SPACE);
    (*header).x = atoi(x);

    // read the y
    char *y = strtok(NULL, SYMBOL_NEW_LINE);
    (*header).y = atoi(y);

    // read the n
    char *n = strtok(NULL, SYMBOL_NEW_LINE);
    (*header).n = atoi(n);

    return header;
}

pgm read_pgm(char *input_file_path)
{
    file input_file = read_file(input_file_path);

#if STD_OUT
    // print loaded data to stdout
    printf("Loaded:\n%s\n", input_file.data);
#endif

    pgm_header *header = read_pgm_header(input_file);

    pgm pgm;
    pgm.header = header;
    pgm.file = input_file;

    return pgm;
}

void write_file(char *output_file, char *data, size_t size)
{
#ifdef linux
    // Create the file
    int fd = open(output_file, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);

    // Set the size of the file
    ftruncate(fd, size);

    // Map file to memory
    char *buffer = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED)
    {
        error("Error: Could not map file to memory \n");
    }

    // Write to the file
    strcpy(buffer, data);

    // Unmap the file
    munmap(buffer, size);
    close(fd);
#endif
#ifdef _WIN32
    // Create the file
    HANDLE hFile = CreateFile(
        output_file,
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ,
        NULL,
        CREATE_ALWAYS,
        0,
        NULL);
    if (hFile == INVALID_HANDLE_VALUE)
    {
        errorf("Unable to create file: %s\n", output_file);
    }

    // Create a file mapping object
    HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE, 0, size, NULL);
    if (hFileMapping == NULL)
    {
        CloseHandle(hFile);
        errorf("Unable to create file mapping for file: %s\n", output_file);
    }

    // Map the file
    LPVOID lpFileBase = MapViewOfFile(hFileMapping, FILE_MAP_WRITE, 0, 0, size);
    if (lpFileBase == NULL)
    {
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        errorf("Unable to map view of file: %s\n", output_file);
    }

    // Copy data to file
    memcpy(lpFileBase, data, size);

    // Close file
    UnmapViewOfFile(lpFileBase);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);
#endif
}

int calc_header_size(pgm_header *header)
{
    int x = length_of_number(header->x);
    int y = length_of_number(header->y);
    int n = length_of_number(header->n);

    return strlen(header->marker) +
           SYMBOL_NEW_LINE_LEN +
           x +
           SYMBOL_NEW_LINE_LEN +
           y +
           SYMBOL_NEW_LINE_LEN +
           n +
           SYMBOL_NEW_LINE_LEN;
}

void convert_pgm_to_ascii(pgm pgm, char *char_set, char *output)
{
    int header_size = calc_header_size(pgm.header);
    int char_set_length = count_char_set_input(char_set);

    int idx = 0;
    int idx_char_set = 0;
    bool is_in_comment = false;
    int idx_buffer = 0;
    char *buffer = malloc(MAX_LEN_SIZE * sizeof(char));
    if (buffer == NULL)
    {
        error("Error: Could not allocate memory for buffer \n");
    }

    for (int i = header_size; i < pgm.file.size; i++)
    {
        char c = pgm.file.data[i];
        bool is_newline = char_is_newline(c);
        bool is_comment = char_is_comment(c);
        bool is_space = char_is_space(c);

        if (is_newline)
        {
            is_in_comment = false;
            if (c == '\n')
            {
                output[idx] = '\n';
                idx++;
            }

            // reset buffer
            idx_buffer = 0;

            continue;
        }
        else if (is_in_comment)
        {
            continue;
        }

        if (is_comment)
        {
            is_in_comment = true;
            continue;
        }
        else if (is_space)
        {
            idx_char_set = 0;
            int err = sscanf(buffer, "%d", &idx_char_set);
            if (err == 0)
            {
                error("Error: Could not convert buffer to int \n");
            }

            if (idx_char_set >= char_set_length)
                over_scale_charset();

            char char_to_set = char_set[idx_char_set];
            err = sscanf(&char_to_set, "%c", &output[idx]);
            if (err == 0)
            {
                error("Error: Could not convert char to char \n");
            }

            idx++;
            idx_buffer = 0;
        }
        else
        {
            buffer[idx_buffer] = c;
            idx_buffer++;
        }
    }
}

int main(int argc, char *argv[])
{
#if STD_OUT
    char *arg_input_file_path = "input.pgm";
    char *arg_output_file_path = "output.txt";
    char *arg_char_set = " .-+=o*O#@";
#else
    // check if the number of arguments is correct
    if (argc != ARG_COUNT)
        usage_is_wrong(argv[ARG_COMMAND]);

    // args
    char *arg_input_file_path = argv[ARG_INPUT];
    char *arg_output_file_path = argv[ARG_OUTPUT];
    char *arg_char_set = argv[ARG_CHAR_SET];
#endif

    // read the pgm file
    pgm pgm = read_pgm(arg_input_file_path);

    // check char set length
    if (!is_charset_length_correct(pgm.header->n, arg_char_set))
    {
        char_set_is_wrong(pgm.header->n);
    }

    // prepare the output buffer
    size_t output_buffer_size = (pgm.header->x * pgm.header->y - 2) * sizeof(char);
    char *output_buffer = malloc(output_buffer_size);

#if STD_OUT
    printf("\n\nLength of output buffer: %d\n\n", pgm.header->x * pgm.header->y);
#endif

    convert_pgm_to_ascii(pgm, arg_char_set, output_buffer);

#if STD_OUT
    printf("ASCII output:\n%s\n\n", output_buffer);
#endif

    // write the output file
    write_file(arg_output_file_path, output_buffer, output_buffer_size);

    // free memory
    free(output_buffer);
    free(pgm.header);

    return 0;
}
