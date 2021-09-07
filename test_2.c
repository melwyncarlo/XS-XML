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

#include "xsxml.h"


/*
To compile:
gcc -O3 -Wall -std=c17 -Wextra -Wpedantic -pedantic-errors xsxml.c test_1.c -o test_1 -lm

To run:
./test_1
*/


int main()
{
    setvbuf(stdout, NULL, _IONBF, 0);

    const char *xml_file_name = "test_data.xml";

    Xsxml_Files *xml_data = xsxml_files_parse(xml_file_name, "");

    printf("\n\nR = %s\n\n", xml_data->result_message);

    size_t *o = xsxml_files_occurrence(xml_data, NULL, NULL, NULL, "123456", XSXML_DIRECTION_FORWARD);

    printf("\n\n");

    printf("%lu\n", o[0]);
    printf("%lu\n", o[1]);
    printf("%lu\n", o[o[0]]);

    char *some_data = xsxml_files_property(xml_data, 0, XSXML_PROPERTY_NODE_NAME, XSXML_PROPERTY_NONE);

    printf("\n\nData = %s\n\n", some_data);

    xsxml_files_unset(&xml_data);

    return 0;
}

