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
#include <stdlib.h>

#include "xsxml.h"


/*
To compile:
gcc -O3 -Wall -std=c17 -Wextra -Wpedantic -pedantic-errors xsxml.c test_3.c -o test_3 -lm

To run:
./test_3

Output:
Result Message : The XSXML object's information has been successfully compiled into file 'save_data.xml'.


Resultant data from file 'save_data.xml'
----------------------------------------------------------------------

<?xml version="1.0" encoding="UTF-8"?>

<Human_Records Number_of_Humans="2">
    <Human>
        <Name>
            Pac Man
        </Name>
        <Name>
            000-111-222
        </Name>
        <Phone>
            pac.man@human.org
        </Phone>
    </Human>
    <Human>
        <Phone>
            Polo Man
        </Phone>
        <Email>
            333-444-555
        </Email>
        <Email>
            polo.man@human.org
        </Email>
    </Human>
</Human_Records>
*/


int main()
{
    /* The below code is just a test code for debugging purposes. */
    /* setvbuf(stdout, NULL, _IONBF, 0);                          */

    /* Creating the required nodes, and assigning data to it.    */
    /* Note that the Xsxml_Nodes member 'depth' is not required. */

    Xsxml_Nodes node_0;
    Xsxml_Nodes node_0_1;
    Xsxml_Nodes node_0_2;
    Xsxml_Nodes node_0_1_1;
    Xsxml_Nodes node_0_1_2;
    Xsxml_Nodes node_0_1_3;
    Xsxml_Nodes node_0_2_1;
    Xsxml_Nodes node_0_2_2;
    Xsxml_Nodes node_0_2_3;

    node_0.node_name     = "Human_Records";
    node_0_1.node_name   = "Human";
    node_0_2.node_name   = "Human";
    node_0_1_1.node_name = "Name";
    node_0_1_2.node_name = "Name";
    node_0_1_3.node_name = "Phone";
    node_0_2_1.node_name = "Phone";
    node_0_2_2.node_name = "Email";
    node_0_2_3.node_name = "Email";

    node_0.number_of_attributes     = 1;
    node_0_1.number_of_attributes   = 0;
    node_0_2.number_of_attributes   = 0;
    node_0_1_1.number_of_attributes = 0;
    node_0_1_2.number_of_attributes = 0;
    node_0_1_3.number_of_attributes = 0;
    node_0_2_1.number_of_attributes = 0;
    node_0_2_2.number_of_attributes = 0;
    node_0_2_3.number_of_attributes = 0;

    node_0.attribute_name     = (char **) malloc(sizeof(char *));
    node_0.attribute_value    = (char **) malloc(sizeof(char *));

    node_0.attribute_name[0]  = "Number_of_Humans";
    node_0.attribute_value[0] = "2";

    node_0.number_of_contents     = 0;
    node_0_1.number_of_contents   = 0;
    node_0_2.number_of_contents   = 0;
    node_0_1_1.number_of_contents = 1;
    node_0_1_2.number_of_contents = 1;
    node_0_1_3.number_of_contents = 1;
    node_0_2_1.number_of_contents = 1;
    node_0_2_2.number_of_contents = 1;
    node_0_2_3.number_of_contents = 1;

    node_0_1_1.content    = (char **) malloc(sizeof(char *));
    node_0_1_2.content    = (char **) malloc(sizeof(char *));
    node_0_1_3.content    = (char **) malloc(sizeof(char *));
    node_0_2_1.content    = (char **) malloc(sizeof(char *));
    node_0_2_2.content    = (char **) malloc(sizeof(char *));
    node_0_2_3.content    = (char **) malloc(sizeof(char *));

    node_0_1_1.content[0] = "Pac Man";
    node_0_1_2.content[0] = "000-111-222";
    node_0_1_3.content[0] = "pac.man@human.org";
    node_0_2_1.content[0] = "Polo Man";
    node_0_2_2.content[0] = "333-444-555";
    node_0_2_3.content[0] = "polo.man@human.org";

    node_0.ancestor     = NULL;
    node_0_1.ancestor   = &node_0;
    node_0_2.ancestor   = &node_0;
    node_0_1_1.ancestor = &node_0_1;
    node_0_1_2.ancestor = &node_0_1;
    node_0_1_3.ancestor = &node_0_1;
    node_0_2_1.ancestor = &node_0_2;
    node_0_2_2.ancestor = &node_0_2;
    node_0_2_3.ancestor = &node_0_2;

    node_0.descendant     = &node_0_1;
    node_0_1.descendant   = &node_0_1_1;
    node_0_2.descendant   = &node_0_2_1;
    node_0_1_1.descendant = NULL;
    node_0_1_2.descendant = NULL;
    node_0_1_3.descendant = NULL;
    node_0_2_1.descendant = NULL;
    node_0_2_2.descendant = NULL;
    node_0_2_3.descendant = NULL;

    node_0.next_sibling     = NULL;
    node_0_1.next_sibling   = &node_0_2;
    node_0_2.next_sibling   = NULL;
    node_0_1_1.next_sibling = &node_0_1_2;
    node_0_1_2.next_sibling = &node_0_1_3;
    node_0_1_3.next_sibling = NULL;
    node_0_2_1.next_sibling = &node_0_2_2;
    node_0_2_2.next_sibling = &node_0_2_3;
    node_0_2_3.next_sibling = NULL;

    node_0.previous_sibling     = NULL;
    node_0_1.previous_sibling   = NULL;
    node_0_2.previous_sibling   = &node_0_1;
    node_0_1_1.previous_sibling = NULL;
    node_0_1_2.previous_sibling = &node_0_1_1;
    node_0_1_3.previous_sibling = &node_0_1_2;
    node_0_2_1.previous_sibling = NULL;
    node_0_2_2.previous_sibling = &node_0_2_1;
    node_0_2_3.previous_sibling = &node_0_2_2;


    /* Creating the Xsxml object, and attaching the outermost node to it. */
    /* Note that the other nodes are not attached to the Xsxml object.    */

    Xsxml xml_data;

    xml_data.node    = (Xsxml_Nodes **) malloc(sizeof(Xsxml_Nodes *));

    xml_data.node[0] = &node_0;


    /* Compiling the nodes data. */

    const char *xml_file_name = "save_data.xml";

    xsxml_compile( &xml_data, 
                   NULL, 
                   xml_file_name, 
                   4, 
                   0);


    /* Outputting the result message and the compiled data. */

    printf("\n\nResult Message : %s\n\n\n", xml_data.result_message);

    if (xml_data.result == XSXML_RESULT_SUCCESS)
    {
        printf("Resultant data from file '%s'\n", xml_file_name);

        printf( "%s%s\n\n", 
                "----------------------------------------", 
                "------------------------------");

        FILE *file_pointer = fopen(xml_file_name, "r");

        char character;

        while ((character = getc(file_pointer)) != EOF)
        {
            putchar(character);
        }

        fclose(file_pointer);
    }

    printf("\n");

    free(node_0.attribute_name);
    free(node_0.attribute_value);

    free(node_0_1_1.content);
    free(node_0_1_2.content);
    free(node_0_1_3.content);
    free(node_0_2_1.content);
    free(node_0_2_2.content);
    free(node_0_2_3.content);

    return 0;
}

