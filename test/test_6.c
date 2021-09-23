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
NOTE:
The original XML file size is 982 bytes.
The 32-bit unsigned integer-based binary file size is 641 bytes (34.73% reduction).
The 16-bit unsigned integer-based binary file size is 405 bytes (58.76% reduction).
The  8-bit unsigned integer-based binary file size is 287 bytes (70.77% reduction).

To compile:
gcc -O3 -Wall -std=c17 -Wextra -Wpedantic -pedantic-errors xsxml.c xsxml_binary_object.c test_6.c -o test_6 -lm

To run:
./test_6

Output:
Result Message : The XSXML object's information has been successfully compiled 
into the binary file 'xml_data.xsxml'.
*/


int main()
{
    /* The below code is just a test code for debugging purposes. */
    /* setvbuf(stdout, NULL, _IONBF, 0);                          */

    const char *xml_file_name = "test_data.xml";

    Xsxml *xml_data = xsxml_parse(xml_file_name);

    create_xsxml_binary_object( xml_data, 
                                NULL, 
                                "xml_data");

    printf("\n\nResult Message : %s\n\n", xml_data->result_message);

    printf("\n\n");

    xsxml_unset(&xml_data);

    return 0;
}

