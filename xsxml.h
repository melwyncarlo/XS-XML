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


#ifndef XSXML_H
#define XSXML_H


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


typedef enum Xsxml_Property
{
    XSXML_PROPERTY_NONE = -1, 

    XSXML_PROPERTY_NODE_NAME

} Xsxml_Property;

typedef enum Xsxml_Result
{
    XSXML_RESULT_SUCCESS      =  1, 
    XSXML_RESULT_FILE_FAILURE = -1, 
    XSXML_RESULT_XML_FAILURE  = -2

} Xsxml_Result;


typedef enum Xsxml_Direction
{
   XSXML_DIRECTION_FORWARD  =  1, 
   XSXML_DIRECTION_BACKWARD = -1

} Xsxml_Direction;


typedef struct Xsxml Xsxml;
typedef struct Xsxml_Nodes Xsxml_Nodes;


typedef struct Xsxml_Nodes
{
    Xsxml_Nodes *ancestor;
    Xsxml_Nodes *descendant;
    Xsxml_Nodes *next_sibling;
    Xsxml_Nodes *previous_sibling;

    unsigned int depth;
    unsigned int number_of_contents;
    unsigned int number_of_attributes;

    char  *node_name;
    char **content;
    char **attribute_name;
    char **attribute_value;

} Xsxml_Nodes;


typedef struct Xsxml
{
    Xsxml_Result result;
    char *result_message;

    unsigned int number_of_nodes;

    Xsxml_Nodes **node;

} Xsxml;


typedef struct Xsxml_Files
{
    Xsxml_Result result;
    char *result_message;

    unsigned int number_of_nodes;

    char *node_directory_path;
    char *node_file_name;

} Xsxml_Files;


/* RAM mode functions */
extern Xsxml *xsxml_parse(const char *input_file_path);

size_t *xsxml_occurrence( Xsxml *xsxml_object, 
                          char *tag_name, 
                          char *attribute_name, 
                          char *attribute_value, 
                          char *content, 
                          Xsxml_Direction direction);

extern void xsxml_unset(Xsxml **xsxml_object);


/* FILE mode functions */
extern Xsxml_Files *xsxml_files_parse( const char *input_file_path, 
                                       const char *temporary_directory_path);

char *xsxml_files_property( Xsxml_Files *xsxml_files_object, 
                            size_t node_index, 
                            char *property_name, 
                            size_t property_index);

size_t *xsxml_files_occurrence( Xsxml_Files *xsxml_files_object, 
                                char *tag_name, 
                                char *attribute_name, 
                                char *attribute_value, 
                                char *content, 
                                Xsxml_Direction direction);

extern void xsxml_files_unset(Xsxml_Files **xsxml_files_object);


#endif /* XSXML_H */

