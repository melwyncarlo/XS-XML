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


#include <stdio.h>

#include "xsxml_binary_object.h"


/*
To compile:
gcc -O3 -Wall -std=c17 -Wextra -Wpedantic -pedantic-errors xsxml.c xsxml_binary_object.c test_7.c -o test_7 -lm

To run:
./test_7

Output:
The Xsxml *xml_data structure pointer has been created.

Number of nodes : 13

Outermost node name : Collection

Outermost node's descendant's node name : Random_Tag

Outermost node's descendant's content : qwerty[a&)
*/


int main()
{
    /* The below code is just a test code for debugging purposes. */
    /* setvbuf(stdout, NULL, _IONBF, 0);                          */

    Xsxml *xml_data = decode_xsxml_binary_object("xml_data.xsxml");

    printf("\n\nThe Xsxml *xml_data structure pointer has been created.\n\n");

    printf("Number of nodes : %d\n\n", xml_data->number_of_nodes);

    printf("Outermost node name : %s\n\n", xml_data->node[0]->node_name);

    printf( "Outermost node's descendant's node name : %s\n\n", 
            xml_data->node[0]->descendant->node_name);

    printf( "Outermost node's descendant's content : %s\n\n", 
            xml_data->node[0]->descendant->content[0]);

    xsxml_unset(&xml_data);

    return 0;
}

