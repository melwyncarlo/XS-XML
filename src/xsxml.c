/*
Copyright 2021 Melwyn Francis Carlo <carlo.melwyn@outlook.com>

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

   http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/


#include <math.h>
#include <time.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


#include "xsxml.h"


/*
Xsxml is a tiny, basic, single-file XML parser.
Xsxml stands for 'Extra small XML'.

It works fine with simple XML files written as per 
the standards of version 1.0 and encoding of UTF-8.

It does not cater to the following features:
    XML schemas and DTDs
    Name prefixes
    XML namespaces (the xmlns attribute) 

Though it can parse CDATA and character entity references.

Also note that all alphabetic input and data are case-sensitive.
*/


/* The below code is just a test code for debugging purposes. */
/* #define INDENTATION 2                                      */


#define MAX_INDENTATION                         10

#define MAX_VERTICAL_SPACING                    10

#define NODE_FILE_NAME_SIZE                     10

#define RESULT_MESSAGE_MAX_LENGTH               200

#define MAX_UTF_8_CHARACTER_VALUE               1114112

#define NUMBER_OF_ALPHABETIC_CHARACTERS         26

#define LOWER_CASE_LETTER_A_ASCII_VALUE         97

#define MAX_ATTRIBUTES_POSSIBLE_N               99999

#define MAX_ATTRIBUTES_POSSIBLE_N_DIGITS        5

#define CHARACTER_ENTRY_REFERENCE_MAX_LENGTH    10


typedef enum Xml_Data_Access_Mode
{
    XSXML_RAM_MODE, 
    XSXML_FILE_MODE

} Xml_Data_Access_Mode;


typedef enum Xsxml_Parse_Mode
{
    XSXML_TAG, 
    XSXML_ATTRIBUTE_NAME, 
    XSXML_ATTRIBUTE_VALUE, 
    XSXML_PCDATA_CONTENT

} Xsxml_Parse_Mode;


typedef struct Xsxml_Char_File_IO
{
    char *temporary_dir_path;
    char *temporary_file_name;
    unsigned int node_i_value;
    char *property_term;
    unsigned int node_j_value;
    char *data_str;
    unsigned int data_int;

} Xsxml_Char_File_IO;


typedef struct Xsxml_Private_Result
{
    Xsxml_Result result_code;
    char *result_message;

} Xsxml_Private_Result;


static const char *PROPERTY_NAMES_LIST[11] = 
{
    "nn", "l", "cN", "aN", "c", "an", "av", "a", "d", "ns", "ps"

};


static FILE* open_char_file( const char *dir_path, 
                             const char *file_name, 
                             const char *file_mode)
{
    /* The plus one (+1) is for the char array's null terminator.  */
    char *file_path = (char *) calloc( (strlen(dir_path) + strlen(file_name) + 1), 
                                        sizeof(char));

    memcpy(&file_path[0], &dir_path[0], strlen(dir_path));

    memcpy(&file_path[strlen(dir_path)], &file_name[0], strlen(file_name));

    return fopen(file_path, file_mode);
}


static int delete_char_file( const char *dir_path, 
                             const char *file_name)
{
    /* The plus one (+1) is for the char array's null terminator.  */
    char *file_path = (char *) calloc( (strlen(dir_path) + strlen(file_name) + 1), 
                                        sizeof(char));

    memcpy(&file_path[0], &dir_path[0], strlen(dir_path));

    memcpy(&file_path[strlen(dir_path)], &file_name[0], strlen(file_name));

    return remove(file_path);
}


static void append_character_to_word( char **word, 
                                      const char character, 
                                      int *word_len)
{
    /* The below code is just a test code for debugging purposes.     */
    /* printf("\n(%s), (%c), and %d\n", *word, character, *word_len); */

    *word = (char *) realloc(*word, ++(*word_len) + 1);

    if (isalpha(character))
    {
        (*word)[(*word_len) - 1] = character;
        return;
    }

    (*word)[(*word_len) - 1] = character;
}


static void reset_word(char **word, int *word_len)
{
    *word     = (char *) realloc(*word, 1);
   (*word)[0] = 0;

    *word_len = 0;
}

static int parse_cer(char **character_entry_reference)
{
    const char *BASE_ENTITY_REFERENCES[2][5] = 
    {
        {   "lt", 
            "gt", 
            "amp", 
            "apos", 
            "quot"
        }, 

        {   "<", 
            ">", 
            "&", 
            "\'", 
            "\""
        }
    };

    for (int i = 0; i < 5; i++)
    {
        if (strcmp( (const char*) (*character_entry_reference), 
                    (const char*)   BASE_ENTITY_REFERENCES[0][i] ) == 0)
        {
            (*character_entry_reference)[0] = BASE_ENTITY_REFERENCES[1][i][0];
              return 1;
        }
    }

    if ((*character_entry_reference)[0] != '#') return 0;

    int i = 1;

    int cer_is_hex = 0;

    if (((*character_entry_reference)[1] == 'x') 
    ||  ((*character_entry_reference)[1] == 'X'))
    {
        i = 2;
        cer_is_hex = 1;
    }

    while ((*character_entry_reference)[i] == '0') i++;

    unsigned int decimal_value = 0;

    /* Convert the hexa-decimal value to its decimal form. */
    if (cer_is_hex)
    {
        int power_count = 0;

        for (int j = strlen((*character_entry_reference)) - 1; j >= i ; j--)
        {
            int temp_val;

            if ((*character_entry_reference)[j] >= '0' && (*character_entry_reference)[j] <= '9')
            {
                temp_val = (*character_entry_reference)[j] - '0';
            }
            else
            {
                (*character_entry_reference)[j] = tolower((*character_entry_reference)[j]);

                if ((*character_entry_reference)[j] > 'f') return 0;

                temp_val = (*character_entry_reference)[j] - 'a' + 10;
            }

            decimal_value += temp_val * pow(16 , power_count++);
        }
    }
    else
    {
        memmove( &(*character_entry_reference)[0], 
                 &(*character_entry_reference)[1], 
                 strlen(*character_entry_reference) - 1);

        (*character_entry_reference) [strlen((*character_entry_reference)) - 1] = 0;

        decimal_value = atol(*character_entry_reference);
    }

    /* Converting decimal value to UTF-8. */
    if (decimal_value >= MAX_UTF_8_CHARACTER_VALUE) return 0;

    (*character_entry_reference)[0] = (char) decimal_value;

    return 1;
}


static void xsxml_unset_node(Xsxml_Nodes **xsxml_node)
{
    if ((*xsxml_node) != NULL)
    {
        if ((*xsxml_node)->node_name != NULL)
        {
            free((*xsxml_node)->node_name);

            (*xsxml_node)->node_name = NULL;
        }

        for (unsigned int j = 0; j < (*xsxml_node)->number_of_contents; j++)
        {
            if ((*xsxml_node)->content[j] != NULL)
            {
                free((*xsxml_node)->content[j]);

                (*xsxml_node)->content[j] = NULL;
            }
        }

        for (unsigned int j = 0; j < (*xsxml_node)->number_of_attributes; j++)
        {
            if ((*xsxml_node)->attribute_name[j] != NULL)
            {
                free((*xsxml_node)->attribute_name[j]);

                (*xsxml_node)->attribute_name[j] = NULL;
            }

            if ((*xsxml_node)->attribute_value[j] != NULL)
            {
                free((*xsxml_node)->attribute_value[j]);

                (*xsxml_node)->attribute_value[j] = NULL;
            }
        }

        (*xsxml_node)->ancestor         = NULL;
        (*xsxml_node)->descendant       = NULL;
        (*xsxml_node)->next_sibling     = NULL;
        (*xsxml_node)->previous_sibling = NULL;

        free(*xsxml_node);

        *xsxml_node = NULL;
    }
}


void xsxml_unset(Xsxml **xsxml_object)
{
    if (*xsxml_object != NULL)
    {
        if ((*xsxml_object)->result_message != NULL)
        {
            free((*xsxml_object)->result_message);

            (*xsxml_object)->result_message = NULL;
        }

        for (unsigned int i = 0; i < (*xsxml_object)->number_of_nodes; i++)
        {
            xsxml_unset_node(&(*xsxml_object)->node[i]);
        }

        free((*xsxml_object)->node);

        (*xsxml_object)->node = NULL;

        free(*xsxml_object);

        *xsxml_object = NULL;
    }
}


void xsxml_files_unset(Xsxml_Files **xsxml_files_object)
{
    if (*xsxml_files_object != NULL)
    {
        if ((*xsxml_files_object)->result_message != NULL)
        {
            free((*xsxml_files_object)->result_message);

            (*xsxml_files_object)->result_message = NULL;
        }

        if (((*xsxml_files_object)->node_file_name  != NULL) 
        &&  ((*xsxml_files_object)->number_of_nodes != 0))
        {
            const size_t MAX_I_STR_LEN = (int)log10((*xsxml_files_object)->number_of_nodes) 
                                       + 1;

            /* The plus one (+2) denotes the two (2) periods (.).             */
            const int FILE_NAME_PREFIX_LEN = NODE_FILE_NAME_SIZE + 2;

            /* The value four (4) values denotes the four periods (.) in      */
            /* a file name, whereas the value two (2) denotes the maximum     */
            /* length of alphabetical extension (.am, .av, .ps, and .ns).     */
            const int MAX_FILE_NAME_LEN = MAX_ATTRIBUTES_POSSIBLE_N_DIGITS 
                                        + FILE_NAME_PREFIX_LEN 
                                        + MAX_I_STR_LEN 
                                        + 4 
                                        + 2;

            char file_name_prefix [FILE_NAME_PREFIX_LEN];

            file_name_prefix [0] = '.';
            file_name_prefix [ FILE_NAME_PREFIX_LEN - 1 ] = '.';

            memcpy( &file_name_prefix[1], 
                    &(*xsxml_files_object)->node_file_name[0], 
                    NODE_FILE_NAME_SIZE);

            for (unsigned int i = 0; i < (*xsxml_files_object)->number_of_nodes; i++)
            {
                size_t I_STR_LEN;
                if (i == 0)
                    I_STR_LEN = 1;
                else
                    I_STR_LEN = (int)log10(i) + 1;

                /* The plus one (+1) is for the char array's null terminator. */
                char file_name [MAX_FILE_NAME_LEN + 1];

                file_name [MAX_FILE_NAME_LEN] = 0;

                memcpy( &file_name[0], 
                        &file_name_prefix[0], 
                        FILE_NAME_PREFIX_LEN);

                sprintf(&file_name[FILE_NAME_PREFIX_LEN], "%d", i);

                int dummy_val;

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN] = '.';

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 2] = 0;

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 1] = 'a';

                dummy_val = delete_char_file( (*xsxml_files_object)->node_directory_path, 
                                                file_name);

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 1] = 'd';

                dummy_val = delete_char_file( (*xsxml_files_object)->node_directory_path, 
                                                file_name);

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 1] = 'l';

                dummy_val = delete_char_file( (*xsxml_files_object)->node_directory_path, 
                                                file_name);

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 3] = 0;

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 1] = 'a';
                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 2] = 'N';

                dummy_val = delete_char_file( (*xsxml_files_object)->node_directory_path, 
                                                file_name);

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 1] = 'c';
                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 2] = 'N';

                dummy_val = delete_char_file( (*xsxml_files_object)->node_directory_path, 
                                                file_name);

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 1] = 'p';
                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 2] = 's';

                dummy_val = delete_char_file( (*xsxml_files_object)->node_directory_path, 
                                                file_name);

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 1] = 'n';
                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 2] = 's';

                dummy_val = delete_char_file( (*xsxml_files_object)->node_directory_path, 
                                                file_name);

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 1] = 'n';
                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 2] = 'n';

                dummy_val = delete_char_file( (*xsxml_files_object)->node_directory_path, 
                                                file_name);

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 3] = '.';

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 1] = 'a';
                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 2] = 'n';

                for (int j = 0; j < MAX_ATTRIBUTES_POSSIBLE_N; j++)
                {
                    sprintf(&file_name[FILE_NAME_PREFIX_LEN + I_STR_LEN + 4], "%d", j);

                    dummy_val = delete_char_file( (*xsxml_files_object)->node_directory_path, 
                                                    file_name);

                    if (dummy_val == -1) break;
                }

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 1] = 'a';
                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 2] = 'v';

                for (int j = 0; j < MAX_ATTRIBUTES_POSSIBLE_N; j++)
                {
                    sprintf(&file_name[FILE_NAME_PREFIX_LEN + I_STR_LEN + 4], "%d", j);

                    dummy_val = delete_char_file( (*xsxml_files_object)->node_directory_path, 
                                                    file_name);

                    if (dummy_val == -1) break;
                }

                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 1] = 'c';
                file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN + 2] = '.';

                for (int j = 0; j < MAX_ATTRIBUTES_POSSIBLE_N; j++)
                {
                    sprintf(&file_name[FILE_NAME_PREFIX_LEN + I_STR_LEN + 3], "%d", j);

                    dummy_val = delete_char_file( (*xsxml_files_object)->node_directory_path, 
                                                    file_name);

                    if (dummy_val == -1) break;
                }
            }

            free((*xsxml_files_object)->node_file_name);

            (*xsxml_files_object)->node_file_name = NULL;
        }

        if ((*xsxml_files_object)->node_directory_path != NULL)
        {
            free((*xsxml_files_object)->node_directory_path);

            (*xsxml_files_object)->node_directory_path = NULL;
        }

        free(*xsxml_files_object);

        *xsxml_files_object = NULL;
    }
}


static int parse_sub_operation_ram_mode( Xsxml **xsxml_object, 
                                         Xsxml_Parse_Mode parse_mode, 
                                         unsigned int node_level, 
                                         char *data)
{
    const size_t DATA_LEN = strlen(data);

    const size_t n = (*xsxml_object)->number_of_nodes;

    if (parse_mode == XSXML_TAG)
    {
        if (node_level == 0)
        {
            if ((*xsxml_object)->number_of_nodes > 0) return 0;
        }

        (*xsxml_object)->number_of_nodes++;

        (*xsxml_object)->node = 
        (Xsxml_Nodes **) realloc( (*xsxml_object)->node, 
                                  (n + 1) * sizeof(Xsxml_Nodes *));

        (*xsxml_object)->node[n] = (Xsxml_Nodes *) malloc(sizeof(Xsxml_Nodes));

        (*xsxml_object)->node[n]->node_name = (char *) malloc (DATA_LEN + 1);

        (*xsxml_object)->node[n]->node_name[DATA_LEN] = 0;

        memcpy( &(*xsxml_object)->node[n]->node_name[0], 
                &data[0], 
                DATA_LEN);

        (*xsxml_object)->node[n]->content = (char **) malloc(sizeof(char *));

        (*xsxml_object)->node[n]->depth = node_level;
        (*xsxml_object)->node[n]->number_of_contents   = 0;
        (*xsxml_object)->node[n]->number_of_attributes = 0;

        (*xsxml_object)->node[n]->descendant   = NULL;
        (*xsxml_object)->node[n]->next_sibling = NULL;

        if (node_level != 0)
        {
            if ((*xsxml_object)->node[n-1]->depth == node_level)
            {
                (*xsxml_object)->node[n]->previous_sibling = (*xsxml_object)->node[n-1];
                (*xsxml_object)->node[n-1]->next_sibling   = (*xsxml_object)->node[n];

                (*xsxml_object)->node[n]->ancestor = (*xsxml_object)->node[n-1]->ancestor;
            }
            else
            {
                (*xsxml_object)->node[n]->ancestor     = (*xsxml_object)->node[n-1];
                (*xsxml_object)->node[n-1]->descendant = (*xsxml_object)->node[n];

                for (int i = n - 1; i >= 0; i--)
                {
                    if ((*xsxml_object)->node[i]->depth == node_level)
                    {
                        (*xsxml_object)->node[n]->previous_sibling = (*xsxml_object)->node[i];
                        (*xsxml_object)->node[i]->next_sibling     = (*xsxml_object)->node[n];
                        break;
                    }
                }
            }
        }
        else /* if (node_level == 0) */
        {
            (*xsxml_object)->node[n]->ancestor         = NULL;
            (*xsxml_object)->node[n]->previous_sibling = NULL;
        }
    }
    else if (parse_mode == XSXML_ATTRIBUTE_NAME)
    {
        const size_t n_a = (*xsxml_object)->node[n-1]->number_of_attributes;

        for (unsigned int i = 0; i < n_a; i++)
        {
            if (strcmp( (const char*)  data, 
                        (const char*) ((*xsxml_object)->node[n-1]->attribute_name[i]) ) == 0)
            {
                return 0;
            }
        }

        (*xsxml_object)->node[n-1]->number_of_attributes++;

        (*xsxml_object)->node[n-1]->attribute_name = 
        (char **) realloc( (*xsxml_object)->node[n-1]->attribute_name, 
                           (n_a + 1) * sizeof(char *));

        (*xsxml_object)->node[n-1]->attribute_name[n_a] = (char *) malloc(DATA_LEN + 1);

        (*xsxml_object)->node[n-1]->attribute_name[n_a][DATA_LEN] = 0;

        memcpy( &(*xsxml_object)->node[n-1]->attribute_name[n_a][0], 
                &data[0], 
                DATA_LEN);
    }
    else if (parse_mode == XSXML_ATTRIBUTE_VALUE)
    {
        const size_t n_a = (*xsxml_object)->node[n-1]->number_of_attributes;

        (*xsxml_object)->node[n-1]->attribute_value = 
        (char **) realloc( (*xsxml_object)->node[n-1]->attribute_value, 
                             n_a * sizeof(char *));

        (*xsxml_object)->node[n-1]->attribute_value[n_a-1] = (char *) malloc(DATA_LEN + 1);

        (*xsxml_object)->node[n-1]->attribute_value[n_a-1][DATA_LEN] = 0;

        memcpy( &(*xsxml_object)->node[n-1]->attribute_value[n_a-1][0], 
                &data[0], 
                DATA_LEN);
    }
    else if (parse_mode == XSXML_PCDATA_CONTENT)
    {
        const size_t n_c = (*xsxml_object)->node[n-1]->number_of_contents;

        (*xsxml_object)->node[n-1]->number_of_contents++;

        (*xsxml_object)->node[n-1]->content = 
        (char **) realloc( (*xsxml_object)->node[n-1]->content, 
                           (n_c + 1) * sizeof(char *));

        (*xsxml_object)->node[n-1]->content[n_c] = (char *) malloc(DATA_LEN + 1);

        (*xsxml_object)->node[n-1]->content[n_c][DATA_LEN] = 0;

        memcpy( &(*xsxml_object)->node[n-1]->content[n_c][0], 
                &data[0], 
                DATA_LEN);
    }

    return 1;
}


static char *get_io_file_name(Xsxml_Char_File_IO *io_obj)
{
    size_t PROPERTY_TERM_LEN = strlen(io_obj->property_term);

    size_t I_STR_LEN;
    if (io_obj->node_i_value == 0)
        I_STR_LEN = 1;
    else
        I_STR_LEN = (int)log10(io_obj->node_i_value) + 1;

    /* The plus one (+2) denotes the two (2) periods (.).             */
    const int FILE_NAME_PREFIX_LEN = NODE_FILE_NAME_SIZE + 2;

    /* The value four (4) values denotes the four periods (.) in      */
    /* a file name, whereas the value two (2) denotes the maximum     */
    /* length of alphabetical extension (.am, .av, .ps, and .ns).     */
    const int MAX_FILE_NAME_LEN = MAX_ATTRIBUTES_POSSIBLE_N_DIGITS 
                                + FILE_NAME_PREFIX_LEN 
                                + I_STR_LEN 
                                + 4 
                                + 2;

    char file_name_prefix [FILE_NAME_PREFIX_LEN];

    file_name_prefix [0] = '.';
    file_name_prefix [ FILE_NAME_PREFIX_LEN - 1 ] = '.';

    memcpy( &file_name_prefix[1], 
            &io_obj->temporary_file_name[0], 
            NODE_FILE_NAME_SIZE);

    /* The plus one (+1) is for the char array's null terminator. */
    char *file_name = (char *) malloc(MAX_FILE_NAME_LEN + 1);

    memcpy( &file_name[0], 
            &file_name_prefix[0], 
            FILE_NAME_PREFIX_LEN);

    sprintf(&file_name[FILE_NAME_PREFIX_LEN], "%d", io_obj->node_i_value);

    file_name [FILE_NAME_PREFIX_LEN + I_STR_LEN] = '.';

    memcpy( &file_name[FILE_NAME_PREFIX_LEN + I_STR_LEN + 1], 
            &io_obj->property_term[0], 
            PROPERTY_TERM_LEN);

    const size_t PART_FILE_NAME_LEN = FILE_NAME_PREFIX_LEN 
                                    + I_STR_LEN 
                                    + PROPERTY_TERM_LEN 
                                    + 1;

    file_name [PART_FILE_NAME_LEN] = 0;

    if (io_obj->node_j_value != (unsigned int)-1)
    {
        file_name [PART_FILE_NAME_LEN] = '.';

        sprintf(&file_name[PART_FILE_NAME_LEN + 1], "%u", io_obj->node_j_value);
    }

    return file_name;
}


static void read_from_char_file(Xsxml_Char_File_IO *io_obj)
{
    char *file_name = get_io_file_name(io_obj);

    FILE *file_pointer_read = open_char_file(io_obj->temporary_dir_path, file_name, "r");

    if (io_obj->data_str == NULL)
    {
        fseek(file_pointer_read, 0L, SEEK_END);

        size_t FILE_SIZE = ftell(file_pointer_read);

        io_obj->data_str = (char *) calloc(FILE_SIZE, sizeof(char));

        fseek(file_pointer_read, 0L, SEEK_SET);

        if (fread(io_obj->data_str, sizeof(char), FILE_SIZE - 1, file_pointer_read)) {}
    }
    else
    {
        if (fscanf(file_pointer_read, "%u", &io_obj->data_int)) {}
    }

    fclose(file_pointer_read);

    free(file_name);
}


static void write_to_char_file(Xsxml_Char_File_IO *io_obj)
{
    char *file_name = get_io_file_name(io_obj);

    FILE *file_pointer_write = open_char_file(io_obj->temporary_dir_path, file_name, "w");

    if (io_obj->data_str != NULL)
        fprintf(file_pointer_write, "%s\n", io_obj->data_str);
    else
        fprintf(file_pointer_write, "%u\n", io_obj->data_int);

    fclose(file_pointer_write);

    free(file_name);
}


static int parse_sub_operation_file_mode( Xsxml_Files **xsxml_files_object, 
                                          Xsxml_Parse_Mode parse_mode, 
                                          unsigned int node_level, 
                                          char *data)
{
    Xsxml_Char_File_IO io_obj;
    io_obj.temporary_dir_path  = (*xsxml_files_object)->node_directory_path;
    io_obj.temporary_file_name = (*xsxml_files_object)->node_file_name;

    const size_t n = (*xsxml_files_object)->number_of_nodes;

    if (parse_mode == XSXML_TAG)
    {
        if (node_level == 0)
        {
            if ((*xsxml_files_object)->number_of_nodes > 0) return 0;
        }

        (*xsxml_files_object)->number_of_nodes++;

        const size_t NODE_FILE_NAME_LEN = strlen((*xsxml_files_object)->node_file_name);

        char *temp_file_name = (char *) malloc(NODE_FILE_NAME_LEN + 5);

        io_obj.property_term = "l";
        io_obj.node_i_value  =  n;
        io_obj.node_j_value  = -1;
        io_obj.data_int      = node_level;
        io_obj.data_str      = NULL;
        write_to_char_file(&io_obj);

        io_obj.property_term = "cN";
        io_obj.node_i_value  =  n;
        io_obj.data_int      =  0;
        write_to_char_file(&io_obj);

        io_obj.property_term = "aN";
        write_to_char_file(&io_obj);

        io_obj.property_term = "nn";
        io_obj.data_str      = data;
        write_to_char_file(&io_obj);

        io_obj.property_term = "d";
        temp_file_name = get_io_file_name(&io_obj);
        delete_char_file((*xsxml_files_object)->node_directory_path, temp_file_name);

        io_obj.property_term = "ns";
        temp_file_name = get_io_file_name(&io_obj);
        delete_char_file((*xsxml_files_object)->node_directory_path, temp_file_name);

        if (node_level != 0)
        {
            io_obj.property_term = "l";
            io_obj.node_i_value  =  n - 1;
            read_from_char_file(&io_obj);

            /* The variable io_obj.data_int plays the role of   */
            /* (*xsxml_object)->node[n-1]->depth                */
            if (io_obj.data_int == node_level)
            {
                io_obj.property_term = "a";
                read_from_char_file(&io_obj);

                io_obj.node_i_value  = n;
                io_obj.data_str      = NULL;
                write_to_char_file(&io_obj);

                io_obj.property_term = "ns";
                io_obj.node_i_value  = n - 1;
                io_obj.data_str      = "dummy";
                io_obj.data_int      = n;
                write_to_char_file(&io_obj);

                io_obj.property_term = "ps";
                io_obj.node_i_value  = n;
                io_obj.data_int      = n - 1;
                write_to_char_file(&io_obj);
            }
            else
            {
                io_obj.property_term = "a";
                io_obj.node_i_value  = n;
                io_obj.data_int      = n - 1;
                write_to_char_file(&io_obj);

                io_obj.property_term = "d";
                io_obj.node_i_value  = n - 1;
                io_obj.data_int      = n;
                write_to_char_file(&io_obj);

                for (int i = n - 1; i >= 0; i--)
                {
                    io_obj.node_i_value  =  i;
                    io_obj.property_term = "l";
                    io_obj.data_str      = "dummy";
                    read_from_char_file(&io_obj);

                    if (io_obj.data_int == node_level)
                    {
                        io_obj.property_term = "ps";
                        io_obj.node_i_value  = n;
                        io_obj.data_int      = i;
                        io_obj.data_str      = NULL;
                        write_to_char_file(&io_obj);

                        io_obj.property_term = "ns";
                        io_obj.node_i_value  = i;
                        io_obj.data_int      = n;
                        write_to_char_file(&io_obj);

                        break;
                    }
                }
            }
        }
        else /* if (node_level == 0) */
        {
            io_obj.property_term = "a";
            temp_file_name = get_io_file_name(&io_obj);
            delete_char_file((*xsxml_files_object)->node_directory_path, temp_file_name);

            io_obj.property_term = "ps";
            temp_file_name = get_io_file_name(&io_obj);
            delete_char_file((*xsxml_files_object)->node_directory_path, temp_file_name);
        }

        free(temp_file_name);
    }
    else if (parse_mode == XSXML_ATTRIBUTE_NAME)
    {
        io_obj.node_j_value  = -1;

        io_obj.property_term = "aN";
        io_obj.node_i_value  =  n - 1;
        io_obj.data_str      = "dummy";
        read_from_char_file(&io_obj);

        const size_t n_a = io_obj.data_int;

        for (unsigned int i = 0; i < n_a; i++)
        {
            io_obj.property_term = "an";
            io_obj.node_j_value  = i;
            io_obj.data_str      = NULL;
            read_from_char_file(&io_obj);

            if (strcmp( (const char*)  data, 
                        (const char*) io_obj.data_str) == 0)
            {
                return 0;
            }
        }

        io_obj.data_str = NULL;
        io_obj.node_j_value  = -1;
        io_obj.data_int = n_a + 1;
        write_to_char_file(&io_obj);

        io_obj.property_term = "an";
        io_obj.node_j_value  = n_a;
        io_obj.data_str      = data;
        write_to_char_file(&io_obj);
    }
    else if (parse_mode == XSXML_ATTRIBUTE_VALUE)
    {
        io_obj.node_j_value  = -1;

        io_obj.property_term = "aN";
        io_obj.node_i_value  =  n - 1;
        io_obj.data_str      = "dummy";
        read_from_char_file(&io_obj);

        const size_t n_a = io_obj.data_int;

        io_obj.property_term = "av";
        io_obj.node_j_value  = n_a - 1;
        io_obj.data_str      = data;
        write_to_char_file(&io_obj);
    }
    else if (parse_mode == XSXML_PCDATA_CONTENT)
    {
        io_obj.node_j_value  = -1;

        io_obj.property_term = "cN";
        io_obj.node_i_value  =  n - 1;
        io_obj.data_str      = "dummy";
        read_from_char_file(&io_obj);

        const size_t n_c = io_obj.data_int;

        io_obj.data_str = NULL;
        io_obj.data_int = n_c + 1;
        write_to_char_file(&io_obj);

        io_obj.property_term = "c";
        io_obj.node_j_value  = n_c;
        io_obj.data_str      = data;
        write_to_char_file(&io_obj);
    }

    return 1;
}


static Xsxml_Private_Result *parse_operation( void **object, 
                                              const Xml_Data_Access_Mode access_mode, 
                                              FILE *file_pointer)
{
    Xsxml_Private_Result *result_obj = 
    (Xsxml_Private_Result *) malloc(sizeof(Xsxml_Private_Result));

    result_obj->result_message = (char *) malloc(RESULT_MESSAGE_MAX_LENGTH);

    int XML_X                   = 0;    /* X */
    int XML_M                   = 0;    /* M */
    int XML_EQUAL               = 0;    /* = */
    int XML_HYPHEN              = 0;    /* - */
    int XML_QUESTION            = 0;    /* ? */
    int XML_AMPERSAND           = 0;    /* & */
    int XML_WHITESPACE          = 0;    /*   */
    int XML_SINGLE_QUOTE        = 0;    /* - */
    int XML_DOUBLE_QUOTE        = 0;    /* - */
    int XML_FORWARD_SLASH_END   = 0;    /* / */
    int XML_FORWARD_SLASH_START = 0;    /* / */
    int XML_END_SQUARE_BRACKET  = 0;    /* ] */

    int XML_HEADING             = 0;
    int XML_COMMENT_CONTENT     = 0;
    int XML_COMMENT_START_TAG   = 0;
    int XML_TAG                 = 0;
    int XML_ATTRIBUTE           = 0;
    int XML_ATTRIBUTE_NAME      = 0;
    int XML_ATTRIBUTE_VALUE     = 0;
    int XML_CDATA_CONTENT       = 0;
    int XML_CDATA_START_TAG     = 0;
    int XML_PCDATA_CONTENT      = 0;


    int cer_i      = 0;                 /* Character entry reference count */
    int word_len   = 0;                 /* Word characters count           */
    int node_level = 0;                 /* Current hierarchical depth      */


    char *word = (char *) calloc(1, sizeof(char));

    char *character_entry_reference = 
    (char *) calloc( CHARACTER_ENTRY_REFERENCE_MAX_LENGTH + 1, sizeof(char));

    character_entry_reference [CHARACTER_ENTRY_REFERENCE_MAX_LENGTH] = 0;


    while (1)
    {
        char file_data_character = fgetc(file_pointer);

        if (feof(file_pointer)) break;

        /* The below code is just a test code for debugging purposes. */
        /* printf("%c", file_data_character);                         */


/* ########################################################################## */
/*                                  XML HEADING                               */
/* ########################################################################## */
        if (XML_HEADING)
        {
            if (file_data_character == '?')
            {
                XML_QUESTION = 1;
                continue;
            }

            if (XML_QUESTION)
            {
                if (file_data_character == '>')
                {
                    XML_QUESTION = 0;
                    XML_HEADING  = 0;
                    continue;
                }
            }

            XML_QUESTION = 0;
            continue;
        }


/* ########################################################################## */
/*                            XML COMMENT CONTENT                             */
/* ########################################################################## */
        if (XML_COMMENT_CONTENT)
        {
            if (file_data_character == '-')
            {
                if (XML_HYPHEN == 1)
                {
                    XML_HYPHEN = 2;
                    continue;
                }
                else if (XML_HYPHEN == 2)
                {
                    result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                    sprintf( &result_obj->result_message[0], 
                             "A double hyphen (--) within comments "
                             "is not allowed.");

                    return result_obj;
                }

                XML_HYPHEN = 1;
                continue;
            }

            if (XML_HYPHEN)
            {
                if (file_data_character == '>')
                {
                    XML_HYPHEN  = 0;
                    XML_COMMENT_CONTENT = 0;
                    continue;
                }
            }

            XML_HYPHEN = 0;
            continue;
        }


/* ########################################################################## */
/*                            XML COMMENT START TAG                           */
/* ########################################################################## */
        if (XML_COMMENT_START_TAG)
        {
            if (file_data_character == '-')
            {
                XML_COMMENT_CONTENT   = 1;
                XML_COMMENT_START_TAG = 0;
                continue;
            }

            result_obj->result_code = XSXML_RESULT_XML_FAILURE;

            sprintf( &result_obj->result_message[0], 
                     "Tag names must start with an alphabetical character "
                     "[a-zA-Z] or underscore (_)");

            return result_obj;
        }


/* ########################################################################## */
/*                             XML CDATA CONTENT                              */
/* ########################################################################## */
        if (XML_CDATA_CONTENT)
        {
            if (file_data_character == ']')
            {
                XML_END_SQUARE_BRACKET++;
                continue;
            }

            if ((XML_END_SQUARE_BRACKET == 2) && (file_data_character == '>'))
            {
                /* Working on CDATA. */

                /* The below code is just a test code for debugging purposes.       */
                /* printf("\n\n%*sCDATA = %s", node_level * INDENTATION, "", word); */

                XML_CDATA_CONTENT      = 0;
                XML_END_SQUARE_BRACKET = 0;
                continue;
            }

            while (XML_END_SQUARE_BRACKET > 0)
            {
                append_character_to_word(&word, ']', &word_len);

                XML_END_SQUARE_BRACKET--;
            }

            append_character_to_word(&word, file_data_character, &word_len);

            continue;
        }


/* ########################################################################## */
/*                              XML CDATA START TAG                           */
/* ########################################################################## */
        if (XML_CDATA_START_TAG)
        {
            if (file_data_character == '-')
            {
                XML_COMMENT_START_TAG = 1;
                XML_CDATA_START_TAG   = 0;
                continue;
            }

            if (((XML_CDATA_START_TAG == 1) && (file_data_character == '[')) 
            ||  ((XML_CDATA_START_TAG == 2) && (file_data_character == 'C')) 
            ||  ((XML_CDATA_START_TAG == 3) && (file_data_character == 'D')) 
            ||  ((XML_CDATA_START_TAG == 4) && (file_data_character == 'A')) 
            ||  ((XML_CDATA_START_TAG == 5) && (file_data_character == 'T')) 
            ||  ((XML_CDATA_START_TAG == 6) && (file_data_character == 'A')))
            {
                XML_CDATA_START_TAG++;
                continue;
            }

            if ((XML_CDATA_START_TAG == 7) && (file_data_character == '['))
            {
                XML_CDATA_CONTENT      = 1;
                XML_PCDATA_CONTENT     = 1;
                XML_CDATA_START_TAG    = 0;
                XML_END_SQUARE_BRACKET = 0;
                continue;
            }

            result_obj->result_code = XSXML_RESULT_XML_FAILURE;

            sprintf( &result_obj->result_message[0], 
                     "Tag names must start with an alphabetical character "
                     "[a-zA-Z] or underscore (_)");

            return result_obj;
        }


/* ########################################################################## */
/*                                  XML TAG                                   */
/* ########################################################################## */
        if (XML_TAG)
        {
            if (XML_PCDATA_CONTENT)
            {
                if (file_data_character == '!')
                {
                    XML_TAG = 0;
                    XML_PCDATA_CONTENT  = 0;
                    XML_CDATA_START_TAG = 1;
                    continue;
                }

                /* Working on PCDATA. */

                if (word[strlen(word) - 1] == ' ')
                    word[strlen(word) - 1] = '\0';

                word [word_len] = 0;

                if (access_mode == XSXML_RAM_MODE)
                {
                    parse_sub_operation_ram_mode( (Xsxml **) object, 
                                                   XSXML_PCDATA_CONTENT, 
                                                   node_level, 
                                                   word);
                }
                else /* if (access_mode == XSXML_FILE_MODE) */
                {
                    parse_sub_operation_file_mode( (Xsxml_Files **) object, 
                                                    XSXML_PCDATA_CONTENT, 
                                                    node_level, 
                                                    word);
                }

                /* The below code is just a test code for debugging purposes.        */
                /* printf("\n\n%*sPCDATA = %s", node_level * INDENTATION, "", word); */

                reset_word(&word, &word_len);

                XML_PCDATA_CONTENT = 0;
            }

            if (!XML_ATTRIBUTE && word_len == 0)
            {
                if (file_data_character == '!')
                {
                    XML_TAG = 0;
                    XML_CDATA_START_TAG = 1;
                    continue;
                }

                if (file_data_character == '/')
                {
                    XML_FORWARD_SLASH_START = 1;
                    continue;
                }

                if (file_data_character == '?')
                {
                    XML_TAG      = 0;
                    XML_QUESTION = 0;
                    XML_HEADING  = 1;
                    continue;
                }

                if (!isalpha(file_data_character) && file_data_character != '_')
                {
                    result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                    sprintf( &result_obj->result_message[0], 
                             "Tag names must start with an alphabetical character "
                             "[a-zA-Z] or underscore (_)");

                    return result_obj;
                }

                if ((file_data_character == 'x') || (file_data_character == 'X'))
                {
                    XML_X = 1;
                    append_character_to_word(&word, file_data_character, &word_len);

                    continue;
                }
            }

            if (XML_X)
            {
                if ((file_data_character == 'm') || (file_data_character == 'M'))
                {
                    XML_M = 1;
                    append_character_to_word(&word, file_data_character, &word_len);
                    continue;
                }
                XML_X = 0;
            }

            if (XML_M)
            {
                if ((file_data_character == 'l') || (file_data_character == 'L'))
                {
                    result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                    sprintf( &result_obj->result_message[0], 
                             "Tag names cannot start with 'xml' or "
                             "any of its variants like XML, Xml, etc.");

                    return result_obj;
                }
                XML_X = 0;
                XML_M = 0;
            }


/* ########################################################################## */
/*                              XML ATTRIBUTE VALUE                           */
/* ########################################################################## */
            if (XML_ATTRIBUTE_VALUE)
            {
                if (file_data_character == '<')
                {
                    result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                    sprintf( &result_obj->result_message[0], 
                             "Attribute values may not contain the "
                             "less-than (<) characters.");

                    return result_obj;
                }

                if (file_data_character == '&')
                {
                    if (!XML_AMPERSAND)
                    {
                        cer_i = 0;

                        memset( &character_entry_reference[0], 
                                0, 
                                CHARACTER_ENTRY_REFERENCE_MAX_LENGTH);

                        character_entry_reference[0] = '&';

                        XML_AMPERSAND = 1;

                        continue;
                    }
                }

                if (file_data_character == ';')
                {
                    if (parse_cer(&character_entry_reference))
                    {
                        append_character_to_word( &word, 
                                                  character_entry_reference[0], 
                                                  &word_len);
                    }
                    else
                    {
                        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                        sprintf( &result_obj->result_message[0], 
                                 "Attribute values may contain the ampersand (&) "
                                 "characters only as character entity references.");

                        return result_obj;
                    }

                    XML_AMPERSAND = 0;
                    continue;
                }

                if (XML_AMPERSAND)
                {
                    if (cer_i == (CHARACTER_ENTRY_REFERENCE_MAX_LENGTH - 1))
                    {
                        const char *MESSAGE_PART_1 = "Attribute values may contain "
                                                     "the ampersand (&) characters "
                                                     "only as character entity "
                                                     "references. ";

                        const char *MESSAGE_PART_2 = "Character entry references "
                                                     "may contain atmost ";

                        const char *MESSAGE_PART_3 = " number of characters.";

                        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                        sprintf( &result_obj->result_message[0], 
                                 "%s%s%d%s", 
                                 MESSAGE_PART_1, 
                                 MESSAGE_PART_2, 
                                 CHARACTER_ENTRY_REFERENCE_MAX_LENGTH, 
                                 MESSAGE_PART_3);

                        return result_obj;
                    }

                    character_entry_reference [cer_i++] = file_data_character;
                    continue;
                }

                if (((file_data_character == '\'') && XML_SINGLE_QUOTE) 
                ||  ((file_data_character == '\"') && XML_DOUBLE_QUOTE))
                {
                    if (XML_AMPERSAND)
                    {
                        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                        sprintf( &result_obj->result_message[0], 
                                 "Attribute values may contain the ampersand (&) "
                                 "characters only as character entity references.");

                        return result_obj;
                    }

                    /* Working on attribute value. */

                    word [word_len] = 0;

                    if (access_mode == XSXML_RAM_MODE)
                    {
                        parse_sub_operation_ram_mode( (Xsxml **) object, 
                                                       XSXML_ATTRIBUTE_VALUE, 
                                                       node_level, 
                                                       word);
                    }
                    else /* if (access_mode == XSXML_FILE_MODE) */
                    {
                        parse_sub_operation_file_mode( (Xsxml_Files **) object, 
                                                        XSXML_ATTRIBUTE_VALUE, 
                                                        node_level, 
                                                        word);
                    }

                    /* The below code is just a test code for debugging purposes. */
                    /* printf("  &  ATTRIBUTE VALUE = %s", word);                 */

                    reset_word(&word, &word_len);

                    XML_ATTRIBUTE       = 1;
                    XML_ATTRIBUTE_NAME  = 0;
                    XML_ATTRIBUTE_VALUE = 0;
                    XML_SINGLE_QUOTE    = 0;
                    XML_DOUBLE_QUOTE    = 0;
                    continue;
                }

                append_character_to_word(&word, file_data_character, &word_len);
                continue;
            }


/* ########################################################################## */
/*                                 XML END TAG                                */
/* ########################################################################## */
            if (file_data_character == '>')
            {
                if (XML_ATTRIBUTE_NAME)
                {
                    result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                    sprintf( &result_obj->result_message[0], 
                             "If empty, then attribute values must atleast "
                             "contain the double quotes ("").");

                    return result_obj;
                }

                if (XML_FORWARD_SLASH_END)
                {
                    node_level--;
                    XML_FORWARD_SLASH_END = 0;
                }

                if (XML_ATTRIBUTE)
                {
                    node_level++;
                    XML_TAG       = 0;
                    XML_ATTRIBUTE = 0;
                    continue;
                }

                XML_TAG       = 0;
                XML_ATTRIBUTE = 0;

                if (XML_FORWARD_SLASH_START)
                {
                    node_level--;
                    XML_FORWARD_SLASH_START = 0;
                    reset_word(&word, &word_len);
                    continue;
                }

                /* Working on tag. */

                word [word_len] = 0;

                if (access_mode == XSXML_RAM_MODE)
                {
                    if (!parse_sub_operation_ram_mode( (Xsxml **) object, 
                                                        XSXML_TAG, 
                                                        node_level, 
                                                        word))
                    {
                        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                        sprintf( &result_obj->result_message[0], 
                                 "There cannot be more than one outermost tag.");

                        return result_obj;
                    }
                }
                else /* if (access_mode == XSXML_FILE_MODE) */
                {
                    if (!parse_sub_operation_file_mode( (Xsxml_Files **) object, 
                                                         XSXML_TAG, 
                                                         node_level, 
                                                         word))
                    {
                        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                        sprintf( &result_obj->result_message[0], 
                                 "There cannot be more than one outermost tag.");

                        return result_obj;
                    }
                }

                /* The below code is just a test code for debugging purposes.     */
                /* printf("\n\n%*sTAG = %s", node_level * INDENTATION, "", word); */

                reset_word(&word, &word_len);

                if (!XML_FORWARD_SLASH_END) /* && (!XML_FORWARD_SLASH_START) */
                {
                    node_level++;
                }

                continue;
            }


/* ########################################################################## */
/*                                 XML ATTRIBUTE                              */
/* ########################################################################## */
            if (XML_ATTRIBUTE)
            {
                if (file_data_character == ' ') continue;

                if (XML_FORWARD_SLASH_START)
                {
                    result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                    sprintf( &result_obj->result_message[0], 
                             "End tags cannot have any attributes.");

                    return result_obj;
                }

                if (file_data_character == '=')
                {
                    if (!XML_ATTRIBUTE_NAME)
                    {
                        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                        sprintf( &result_obj->result_message[0], 
                                 "Attribute names cannot be empty "
                                 "(i.e. contain a lone equal-to sign (=).");

                        return result_obj;
                    }

                    /* Working on attribute name. */

                    word [word_len] = 0;

                    if (access_mode == XSXML_RAM_MODE)
                    {
                        if (!parse_sub_operation_ram_mode( (Xsxml **) object, 
                                                            XSXML_ATTRIBUTE_NAME, 
                                                            node_level, 
                                                            word))
                        {
                            result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                            sprintf( &result_obj->result_message[0], 
                                     "Within a given tag, attributes "
                                     "cannot share the same name.");

                            return result_obj;
                        }
                    }
                    else /* if (access_mode == XSXML_FILE_MODE) */
                    {
                        if (!parse_sub_operation_file_mode( (Xsxml_Files **) object, 
                                                             XSXML_ATTRIBUTE_NAME, 
                                                             node_level, 
                                                             word))
                        {
                            result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                            sprintf( &result_obj->result_message[0], 
                                     "Within a given tag, attributes "
                                     "cannot share the same name.");

                            return result_obj;
                        }
                    }

                    /* The below code is just a test code for debugging purposes.              */
                    /* printf("\n%*sATTRIBUTE NAME = %s", node_level * INDENTATION, "", word); */

                    reset_word(&word, &word_len);

                    if (file_data_character == '=') XML_EQUAL = 1;

                    continue;
                }

                if (XML_EQUAL)
                {
                    if (file_data_character == '\'')
                    {
                        XML_AMPERSAND       = 0;
                        XML_DOUBLE_QUOTE    = 0;
                        XML_SINGLE_QUOTE    = 1;
                        XML_ATTRIBUTE_VALUE = 1;
                        continue;
                    }
                    else if (file_data_character == '\"')
                    {
                        XML_AMPERSAND       = 0;
                        XML_SINGLE_QUOTE    = 0;
                        XML_DOUBLE_QUOTE    = 1;
                        XML_ATTRIBUTE_VALUE = 1;
                        continue;
                    }
                    else
                    {
                        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                        sprintf( &result_obj->result_message[0], 
                                 "Attribute assignment (=) must be followed "
                                 "either by a single quote (') "
                                 "or a double quote (\").");

                        return result_obj;
                    }
                }

                if (file_data_character != '/')
                {
                    if (!XML_ATTRIBUTE_NAME)
                    {
                        if (!isalpha(file_data_character) && file_data_character != '_')
                        {
                            result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                            sprintf( &result_obj->result_message[0], 
                                     "Attribute names must start with an "
                                     "alphabetical character [a-zA-Z] or "
                                     "underscore (_)");

                            return result_obj;
                        }

                        XML_ATTRIBUTE_NAME = 1;
                    }

                    append_character_to_word(&word, file_data_character, &word_len);

                    continue;
                }
            }


/* ########################################################################## */
/*                             XML TAG ... CONT'D                             */
/* ########################################################################## */
            if (file_data_character == '/')
            {
                if (XML_FORWARD_SLASH_START)
                {
                    result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                    sprintf( &result_obj->result_message[0], 
                             "A tag cannot have more than one forward slash.");

                    return result_obj;
                }

                XML_FORWARD_SLASH_END = 1;

                continue;
            }

            if (file_data_character == ' ')
            {
                if (word_len != 0)
                {
                    /* Working on tag. */

                    word [word_len] = 0;

                    if (access_mode == XSXML_RAM_MODE)
                    {
                        if (!parse_sub_operation_ram_mode( (Xsxml **) object, 
                                                            XSXML_TAG, 
                                                            node_level, 
                                                            word))
                        {
                            result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                            sprintf( &result_obj->result_message[0], 
                                     "There cannot be more than one outermost tag.");

                            return result_obj;
                        }
                    }
                    else /* if (access_mode == XSXML_FILE_MODE) */
                    {
                        if (!parse_sub_operation_file_mode( (Xsxml_Files **) object, 
                                                             XSXML_TAG, 
                                                             node_level, 
                                                             word))
                        {
                            result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                            sprintf( &result_obj->result_message[0], 
                                     "There cannot be more than one outermost tag.");

                            return result_obj;
                        }
                    }

                    /* The below code is just a test code for debugging purposes.     */
                    /* printf("\n\n%*sTAG = %s", node_level * INDENTATION, "", word); */

                    reset_word(&word, &word_len);

                    XML_ATTRIBUTE      = 1;
                    XML_ATTRIBUTE_NAME = 0;
                }

                continue;
            }

            if (file_data_character == '=')
            {
                if (XML_ATTRIBUTE_VALUE)
                {
                    append_character_to_word(&word, file_data_character, &word_len);
                }

                if (XML_ATTRIBUTE)
                {
                    XML_ATTRIBUTE_VALUE = 1;
                }

                continue;
            }

            if (!isalnum(file_data_character) && (file_data_character != '-') 
            && (file_data_character != '_')    && (file_data_character != '.'))
            {
                result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                sprintf( &result_obj->result_message[0], 
                         "Tag names may contain letters [a-zA-z], "
                         "digits [0-9], hyphens (-), underscores (_), "
                         "and periods (.) only.");

                return result_obj;
            }

            append_character_to_word(&word, file_data_character, &word_len);

            continue;
        }


/* ########################################################################## */
/*                                  XML PCDATA                                */
/* ########################################################################## */
        if ((file_data_character ==  ' ') || (file_data_character == '\r') 
        ||  (file_data_character == '\n') || (file_data_character == '\t') 
        ||  (file_data_character == '\v') || (file_data_character == '\f'))
        {
            if (XML_AMPERSAND)
            {
                result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                sprintf( &result_obj->result_message[0], 
                         "A tag's PCDATA may contain the ampersand (&) "
                         "characters only as character entity references.");

                return result_obj;
            }

            if (!XML_PCDATA_CONTENT) continue;

            if (!XML_WHITESPACE)
            {
                XML_WHITESPACE = 1;
                append_character_to_word(&word, ' ', &word_len);
            }
            continue;
        }

        XML_WHITESPACE = 0;

        if (file_data_character == '<')
        {
            if (XML_AMPERSAND)
            {
                result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                sprintf( &result_obj->result_message[0], 
                         "1A tag's PCDATA may contain the ampersand (&) "
                         "characters only as character entity references.");

                return result_obj;
            }

            XML_TAG                 = 1;
            XML_AMPERSAND           = 0;
            XML_FORWARD_SLASH_START = 0;
            XML_FORWARD_SLASH_END   = 0;

            continue;
        }

        if (file_data_character == '&')
        {
            if (!XML_AMPERSAND)
            {
                cer_i = 0;

                memset( &character_entry_reference[0], 
                        0, 
                        CHARACTER_ENTRY_REFERENCE_MAX_LENGTH);

                character_entry_reference[0] = '&';

                XML_AMPERSAND = 1;

                continue;
            }
        }

        if (file_data_character == ';')
        {
            if (parse_cer(&character_entry_reference))
            {
                append_character_to_word( &word, 
                                          character_entry_reference[0], 
                                          &word_len);
            }
            else
            {
                result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                sprintf( &result_obj->result_message[0], 
                         "A tag's PCDATA may contain the ampersand (&) "
                         "characters only as character entity references.");

                return result_obj;
            }

            XML_AMPERSAND = 0;
            continue;
        }

        if (XML_AMPERSAND)
        {
            if (cer_i == (CHARACTER_ENTRY_REFERENCE_MAX_LENGTH - 1))
            {
                const char *MESSAGE_PART_1 = "A tag's PCDATA may contain the "
                                             "ampersand (&) characters only as "
                                             "character entity references.";

                const char *MESSAGE_PART_2 = "Character entry references "
                                             "may contain atmost ";

                const char *MESSAGE_PART_3 = " number of characters.";

                result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                sprintf( &result_obj->result_message[0], 
                         "%s%s%d%s", 
                         MESSAGE_PART_1, 
                         MESSAGE_PART_2, 
                         CHARACTER_ENTRY_REFERENCE_MAX_LENGTH, 
                         MESSAGE_PART_3);

                return result_obj;
            }

            character_entry_reference [cer_i++] = file_data_character;
            continue;
        }

        if (node_level == 0)
        {
            result_obj->result_code = XSXML_RESULT_XML_FAILURE;

            sprintf( &result_obj->result_message[0], 
                     "Parsed character data (PCDATA or text data) "
                     "cannot be placed outside the outermost tag.");

            return result_obj;
        }


        XML_X                   = 0;    /* X */
        XML_M                   = 0;    /* M */
        XML_EQUAL               = 0;    /* = */
        XML_HYPHEN              = 0;    /* - */
        XML_QUESTION            = 0;    /* ? */
        XML_AMPERSAND           = 0;    /* & */
        XML_SINGLE_QUOTE        = 0;    /* - */
        XML_DOUBLE_QUOTE        = 0;    /* - */
        XML_FORWARD_SLASH_END   = 0;    /* / */
        XML_FORWARD_SLASH_START = 0;    /* / */
        XML_END_SQUARE_BRACKET  = 0;    /* ] */


        XML_PCDATA_CONTENT = 1;

        append_character_to_word(&word, file_data_character, &word_len);
    }


/* ########################################################################## */
/*                          XML TAGS COMPLETION CHECK                         */
/* ########################################################################## */
    if (node_level != 0)
    {
        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

        if (XML_CDATA_CONTENT)
        {
            sprintf( &result_obj->result_message[0], 
                     "A CDATA entity does not terminate.");
        }
        else
        {
            sprintf( &result_obj->result_message[0], 
                     "%d start tag elements do not have their end tag counterparts.", 
                     node_level);
        }

        return result_obj;
    }

    result_obj->result_code = XSXML_RESULT_SUCCESS;

    return result_obj;
}


Xsxml *xsxml_parse(const char *input_file_path)
{
    Xsxml *xsxml_object = (Xsxml *) malloc(sizeof(Xsxml));

    xsxml_object->result_message = (char *) malloc(RESULT_MESSAGE_MAX_LENGTH);

    if ((input_file_path == NULL) || (strlen(input_file_path) == 0))
    {
        xsxml_object->result = XSXML_RESULT_FILE_FAILURE;

        sprintf( &xsxml_object->result_message[0], 
                 "The input file path cannot be empty or NULL.");

        return xsxml_object;
    }

    FILE *file_pointer = fopen(input_file_path, "r");

    if (file_pointer == NULL)
    {
        xsxml_object->result = XSXML_RESULT_FILE_FAILURE;

        sprintf( &xsxml_object->result_message[0], 
                 "The file path '%s' does not exist.", 
                 input_file_path);

        return xsxml_object;
    }

    xsxml_object->number_of_nodes = 0;

    Xsxml_Private_Result *private_result = 
    parse_operation((void **)&xsxml_object, XSXML_RAM_MODE, file_pointer);

    fclose(file_pointer);

    if (private_result->result_code == XSXML_RESULT_SUCCESS)
    {
        sprintf( &xsxml_object->result_message[0], 
                 "The file '%s' has been successfully parsed.", 
                 input_file_path);
    }
    else
    {
        sprintf( &xsxml_object->result_message[0], 
                 "%s", 
                 private_result->result_message);
    }

    free(private_result->result_message);

    /*
    For some unknown, odd reason, the following code produces the 
    'Double free or corruption' error:

    free(private_result);
    */

    return xsxml_object;
}


Xsxml_Files *xsxml_files_parse( const char *input_file_path, 
                                const char *temporary_directory_path)
{
    Xsxml_Files *xsxml_files_object = (Xsxml_Files *) malloc(sizeof(Xsxml));

    xsxml_files_object->result_message = (char *) malloc(RESULT_MESSAGE_MAX_LENGTH);

    if ((input_file_path == NULL) || (strlen(input_file_path) == 0))
    {
        xsxml_files_object->result = XSXML_RESULT_FILE_FAILURE;

        sprintf( &xsxml_files_object->result_message[0], 
                 "The input file path cannot be empty or NULL.");

        return xsxml_files_object;
    }

    FILE *file_pointer = fopen(input_file_path, "r");

    if (file_pointer == NULL)
    {
        xsxml_files_object->result = XSXML_RESULT_FILE_FAILURE;

        sprintf( &xsxml_files_object->result_message[0], 
                 "The file path '%s' does not exist.", 
                 input_file_path);

        return xsxml_files_object;
    }


    /* The plus one (+1) is for the char array's null terminator. */
    xsxml_files_object->node_file_name = 
    (char *) realloc( xsxml_files_object->node_file_name, 
                      NODE_FILE_NAME_SIZE + 1);

    xsxml_files_object->node_file_name [NODE_FILE_NAME_SIZE] = 0;

    srand(time(NULL));

    for (int i = 0; i < NODE_FILE_NAME_SIZE; i++)
    {
        xsxml_files_object->node_file_name [i] = 

            (rand() % NUMBER_OF_ALPHABETIC_CHARACTERS) 
          + LOWER_CASE_LETTER_A_ASCII_VALUE;
    }

    if ((temporary_directory_path != NULL) && (temporary_directory_path[0] != 0))
    {
        const int TEMPORARY_DIRECTORY_PATH_LEN = strlen(temporary_directory_path);

        /* The plus one (+1) is for the char array's null terminator,      */
        /* plus an additional forward slash (/), if required.                 */
        xsxml_files_object->node_directory_path = 
        (char *) realloc( xsxml_files_object->node_directory_path, 
                          TEMPORARY_DIRECTORY_PATH_LEN + 1);

        memcpy( &xsxml_files_object->node_directory_path[0], 
                &temporary_directory_path[0], 
                TEMPORARY_DIRECTORY_PATH_LEN);

        if (xsxml_files_object->node_directory_path [TEMPORARY_DIRECTORY_PATH_LEN - 1]
        != '/')
        {
            xsxml_files_object->node_directory_path [TEMPORARY_DIRECTORY_PATH_LEN]
            = '/';

            xsxml_files_object->node_directory_path [TEMPORARY_DIRECTORY_PATH_LEN + 1]
            = 0;
        }
        else
        {
            xsxml_files_object->node_directory_path[TEMPORARY_DIRECTORY_PATH_LEN]
            = 0;
        }

        FILE *dummy_pointer = open_char_file( xsxml_files_object->node_directory_path, 
                                              xsxml_files_object->node_file_name, 
                                              "w");

        if (dummy_pointer == NULL)
        {
            xsxml_files_object->result = XSXML_RESULT_FILE_FAILURE;

            sprintf( &xsxml_files_object->result_message[0], 
                     "The temporary directory path '%s' does not exist.", 
                     temporary_directory_path);

            return xsxml_files_object;
        }

        fclose(dummy_pointer);

        delete_char_file( xsxml_files_object->node_directory_path, 
                          xsxml_files_object->node_file_name);
    }
    else /* if (temporary_directory_path == NULL) */
    {
        char *file_path_split = strrchr(input_file_path, '/');

        if (file_path_split == NULL)
        {
            xsxml_files_object->node_directory_path = 
            (char *) realloc(xsxml_files_object->node_directory_path, 1);

            xsxml_files_object->node_directory_path[0] = 0;
        }
        else
        {
            /* The plus one (+1) is for the char array's null terminator. */
            xsxml_files_object->node_directory_path = 
            (char *) realloc( xsxml_files_object->node_directory_path, 
                              strlen(input_file_path));

            memcpy( &xsxml_files_object->node_directory_path[0], 
                    &input_file_path[0], 
                    file_path_split - input_file_path);
        }
    }

    xsxml_files_object->number_of_nodes = 0;

    Xsxml_Private_Result *private_result = 
    parse_operation((void **)&xsxml_files_object, XSXML_FILE_MODE, file_pointer);

    fclose(file_pointer);

    if (private_result->result_code == XSXML_RESULT_SUCCESS)
    {
        sprintf( &xsxml_files_object->result_message[0], 
                 "The file '%s' has been successfully parsed.", 
                 input_file_path);
    }
    else
    {
        sprintf( &xsxml_files_object->result_message[0], 
                 "%s", 
                 private_result->result_message);
    }

    free(private_result->result_message);

    /*
    For some unknown, odd reason, the following code produces the 
    'Double free or corruption' error:

    free(private_result);
    */

    return xsxml_files_object;
}


char *xsxml_files_property( Xsxml_Files *xsxml_files_object, 
                            size_t node_index, 
                            Xsxml_Property property_name, 
                            size_t property_index)
{
    Xsxml_Char_File_IO io_obj;

    io_obj.temporary_dir_path  = xsxml_files_object->node_directory_path;
    io_obj.temporary_file_name = xsxml_files_object->node_file_name;
    io_obj.node_i_value        = node_index;
    io_obj.property_term       = (char *) PROPERTY_NAMES_LIST [property_name];
    io_obj.node_j_value        = property_index;
    io_obj.data_str            = NULL;

    read_from_char_file(&io_obj);

    return io_obj.data_str;
}


size_t *xsxml_occurrence( Xsxml *xsxml_object, 
                          char *tag_name, 
                          char *attribute_name, 
                          char *attribute_value, 
                          char *content, 
                          Xsxml_Direction direction)
{
    size_t *return_node_indices = (size_t *) malloc(sizeof(size_t));

    return_node_indices[0] = 0;

    if ((tag_name == NULL) && (attribute_name  == NULL) 
    &&  (content  == NULL) && (attribute_value == NULL))
    {
        return return_node_indices;
    }

    size_t i_start;
    size_t i_negatory_end_condition;

    if (direction == XSXML_DIRECTION_FORWARD)
    {
        i_start = 0;
        i_negatory_end_condition = xsxml_object->number_of_nodes;
    }
    else /* if (direction == XSXML_DIRECTION_BACKWARD) */
    {
        i_start = xsxml_object->number_of_nodes - 1;
        i_negatory_end_condition = (unsigned int)-1;
        direction = -1;
    }

    for (unsigned int i = i_start; i != i_negatory_end_condition; i += direction)
    {
        int node_i_is_true = 0;

        if (tag_name != NULL)
        {
            if (strcmp(xsxml_object->node[i]->node_name, tag_name) == 0)
                node_i_is_true = 1;
            else
                continue;
        }

        for (unsigned j = 0; j < xsxml_object->node[i]->number_of_attributes; j++)
        {
            if (attribute_name != NULL)
            {
                if (strcmp(xsxml_object->node[i]->attribute_name[j], attribute_name) == 0)
                {
                    node_i_is_true = 1;
                }
                else
                {
                    node_i_is_true = 0;
                    continue;
                }
            }

            if (attribute_value != NULL)
            {
                if (strcmp(xsxml_object->node[i]->attribute_value[j], attribute_value) == 0)
                {
                    node_i_is_true = 1;
                }
                else
                {
                    node_i_is_true = 0;
                    continue;
                }
            }

            if (node_i_is_true) break;
        }

        if (content != NULL)
        {
            node_i_is_true = 0;

            for (unsigned k = 0; k < xsxml_object->node[i]->number_of_contents; k++)
            {
                if (strstr(xsxml_object->node[i]->content[k], content) != NULL)
                {
                    node_i_is_true = 1;
                    break;
                }
            }
        }

        if (node_i_is_true)
        {
            return_node_indices = 
            (size_t *) realloc( return_node_indices, 
                                (++return_node_indices[0] + 1) * sizeof(size_t));

            return_node_indices [return_node_indices[0]] = i;
        }
    }

    return return_node_indices;
}


size_t *xsxml_files_occurrence( Xsxml_Files *xsxml_files_object, 
                                char *tag_name, 
                                char *attribute_name, 
                                char *attribute_value, 
                                char *content, 
                                Xsxml_Direction direction)
{
    size_t *return_node_indices = (size_t *) malloc(sizeof(size_t));

    return_node_indices[0] = 0;

    if ((tag_name == NULL) && (attribute_name  == NULL) 
    &&  (content  == NULL) && (attribute_value == NULL))
    {
        return return_node_indices;
    }

    size_t i_start;
    size_t i_negatory_end_condition;

    if (direction == XSXML_DIRECTION_FORWARD)
    {
        i_start = 0;
        i_negatory_end_condition = xsxml_files_object->number_of_nodes;
    }
    else /* if (direction == XSXML_DIRECTION_BACKWARD) */
    {
        i_start = xsxml_files_object->number_of_nodes - 1;
        i_negatory_end_condition = (unsigned int)-1;
        direction = -1;
    }

    Xsxml_Char_File_IO io_obj;

    io_obj.temporary_dir_path  = xsxml_files_object->node_directory_path;
    io_obj.temporary_file_name = xsxml_files_object->node_file_name;

    for (unsigned int i = i_start; i != i_negatory_end_condition; i += direction)
    {
        io_obj.node_j_value = -1;
        io_obj.data_str     = NULL;

        int node_i_is_true = 0;

        io_obj.node_i_value = i;

        if (tag_name != NULL)
        {
            io_obj.property_term = "nn";
            read_from_char_file(&io_obj);

            if (strcmp(io_obj.data_str, tag_name) == 0)
                node_i_is_true = 1;
            else
                continue;
        }

        io_obj.property_term = "aN";
        io_obj.data_str      = "dummy";
        read_from_char_file(&io_obj);

        for (unsigned j = 0; j < io_obj.data_int; j++)
        {
            if (attribute_name != NULL)
            {
                io_obj.node_j_value  =  j;
                io_obj.property_term = "an";
                io_obj.data_str      = NULL;
                read_from_char_file(&io_obj);

                if (strcmp(io_obj.data_str, attribute_name) == 0)
                {
                    node_i_is_true = 1;
                }
                else
                {
                    node_i_is_true = 0;
                    continue;
                }
            }

            if (attribute_value != NULL)
            {
                io_obj.node_j_value  =  j;
                io_obj.property_term = "av";
                io_obj.data_str      = NULL;
                read_from_char_file(&io_obj);

                if (strcmp(io_obj.data_str, attribute_value) != 0)
                {
                    node_i_is_true = 1;
                }
                else
                {
                    node_i_is_true = 0;
                    continue;
                }
            }

            if (node_i_is_true) break;
        }

        if (content != NULL)
        {
            node_i_is_true = 0;

            io_obj.property_term = "cN";
            io_obj.data_str      = "dummy";
            read_from_char_file(&io_obj);

            for (unsigned k = 0; k < io_obj.data_int; k++)
            {
                io_obj.node_j_value  =  k;
                io_obj.property_term = "c";
                io_obj.data_str      = NULL;
                read_from_char_file(&io_obj);

                if (strstr(io_obj.data_str, content) != NULL)
                {
                    node_i_is_true = 1;
                    break;
                }
            }
        }

        if (node_i_is_true)
        {
            return_node_indices = 
            (size_t *) realloc( return_node_indices, 
                                (++return_node_indices[0] + 1) * sizeof(size_t));

            return_node_indices [return_node_indices[0]] = i;
        }
    }

    return return_node_indices;
}


static void compile_all_nodes( Xsxml_Private_Result *result_obj, 
                               Xsxml_Nodes *xsxml_node_object, 
                               FILE *save_file_pointer, 
                               unsigned int indentation, 
                               unsigned int vertical_spacing, 
                               unsigned int *level)
{
    /* Validating tag. */

    if ((strchr(xsxml_node_object->node_name,  ' ') != NULL) 
    ||  (strchr(xsxml_node_object->node_name, '\r') != NULL) 
    ||  (strchr(xsxml_node_object->node_name, '\n') != NULL) 
    ||  (strchr(xsxml_node_object->node_name, '\t') != NULL) 
    ||  (strchr(xsxml_node_object->node_name, '\v') != NULL) 
    ||  (strchr(xsxml_node_object->node_name, '\f') != NULL))
    {
        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

        sprintf( &result_obj->result_message[0], 
                 "Tag names cannot have any space characters.");

        return;
    }

    if (!isalpha(xsxml_node_object->node_name[0]) 
    &&  (xsxml_node_object->node_name[0] != '_'))
    {
        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

        sprintf( &result_obj->result_message[0], 
                 "Tag names must start with an alphabetical character "
                 "[a-zA-Z] or underscore (_)");

        return;
    }

    if ((tolower(xsxml_node_object->node_name[0]) == 'x') 
    &&  (tolower(xsxml_node_object->node_name[1]) == 'm') 
    &&  (tolower(xsxml_node_object->node_name[2]) == 'l'))
    {
        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

        sprintf( &result_obj->result_message[0], 
                 "Tag names cannot start with 'xml' or "
                 "any of its variants like XML, Xml, etc.");

        return;
    }

    for (unsigned int i = 0; i < strlen(xsxml_node_object->node_name); i++)
    {
        if (!isalnum(xsxml_node_object->node_name[i]) 
        &&  (xsxml_node_object->node_name[i] != '-') 
        &&  (xsxml_node_object->node_name[i] != '_') 
        &&  (xsxml_node_object->node_name[i] != '.'))
        {
            result_obj->result_code = XSXML_RESULT_XML_FAILURE;

            sprintf( &result_obj->result_message[0], 
                     "Tag names may contain letters [a-zA-z], "
                     "digits [0-9], hyphens (-), underscores (_), "
                     "and periods (.) only.");

            return;
        }
    }

    fprintf( save_file_pointer, 
             "%*s<%s", 
             indentation * (*level), "", 
             xsxml_node_object->node_name);

    const unsigned int n_attributes = xsxml_node_object->number_of_attributes;

    for (unsigned int j = 0; j < n_attributes; j++)
    {
        /* Validating attribute name. */

        for (unsigned int k = 0; k < n_attributes; k++)
        {
            if (k == j) continue;

            if (strcmp(xsxml_node_object->attribute_name[j], xsxml_node_object->attribute_name[k]) == 0)
            {
                result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                sprintf( &result_obj->result_message[0], 
                         "Within a given tag, attributes "
                         "cannot share the same name.");

                return;
            }
        }

        if ((strchr(xsxml_node_object->attribute_name[j],  ' ') != NULL) 
        ||  (strchr(xsxml_node_object->attribute_name[j], '\r') != NULL) 
        ||  (strchr(xsxml_node_object->attribute_name[j], '\n') != NULL) 
        ||  (strchr(xsxml_node_object->attribute_name[j], '\t') != NULL) 
        ||  (strchr(xsxml_node_object->attribute_name[j], '\v') != NULL) 
        ||  (strchr(xsxml_node_object->attribute_name[j], '\f') != NULL))
        {
            result_obj->result_code = XSXML_RESULT_XML_FAILURE;

            sprintf( &result_obj->result_message[0], 
                     "Tag names cannot have any space characters.");

            return;
        }

        if (!isalpha(xsxml_node_object->attribute_name[j][0]) 
        &&  (xsxml_node_object->attribute_name[j][0] != '_'))
        {
            result_obj->result_code = XSXML_RESULT_XML_FAILURE;

            sprintf( &result_obj->result_message[0], 
                     "Tag names must start with an alphabetical character "
                     "[a-zA-Z] or underscore (_)");

            return;
        }

        for (unsigned int i = 0; i < strlen(xsxml_node_object->attribute_name[j]); i++)
        {
            if (!isalnum(xsxml_node_object->attribute_name[j][i]) 
            &&  (xsxml_node_object->attribute_name[j][i] != '-') 
            &&  (xsxml_node_object->attribute_name[j][i] != '_') 
            &&  (xsxml_node_object->attribute_name[j][i] != '.'))
            {
                result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                sprintf( &result_obj->result_message[0], 
                         "Tag names may contain letters [a-zA-z], "
                         "digits [0-9], hyphens (-), underscores (_), "
                         "and periods (.) only.");

                return;
            }
        }


        /* Validating attribute value. */

        if (strchr(xsxml_node_object->attribute_value[j],  '<') != NULL)
        {
            result_obj->result_code = XSXML_RESULT_XML_FAILURE;

            sprintf( &result_obj->result_message[0], 
                     "Attribute values may not contain the "
                     "less-than (<) characters.");

            return;
        }

        size_t ret_0 = 0;

        while (1)
        {
            const char *ret_1 = strchr(&xsxml_node_object->attribute_value[j][ret_0], '&');

            if (ret_1 == NULL)
            {
                break;
            }
            else /* if (ret_1 != NULL) */
            {
                const char *ret_2 = strchr( &xsxml_node_object->attribute_value
                                        [j][ret_1 - xsxml_node_object->attribute_value[j]], 
                                      ';');

                if (ret_2 == NULL)
                {
                    result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                    sprintf( &result_obj->result_message[0], 
                             "Attribute values may contain the ampersand (&) "
                             "characters only as character entity references.");

                    return;
                }
                else /* if (ret_2 != NULL) */
                {
                    char *cer_data = (char *) malloc(ret_2 - ret_1);

                    memcpy( &cer_data[0], 
                            &xsxml_node_object->attribute_value
                                [j][ret_1 - xsxml_node_object->attribute_value[j]], 
                            ret_2 - ret_1 - 1);

                    if (!parse_cer(&cer_data))
                    {
                        free(cer_data);

                        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                        sprintf( &result_obj->result_message[0], 
                                 "Attribute values may contain the ampersand (&) "
                                 "characters only as character entity references.");

                        return;
                    }

                    free(cer_data);
                }

                ret_0 = ret_2 - xsxml_node_object->attribute_value[j];
            }
        }

        fprintf( save_file_pointer, 
                 " %s=\"%s\"", 
                 xsxml_node_object->attribute_name[j], 
                 xsxml_node_object->attribute_value[j]);
    }

    fprintf(save_file_pointer, ">\n");

    for (unsigned int count = 0; count < vertical_spacing; count++)
    {
        fprintf(save_file_pointer, "\n");
    }

    unsigned int content_i = 0;

    const unsigned int n_contents = xsxml_node_object->number_of_contents;

    if (n_contents > 0)
    {
        /* Validating PCDATA. */

        for (unsigned int j = 0; j < n_contents; j++)
        {
            // <![CDATA[ ]]>

            size_t ret_0 = 0;

            int cdata_tags_n = 0;

            size_t *cdata_tag_pos_start = (size_t *) malloc(1 * sizeof(size_t));
            size_t *cdata_tag_pos_end   = (size_t *) malloc(1 * sizeof(size_t));

            while (1)
            {
                const char *ret_1 = strstr(&xsxml_node_object->content[j][ret_0], "<![CDATA[");

                if (ret_1 == NULL)
                {
                    break;
                }
                else /* if (ret_1 != NULL) */
                {
                    const char *ret_2 = strstr( &xsxml_node_object->content
                                                    [j][ret_1 - xsxml_node_object->content[j]], 
                                                "]]>");

                    if (ret_2 == NULL)
                    {
                        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                        sprintf( &result_obj->result_message[0], 
                                 "A CDATA entity does not terminate.");

                        return;
                    }
                    else /* if (ret_2 != NULL) */
                    {
                        cdata_tags_n++;

                        cdata_tag_pos_start = 
                        (size_t *) realloc( cdata_tag_pos_start, 
                                            cdata_tags_n * sizeof(size_t));

                        cdata_tag_pos_end = 
                        (size_t *) realloc( cdata_tag_pos_end, 
                                            cdata_tags_n * sizeof(size_t));

                        cdata_tag_pos_start [cdata_tags_n - 1] = 
                        ret_1 - xsxml_node_object->content[j];

                        cdata_tag_pos_end [cdata_tags_n - 1] = 
                        ret_2 - xsxml_node_object->content[j];
                    }

                    ret_0 = ret_2 - xsxml_node_object->content[j];
                }
            }

            ret_0 = 0;

            while (1)
            {
                const char *ret_1 = strchr(&xsxml_node_object->content[j][ret_0],  '<');

                const size_t ret_1_pos = ret_1 - xsxml_node_object->content[j];

                if (ret_1 == NULL)
                {
                    break;
                }
                else /* if (ret_1 != NULL) */
                {
                    int char_is_valid = 0;

                    for (int k = 0; k < cdata_tags_n; k++)
                    {
                        if ((ret_1_pos > cdata_tag_pos_start[k]) 
                        &&  (ret_1_pos < cdata_tag_pos_end[k]))
                        {
                            char_is_valid = 1;
                            break;
                        }
                    }

                    if (!char_is_valid)
                    {
                        result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                        sprintf( &result_obj->result_message[0], 
                                 "A tag's PCDATA may not contain the "
                                 "less-than (<) characters.");

                        return;
                    }
                }

                ret_0 = ret_1_pos;
            }

            ret_0 = 0;

            while (1)
            {
                const char *ret_1 = strchr(&xsxml_node_object->content[j][ret_0], '&');

                const size_t ret_1_pos = ret_1 - xsxml_node_object->content[j];

                if (ret_1 == NULL)
                {
                    break;
                }
                else /* if (ret_1 != NULL) */
                {
                    int char_is_valid = 0;

                    for (int k = 0; k < cdata_tags_n; k++)
                    {
                        if ((ret_1_pos > cdata_tag_pos_start[k]) 
                        &&  (ret_1_pos < cdata_tag_pos_end[k]))
                        {
                            char_is_valid = 1;
                            break;
                        }
                    }

                    if (!char_is_valid)
                    {
                        const char *ret_2 = strchr( &xsxml_node_object->content
                                                        [j][ret_1_pos], 
                                                    ';');

                        if (ret_2 == NULL)
                        {
                            result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                            sprintf( &result_obj->result_message[0], 
                                     "A tag's PCDATA may contain the ampersand (&) "
                                     "characters only as character entity references.");

                            return;
                        }
                        else /* if (ret_2 != NULL) */
                        {
                            char *cer_data = (char *) malloc(ret_2 - ret_1);

                            memcpy( &cer_data[0], 
                                    &xsxml_node_object->attribute_value
                                        [j][ret_1 - xsxml_node_object->attribute_value[j]], 
                                    ret_2 - ret_1 - 1);

                            if (!parse_cer(&cer_data))
                            {
                                free(cer_data);

                                result_obj->result_code = XSXML_RESULT_XML_FAILURE;

                                sprintf( &result_obj->result_message[0], 
                                         "A tag's PCDATA may contain the ampersand (&) "
                                         "characters only as character entity references.");

                                return;
                            }

                            free(cer_data);
                        }

                        ret_0 = ret_2 - xsxml_node_object->attribute_value[j];
                    }
                    else /* if (char_is_valid) */
                    {
                        ret_0 = ret_1_pos;
                    }
                }
            }
        }
    }

    if (n_contents > 0)
    {
        fprintf(save_file_pointer, "%*s", indentation * ((*level) + 1), "");

        const size_t CONTENT_LEN = strlen(xsxml_node_object->content[content_i]);

        for (unsigned int i = 0; i < CONTENT_LEN; i++)
        {
            if (xsxml_node_object->content[content_i][i] == '\n')
                fprintf(save_file_pointer, "%*s", indentation * ((*level) + 1), "");
            else
                fputc(xsxml_node_object->content[content_i][i], save_file_pointer);
        }

        fputc('\n', save_file_pointer);

        content_i++;

        for (unsigned int count = 0; count < vertical_spacing; count++)
        {
            fprintf(save_file_pointer, "\n");
        }
    }

    if (xsxml_node_object->descendant != NULL)
    {
        Xsxml_Nodes *xsxml_sub_node_object = xsxml_node_object->descendant;

        (*level)++;

        while (xsxml_sub_node_object != NULL)
        {
            compile_all_nodes( result_obj, 
                               xsxml_sub_node_object, 
                               save_file_pointer, 
                               indentation, 
                               vertical_spacing, 
                               level);

            if (result_obj->result_code != XSXML_RESULT_SUCCESS) return;

            if (n_contents > 1)
            {
                fprintf(save_file_pointer, "%*s", indentation * ((*level) + 1), "");

                const size_t CONTENT_LEN = strlen(xsxml_node_object->content[content_i]);

                for (unsigned int i = 0; i < CONTENT_LEN; i++)
                {
                    if (xsxml_node_object->content[content_i][i] == '\n')
                        fprintf(save_file_pointer, "%*s", indentation * ((*level) + 1), "");
                    else
                        fputc(xsxml_node_object->content[content_i][i], save_file_pointer);
                }

                fputc('\n', save_file_pointer);

                content_i++;

                for (unsigned int count = 0; count < vertical_spacing; count++)
                {
                    fprintf(save_file_pointer, "\n");
                }
            }

            xsxml_sub_node_object = xsxml_sub_node_object->next_sibling;
        }

        (*level)--;
    }

    fprintf( save_file_pointer, 
             "%*s",  
             indentation * (*level), "");

    fprintf( save_file_pointer, 
             "</%s>\n",  
             xsxml_node_object->node_name);

    for (unsigned int count = 0; count < vertical_spacing; count++)
    {
        fprintf(save_file_pointer, "\n");
    }
}


void xsxml_compile( Xsxml *xsxml_object, 
                    const char *save_directory, 
                    const char *save_file_name, 
                    unsigned int indentation, 
                    unsigned int vertical_spacing)
{
    if (xsxml_object->result_message == NULL)
    {
        xsxml_object->result_message = (char *) malloc(RESULT_MESSAGE_MAX_LENGTH);
    }

    if (indentation > MAX_INDENTATION)
    {
        xsxml_object->result = XSXML_RESULT_FILE_FAILURE;

        sprintf( &xsxml_object->result_message[0], 
                 "Error in input argument number 4 of function 'xsxml_compile'."
                 "You cannot have more than %u horizontal spaces as indentation.", 
                 MAX_INDENTATION);

        return;
    }

    if (vertical_spacing > MAX_VERTICAL_SPACING)
    {
        xsxml_object->result = XSXML_RESULT_FILE_FAILURE;

        sprintf( &xsxml_object->result_message[0], 
                 "Error in input argument number 5 of function 'xsxml_compile'."
                 "You cannot have more than %u vertical spaces.", 
                 MAX_VERTICAL_SPACING);

        return;
    }

    char *save_directory_modified;

    if ((save_directory == NULL) || (save_directory[0] == 0))
    {
        save_directory_modified    = malloc(sizeof(char));

        save_directory_modified[0] = 0;
    }
    else /* if (save_directory != NULL) */
    {
        /* The plus one (+1) is for the forward slash character, if required. */
        save_directory_modified = malloc(strlen(save_directory) + 1);

        memcpy( &save_directory_modified[0], 
                &save_directory[0], 
                strlen(save_directory));

        if (save_directory_modified [strlen(save_directory_modified) - 1] != '/')
        {
            save_directory_modified [strlen(save_directory_modified)] = '/';
        }
    }

    /* The plus one (+1) is for the char array's null terminator. */
    char save_file_path [strlen(save_directory_modified) + strlen(save_file_name) + 1];

    save_file_path [strlen(save_directory_modified) + strlen(save_file_name)] = 0;

    memcpy( &save_file_path[0], 
            &save_directory_modified[0], 
            strlen(save_directory_modified));

    memcpy( &save_file_path[strlen(save_directory_modified)], 
            &save_file_name[0], 
            strlen(save_file_name));

    free(save_directory_modified);

    FILE *save_file_pointer = fopen(save_file_path, "w");

    if (save_file_pointer == NULL)
    {
        xsxml_object->result = XSXML_RESULT_FILE_FAILURE;

        sprintf( &xsxml_object->result_message[0], 
                 "The save directory '%s' does not exist.", 
                 save_directory);

        return;
    }

    if (xsxml_object->node == NULL)
    {
        xsxml_object->result = XSXML_RESULT_XML_FAILURE;

        sprintf( &xsxml_object->result_message[0], 
                 "There must be exactly one outermost tag.");

        return;
    }

    if (xsxml_object->node[0] == NULL)
    {
        xsxml_object->result = XSXML_RESULT_XML_FAILURE;

        sprintf( &xsxml_object->result_message[0], 
                 "There must be exactly one outermost tag.");

        return;
    }

    fprintf(save_file_pointer, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n");

    for (unsigned int count = 0; count < vertical_spacing; count++)
    {
        fprintf(save_file_pointer, "\n");
    }

    unsigned int level = 0;

    Xsxml_Private_Result private_result;

    private_result.result_code = XSXML_RESULT_SUCCESS;

    private_result.result_message = (char *) malloc(RESULT_MESSAGE_MAX_LENGTH);

    compile_all_nodes( &private_result, 
                       xsxml_object->node[0], 
                       save_file_pointer, 
                       indentation, 
                       vertical_spacing, 
                       &level);

    if (private_result.result_code == XSXML_RESULT_SUCCESS)
    {
        fprintf(save_file_pointer, "\n");

        fclose(save_file_pointer);

        xsxml_object->result = XSXML_RESULT_SUCCESS;

        sprintf( &xsxml_object->result_message[0], 
                 "The XSXML object's information has been successfully compiled "
                 "into file '%s'.", 
                 save_file_name);
    }
    else /* if (private_result.result_code != XSXML_RESULT_SUCCESS) */
    {
        fclose(save_file_pointer);

        remove(save_file_path);

        xsxml_object->result = private_result.result_code;

        sprintf( &xsxml_object->result_message[0], 
                 "%s", 
                 private_result.result_message);
    }
}

