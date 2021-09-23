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


#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/stat.h>

#include "xsxml.h"

#include "xsxml_binary_object.h"


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


#define RESULT_MESSAGE_MAX_LENGTH 200

#define UINT_FIXED_BYTES uint8_t
/*
Either of the two macros given below can also be used instead, if required:
1. #define UINT_FIXED_BYTES uint16_t
2. #define UINT_FIXED_BYTES uint8_t

An 8-bit unsigned integer can deal with a maximum of           256 tag elements (nodes).
A 16-bit unsigned integer can deal with a maximum of        65,536 tag elements (nodes).
A 32-bit unsigned integer can deal with a maximum of 4,294,967,296 tag elements (nodes).

An 8-bit-based binary file would be smallest in terms of file size, 
followed by a 16-bit file, 
followed by a 32-bit file, which would be comparatively larger in file size.
*/


void create_xsxml_binary_object( Xsxml *xsxml_object, 
                                 const char *save_directory, 
                                 const char *save_file_name)
{
    if (xsxml_object->result_message == NULL)
    {
        xsxml_object->result_message = (char *) malloc(RESULT_MESSAGE_MAX_LENGTH);
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

    /* The plus seven (+7) is for the char array's null terminator, */
    /* plus the file extension *.xsxml.                             */
    char save_file_path [strlen(save_directory_modified) + strlen(save_file_name) + 7];

    strcpy(save_file_path, save_directory_modified);

    strcat(save_file_path, save_file_name);

    strcat(save_file_path, ".xsxml");

    free(save_directory_modified);


    if (xsxml_object->node == NULL)
    {
        xsxml_object->result = XSXML_RESULT_XML_FAILURE;

        sprintf( &xsxml_object->result_message[0], 
                 "There must be atleast one tag.");

        return;
    }

    if (xsxml_object->number_of_nodes == 0)
    {
        xsxml_object->result = XSXML_RESULT_XML_FAILURE;

        sprintf( &xsxml_object->result_message[0], 
                 "There must be atleast one tag.");

        return;
    }


    /* Main operation starts here. */
        int      dummy_value;
        UINT_FIXED_BYTES string_len;

        dummy_value = remove(save_file_path);

        int file_descriptor = open( save_file_path, 
                                    O_WRONLY | O_CREAT | O_TRUNC, 
                                    S_IRUSR);

        if ((file_descriptor == EEXIST) || (file_descriptor == EACCES))
        {
            xsxml_object->result = XSXML_RESULT_FILE_FAILURE;

            sprintf( &xsxml_object->result_message[0], 
                     "The save directory '%s' does not exist.", 
                     save_directory);

            return;
        }


        /* Write the number of nodes. */
        dummy_value = write( file_descriptor, 
                             &xsxml_object->number_of_nodes, 
                             sizeof(UINT_FIXED_BYTES));

        for (unsigned int i = 0; i < xsxml_object->number_of_nodes; i++)
        {
            if (xsxml_object->node[0] == NULL)
            {
                dummy_value = remove(save_file_path);

                xsxml_object->result = XSXML_RESULT_XML_FAILURE;

                sprintf( &xsxml_object->result_message[0], 
                         "Tag %u of %u does not exist.", 
                         i + 1, xsxml_object->number_of_nodes);

                return;
            }

            /* Write the node level. */
            dummy_value = write( file_descriptor, 
                                 &xsxml_object->node[i]->depth, 
                                 sizeof(UINT_FIXED_BYTES));

            /* Write the number of contents. */
            dummy_value = write( file_descriptor, 
                                 &xsxml_object->node[i]->number_of_contents, 
                                 sizeof(UINT_FIXED_BYTES));

            /* Write the number of attributes. */
            dummy_value = write( file_descriptor, 
                                 &xsxml_object->node[i]->number_of_attributes, 
                                 sizeof(UINT_FIXED_BYTES));

            /* Write the node name length. */
            string_len = strlen(xsxml_object->node[i]->node_name);
            dummy_value = write( file_descriptor, 
                                 &string_len, 
                                 sizeof(UINT_FIXED_BYTES));

            /* Write the node name. */
            dummy_value = write( file_descriptor, 
                                 xsxml_object->node[i]->node_name, 
                                 string_len);

            for (unsigned int j = 0; j < xsxml_object->node[i]->number_of_contents; j++)
            {
                if (xsxml_object->node[i]->content[j] == NULL)
                {
                    dummy_value = remove(save_file_path);

                    xsxml_object->result = XSXML_RESULT_XML_FAILURE;

                    sprintf( &xsxml_object->result_message[0], 
                             "Content %u of %u does not exist.", 
                             j + 1, xsxml_object->node[i]->number_of_contents);

                    return;
                }

                /* Write the jth content length. */
                string_len = strlen(xsxml_object->node[i]->content[j]);
                dummy_value = write( file_descriptor, 
                                     &string_len, 
                                     sizeof(UINT_FIXED_BYTES));

                /* Write the jth content. */
                dummy_value = write( file_descriptor, 
                                     xsxml_object->node[i]->content[j], 
                                     string_len);
            }

            for (unsigned int j = 0; j < xsxml_object->node[i]->number_of_attributes; j++)
            {
                if ((xsxml_object->node[i]->attribute_name [j] == NULL) 
                ||  (xsxml_object->node[i]->attribute_value[j] == NULL))
                {
                    dummy_value = remove(save_file_path);

                    xsxml_object->result = XSXML_RESULT_XML_FAILURE;

                    sprintf( &xsxml_object->result_message[0], 
                             "Attribute %u of %u either does not exist or is incomplete.", 
                             j + 1, xsxml_object->node[i]->number_of_attributes);

                    return;
                }

                /* Write the jth attribute name length. */
                string_len = strlen(xsxml_object->node[i]->attribute_name[j]);
                dummy_value = write( file_descriptor, 
                                     &string_len, 
                                     sizeof(UINT_FIXED_BYTES));

                /* Write the jth attribute name. */
                dummy_value = write( file_descriptor, 
                                     xsxml_object->node[i]->attribute_name[j], 
                                     string_len);

                /* Write the jth attribute value length. */
                string_len = strlen(xsxml_object->node[i]->attribute_value[j]);
                dummy_value = write( file_descriptor, 
                                     &string_len, 
                                     sizeof(UINT_FIXED_BYTES));

                /* Write the jth attribute value. */
                dummy_value = write( file_descriptor, 
                                     xsxml_object->node[i]->attribute_value[j], 
                                     string_len);
            }
        }


        for (unsigned int i = 0; i < xsxml_object->number_of_nodes; i++)
        {
            /* Write the ancestor, descendant, previous and next siblings nodes. */
                UINT_FIXED_BYTES node_numbers[4] = { 0, 0, 0, 0 };

                for (unsigned int j = 0; j < xsxml_object->number_of_nodes; j++)
                {
                    if (!node_numbers[0])
                    {
                        if (xsxml_object->node[i]->ancestor == xsxml_object->node[j])
                        {
                            node_numbers[0] = j + 1;
                        }
                    }

                    if (!node_numbers[1])
                    {
                        if (xsxml_object->node[i]->descendant == xsxml_object->node[j])
                        {
                            node_numbers[1] = j + 1;
                        }
                    }

                    if (!node_numbers[2])
                    {
                        if (xsxml_object->node[i]->previous_sibling == xsxml_object->node[j])
                        {
                            node_numbers[2] = j + 1;
                        }
                    }

                    if (!node_numbers[3])
                    {
                        if (xsxml_object->node[i]->next_sibling == xsxml_object->node[j])
                        {
                            node_numbers[3] = j + 1;
                        }
                    }
                }

                dummy_value = write( file_descriptor, 
                                     &node_numbers[0], 
                                     sizeof(UINT_FIXED_BYTES));

                dummy_value = write( file_descriptor, 
                                     &node_numbers[1], 
                                     sizeof(UINT_FIXED_BYTES));

                dummy_value = write( file_descriptor, 
                                     &node_numbers[2], 
                                     sizeof(UINT_FIXED_BYTES));

                dummy_value = write( file_descriptor, 
                                     &node_numbers[3], 
                                     sizeof(UINT_FIXED_BYTES));
        }

        close(file_descriptor);
    /* Main operation ends here. */


    if (dummy_value) dummy_value = 0;


    xsxml_object->result = XSXML_RESULT_SUCCESS;

    sprintf( &xsxml_object->result_message[0], 
             "The XSXML object's information has been successfully compiled "
             "into the binary file '%s.xsxml'.", 
             save_file_name);
}


Xsxml *decode_xsxml_binary_object(const char *input_file_path)
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


    /* Main operation starts here. */
        int file_descriptor = open(input_file_path, O_RDONLY);

        if (file_descriptor == EEXIST)
        {
            xsxml_object->result = XSXML_RESULT_FILE_FAILURE;

            sprintf( &xsxml_object->result_message[0], 
                     "The file path '%s' does not exist.", 
                     input_file_path);

            return xsxml_object;
        }


        int dummy_value;

        UINT_FIXED_BYTES string_len;


        /* Read the number of nodes. */
        dummy_value = read( file_descriptor, 
                            (unsigned int *) &xsxml_object->number_of_nodes, 
                            sizeof(UINT_FIXED_BYTES));


        xsxml_object->node = 
        (Xsxml_Nodes **) malloc(xsxml_object->number_of_nodes * sizeof(Xsxml_Nodes *));


        for (unsigned int i = 0; i < xsxml_object->number_of_nodes; i++)
        {
            xsxml_object->node[i] = (Xsxml_Nodes *) malloc(sizeof(Xsxml_Nodes));

            /* Read the node level. */
            dummy_value = read( file_descriptor, 
                                &xsxml_object->node[i]->depth, 
                                sizeof(UINT_FIXED_BYTES));

            /* Read the number of contents. */
            dummy_value = read( file_descriptor, 
                                &xsxml_object->node[i]->number_of_contents, 
                                sizeof(UINT_FIXED_BYTES));

            /* Read the number of attributes. */
            dummy_value = read( file_descriptor, 
                                &xsxml_object->node[i]->number_of_attributes, 
                                sizeof(UINT_FIXED_BYTES));

            /* Read the node name length. */
            dummy_value = read( file_descriptor, 
                                &string_len, 
                                sizeof(UINT_FIXED_BYTES));

            /* The plus one (+1) is for the null terminator. */
            xsxml_object->node[i]->node_name = (char *) malloc(string_len + 1);

            xsxml_object->node[i]->node_name [string_len] = 0;

            /* The plus one (+1) is for the null terminator. */
            char temp_string_1 [string_len + 1];

            /* Read the node name. */
            dummy_value = read(file_descriptor, temp_string_1, string_len);

            memcpy( &xsxml_object->node[i]->node_name[0], 
                    &temp_string_1[0], 
                    string_len);

            if (xsxml_object->node[i]->number_of_contents == 0)
            {
                xsxml_object->node[i]->content = 
                (char **) malloc(sizeof(char *));
            }
            else
            {
                xsxml_object->node[i]->content = 
                (char **) malloc(xsxml_object->node[i]->number_of_contents * sizeof(char *));
            }

            for (unsigned int j = 0; j < xsxml_object->node[i]->number_of_contents; j++)
            {
                /* Read the jth content length. */
                dummy_value = read( file_descriptor, 
                                    &string_len, 
                                    sizeof(UINT_FIXED_BYTES));

                /* The plus one (+1) is for the null terminator. */
                xsxml_object->node[i]->content[j] = (char *) malloc(string_len + 1);

                xsxml_object->node[i]->content[j] [string_len] = 0;

                /* The plus one (+1) is for the null terminator. */
                char temp_string_2 [string_len + 1];

                /* Read the jth content. */
                dummy_value = read(file_descriptor, temp_string_2, string_len);

                memcpy( &xsxml_object->node[i]->content[j][0], 
                        &temp_string_2[0], 
                        string_len);
            }

            if (xsxml_object->node[i]->number_of_attributes == 0)
            {
                xsxml_object->node[i]->attribute_name  = 
                (char **) malloc(sizeof(char *));

                xsxml_object->node[i]->attribute_value = 
                (char **) malloc(sizeof(char *));
            }
            else
            {
                xsxml_object->node[i]->attribute_name  = 
                (char **) malloc(xsxml_object->node[i]->number_of_attributes * sizeof(char *));

                xsxml_object->node[i]->attribute_value = 
                (char **) malloc(xsxml_object->node[i]->number_of_attributes * sizeof(char *));
            }

            for (unsigned int j = 0; j < xsxml_object->node[i]->number_of_attributes; j++)
            {
                /* Read the jth attribute name length. */
                dummy_value = read( file_descriptor, 
                                    &string_len, 
                                    sizeof(UINT_FIXED_BYTES));

                /* The plus one (+1) is for the null terminator. */
                xsxml_object->node[i]->attribute_name[j] = (char *) malloc(string_len + 1);

                xsxml_object->node[i]->attribute_name[j] [string_len] = 0;

                /* The plus one (+1) is for the null terminator. */
                char temp_string_3 [string_len + 1];

                /* Read the jth attribute name. */
                dummy_value = read(file_descriptor, temp_string_3, string_len);

                memcpy( &xsxml_object->node[i]->attribute_name[j][0], 
                        &temp_string_3[0], 
                        string_len);

                /* Read the jth attribute value length. */
                dummy_value = read( file_descriptor, 
                                    &string_len, 
                                    sizeof(UINT_FIXED_BYTES));

                /* The plus one (+1) is for the null terminator. */
                xsxml_object->node[i]->attribute_value[j] = (char *) malloc(string_len + 1);

                xsxml_object->node[i]->attribute_value[j] [string_len] = 0;

                /* The plus one (+1) is for the null terminator. */
                char temp_string_4 [string_len + 1];

                /* Read the jth attribute name. */
                dummy_value = read(file_descriptor, temp_string_4, string_len);

                memcpy( &xsxml_object->node[i]->attribute_value[j][0], 
                        &temp_string_4[0], 
                        string_len);
            }
        }


        for (unsigned int i = 0; i < xsxml_object->number_of_nodes; i++)
        {
            UINT_FIXED_BYTES node_number;

            /* Read the ancestor nodes. */
                dummy_value = read( file_descriptor, 
                                    &node_number, 
                                    sizeof(UINT_FIXED_BYTES));

                if (node_number == 0)
                    xsxml_object->node[i]->ancestor = NULL;
                else
                    xsxml_object->node[i]->ancestor = xsxml_object->node[node_number - 1];

            /* Read the descendant nodes. */
                dummy_value = read( file_descriptor, 
                                    &node_number, 
                                    sizeof(UINT_FIXED_BYTES));

                if (node_number == 0)
                    xsxml_object->node[i]->descendant = NULL;
                else
                    xsxml_object->node[i]->descendant = xsxml_object->node[node_number - 1];

            /* Read the previous sibling nodes. */
                dummy_value = read( file_descriptor, 
                                    &node_number, 
                                    sizeof(UINT_FIXED_BYTES));

                if (node_number == 0)
                    xsxml_object->node[i]->previous_sibling = NULL;
                else
                    xsxml_object->node[i]->previous_sibling = xsxml_object->node[node_number - 1];

            /* Read the next sibling nodes. */
                dummy_value = read( file_descriptor, 
                                    &node_number, 
                                    sizeof(UINT_FIXED_BYTES));

                if (node_number == 0)
                    xsxml_object->node[i]->next_sibling = NULL;
                else
                    xsxml_object->node[i]->next_sibling = xsxml_object->node[node_number - 1];
        }


        close(file_descriptor);
    /* Main operation ends here. */


    if (dummy_value) dummy_value = 0;


    return xsxml_object;
}

