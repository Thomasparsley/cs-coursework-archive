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

#if defined(__linux__) || defined(__unix__) || defined(__APPLE__)
#define PGM_POSIX 1
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif
#endif

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#ifdef PGM_POSIX
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#endif

#ifdef _WIN32
#include <windows.h>
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
#ifdef PGM_POSIX
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

#ifdef PGM_POSIX
#define error(message) \
    printf(message);   \
    exit(1);
#define errorf(message, ...)      \
    printf(message, __VA_ARGS__); \
    exit(1);
#endif
#ifdef _WIN32
#define error(message) \
    printf(message);   \
    exit(1);
#define errorf(message, ...)      \
    printf(message, __VA_ARGS__); \
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
    size_t data_offset; // offset of the first byte after the header
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

file read_file(char *input_file)
{
    file file;

#ifdef PGM_POSIX
    // Open the file
    int fd = open(input_file, O_RDONLY);
    if (fd < 0)
    {
        perror("Error: Could not open input file");
        exit(1);
    }

    // Get the size of the file
    off_t end = lseek(fd, 0, SEEK_END);
    if (end < 0)
    {
        perror("Error: Could not get size of input file");
        exit(1);
    }
    size_t size = (size_t)end;
    if (size == 0)
    {
        error("Error: Input file is empty \n");
    }

    // Map file to memory
    char *buffer = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buffer == MAP_FAILED)
    {
        error("Error: Could not map file to memory \n");
    }

    // Copy the file to a char array
    char *data = malloc(size + 1);
    if (data == NULL)
    {
        error("Error: Could not allocate memory \n");
    }

    memcpy(data, buffer, size);
    data[size] = '\0';

    // Close the file
    munmap(buffer, size);
    close(fd);
#endif
#ifdef _WIN32
    char *inputFile = input_file;

    // Open the file for read
    HANDLE hFile = CreateFileA(
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
    char *data = malloc((size_t)dwFileSize + 1);
    if (data == NULL)
    {
        UnmapViewOfFile(lpFileBase);
        CloseHandle(hFileMapping);
        CloseHandle(hFile);
        errorf("Unable to allocate memory for file: %s\n", inputFile);
    }
    size_t size = dwFileSize;

    // Copy file to a char array
    memcpy(data, lpFileBase, size);
    data[size] = '\0';

    // Close file
    UnmapViewOfFile(lpFileBase);
    CloseHandle(hFileMapping);
    CloseHandle(hFile);
#endif

    file.data = data;
    file.size = size;

    return file;
}

// Read next header token. Skips whitespace and comments (# to end of line).
// Returns false when there is no further token.
static bool next_header_token(file input_file, size_t *pos, char *token, size_t token_size)
{
    size_t i = *pos;
    const char *d = input_file.data;

    for (;;)
    {
        while (i < input_file.size && (char_is_space(d[i]) || char_is_newline(d[i])))
            i++;
        if (i < input_file.size && char_is_comment(d[i]))
        {
            while (i < input_file.size && d[i] != '\n' && d[i] != '\r')
                i++;
            continue;
        }
        break;
    }

    size_t len = 0;
    while (i < input_file.size && !char_is_space(d[i]) && !char_is_newline(d[i]) && !char_is_comment(d[i]))
    {
        if (len + 1 >= token_size)
        {
            error("Error: Header token is too long \n");
        }
        token[len++] = d[i++];
    }
    token[len] = '\0';
    *pos = i;

    return len > 0;
}

// Parse a header number in range min..max
static long parse_header_number(const char *token, long min, long max)
{
    char *end = NULL;
    long value = strtol(token, &end, 10);
    if (end == token || *end != '\0' || value < min || value > max)
    {
        errorf("Error: Invalid value in the header: %s \n", token);
    }

    return value;
}

// Read the header of the pgm file
pgm_header *read_pgm_header(file input_file)
{
    pgm_header *header = (pgm_header *)malloc(sizeof(pgm_header));
    if (header == NULL)
    {
        error("Error: Could not allocate memory for header \n");
    }

    char token[MAX_LEN_SIZE];
    size_t pos = 0;

    // read the marker
    if (!next_header_token(input_file, &pos, token, sizeof(token)) || !is_marrker_correct(token))
        file_marker_is_wrong(P2_MARKER);
    header->marker = P2_MARKER;

    // read the x
    if (!next_header_token(input_file, &pos, token, sizeof(token)))
    {
        error("Error: Header is incomplete \n");
    }
    header->x = (int)parse_header_number(token, 1, 65535);

    // read the y
    if (!next_header_token(input_file, &pos, token, sizeof(token)))
    {
        error("Error: Header is incomplete \n");
    }
    header->y = (uint16_t)parse_header_number(token, 1, 65535);

    // read the n
    if (!next_header_token(input_file, &pos, token, sizeof(token)))
    {
        error("Error: Header is incomplete \n");
    }
    header->n = (uint16_t)parse_header_number(token, 0, 65535);

    header->data_offset = pos;

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
#ifdef PGM_POSIX
    // Create the file
    int fd = open(output_file, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        perror("Error: Could not open output file");
        exit(1);
    }

    // Set the size of the file
    if (ftruncate(fd, (off_t)size) != 0)
    {
        perror("Error: Could not set size of output file");
        exit(1);
    }

    // Map file to memory
    char *buffer = mmap(NULL, size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (buffer == MAP_FAILED)
    {
        error("Error: Could not map file to memory \n");
    }

    // Write to the file
    memcpy(buffer, data, size);

    // Unmap the file
    munmap(buffer, size);
    close(fd);
#endif
#ifdef _WIN32
    // Create the file
    HANDLE hFile = CreateFileA(
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

typedef struct
{
    char *output;
    size_t idx;
    size_t pixels;
    size_t total_pixels;
    int x;
    int column;
    int n;
    char *char_set;
    int char_set_length;
} converter;

// Append one pixel to the output; end the row after x pixels
static void emit_pixel(converter *c, long value)
{
    if (value < 0 || value > c->n || value >= c->char_set_length)
        over_scale_charset();

    if (c->pixels >= c->total_pixels)
    {
        error("Error: The file contains more pixels than the header says \n");
    }

    c->output[c->idx++] = c->char_set[value];
    c->pixels++;
    if (++c->column == c->x)
    {
        memcpy(c->output + c->idx, SYMBOL_NEW_LINE, SYMBOL_NEW_LINE_LEN);
        c->idx += SYMBOL_NEW_LINE_LEN;
        c->column = 0;
    }
}

// Convert pending token (if any) to a pixel
static void flush_token(converter *c, char *buffer, int *idx_buffer)
{
    if (*idx_buffer == 0)
        return;

    buffer[*idx_buffer] = '\0';
    *idx_buffer = 0;

    char *end = NULL;
    long value = strtol(buffer, &end, 10);
    if (end == buffer || *end != '\0')
    {
        error("Error: Could not convert buffer to int \n");
    }

    emit_pixel(c, value);
}

// Returns the number of bytes written to output
size_t convert_pgm_to_ascii(pgm pgm, char *char_set, char *output)
{
    converter c;
    c.output = output;
    c.idx = 0;
    c.pixels = 0;
    c.total_pixels = (size_t)pgm.header->x * pgm.header->y;
    c.x = pgm.header->x;
    c.column = 0;
    c.n = pgm.header->n;
    c.char_set = char_set;
    c.char_set_length = (int)count_char_set_input(char_set);

    // With at most 10 gray levels every digit is one pixel, so pixels do not
    // have to be separated by spaces.
    bool single_digit = pgm.header->n <= 9;

    bool is_in_comment = false;
    int idx_buffer = 0;
    char *buffer = malloc(MAX_LEN_SIZE * sizeof(char));
    if (buffer == NULL)
    {
        error("Error: Could not allocate memory for buffer \n");
    }

    for (size_t i = pgm.header->data_offset; i < pgm.file.size; i++)
    {
        char ch = pgm.file.data[i];

        if (char_is_newline(ch))
        {
            is_in_comment = false;
            flush_token(&c, buffer, &idx_buffer);
        }
        else if (is_in_comment)
        {
            continue;
        }
        else if (char_is_comment(ch))
        {
            is_in_comment = true;
            flush_token(&c, buffer, &idx_buffer);
        }
        else if (char_is_space(ch))
        {
            flush_token(&c, buffer, &idx_buffer);
        }
        else if (ch >= '0' && ch <= '9')
        {
            if (single_digit)
            {
                emit_pixel(&c, ch - '0');
            }
            else
            {
                // a pixel value has at most 5 digits
                if (idx_buffer >= 6)
                {
                    error("Error: Pixel value is too long \n");
                }
                buffer[idx_buffer++] = ch;
            }
        }
        else
        {
            error("Error: Invalid character in pixel data \n");
        }
    }
    flush_token(&c, buffer, &idx_buffer);

    free(buffer);

    if (c.pixels != c.total_pixels)
    {
        error("Error: The file contains fewer pixels than the header says \n");
    }

    return c.idx;
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
    size_t output_buffer_size = ((size_t)pgm.header->x + SYMBOL_NEW_LINE_LEN) * pgm.header->y;
    char *output_buffer = malloc(output_buffer_size + 1);
    if (output_buffer == NULL)
    {
        error("Error: Could not allocate memory for output \n");
    }

#if STD_OUT
    printf("\n\nLength of output buffer: %d\n\n", pgm.header->x * pgm.header->y);
#endif

    size_t output_size = convert_pgm_to_ascii(pgm, arg_char_set, output_buffer);
    output_buffer[output_size] = '\0';

#if STD_OUT
    printf("ASCII output:\n%s\n\n", output_buffer);
#endif

    // write the output file
    write_file(arg_output_file_path, output_buffer, output_size);

    // free memory
    free(output_buffer);
    free(pgm.header);
    free(pgm.file.data);

    return 0;
}
