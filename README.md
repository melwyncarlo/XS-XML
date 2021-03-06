# XS-XML : An extra-small XML parser and compiler.


## <br>Description
**XS-XML is a tiny, basic, single-file, and portable XML parser and compiler.**<br><br>


**||--- A NEW feature has been added in XS-XML version 1.2 ---||**<br><br>
**||--- XS-XML is now a file extension, too (\*.xsmxl). ---||**<br><br>
**||--- XS-XML can now compile and utilise it's very own XML processed binary files. ---||**<br><br>


XS-XML stands for *'Extra small XML'*. It works fine with simple XML files written as per 
the standards of *version 1.0* and encoding of *UTF-8*.

Though it can parse [CDATA](https://en.wikipedia.org/wiki/CDATA) and character entity references ([CERs](https://en.wikipedia.org/wiki/List_of_XML_and_HTML_character_entity_references)), it does not cater to the following features:
* XML schemas and DTDs
* Name prefixes
* XML namespaces (the xmlns attribute) 


<br>For a quick illustration of the library’s functions (their input names and types, and their output types), and their structures, enumerations and variables, refer to the [**xsxml.h**](https://github.com/melwyncarlo/XS-XML/blob/main/src/xsxml.h) file.

<br>To read more about the XS-XML library, refer to its [**Documentation Manual**](https://github.com/melwyncarlo/XS-XML/blob/main/Documentation_Manual.pdf).



## <br>Features
* XML file parser and compiler
* Single-file and portable library
* Extracts information in a meaningful manner
* Extracts information in the form of inter-connected nodes
* Perform a basic validation check and returns the result
* Apt for basic, everyday, and non-sophisticated use
* Can search through the extracted data based on given input
* Works with unicode characters (UTF-8)
* Works with CDATA and character entry references (CER)
* Works with both empty (`<tag/>`) and non-empty (`<tag>some data</tag>`) tags
* Works in RAM mode using arrays, pointers and objects
* Works in FILE mode for large XML data files
* The FILE mode relies on the computer file system
* The FILE mode utilises negligible RAM space
* Written in two programming languages: C and JavaScript (JS)
* **NEW FEATURE ADDED:** <br>Create processed XS-XML binary object files that act as memory resident databases (Linux and POSIX only).<br>The created XS-XML binary object file would be smaller in file size than its original XML file (between one-third and two-thirds). The \*.xsxml files upload and compile data a lot quicker, given their off-the-shelf format.

<br>**NOTE:** The FILE mode only works in C, and not in JavaScript (JS), for obvious reasons.


<br>


![This is the description image.](https://github.com/melwyncarlo/XS-XML/blob/main/Description_Image.png)



## <br>Files list


The following are a list of files present within the XS-XML library's GitHub repository.
<br><br>

| Sr. No. | File name | File Description |
| --- | --- | --- |
| 1. | xsxml.c | The XS-XML C source code file |
| 2. | xsxml.h | The XS-XML C header  file     |
| 3. | xsxml.js | The XS-XML JavaScript source code file |
| 4. | xsxml_binary_object.c **[NEW]** | The XS-XML binary object C source code file |
| 5. | xsxml_binary_object.h **[NEW]** | The XS-XML binary object C header  file     |
| 6. | test_1.c | The RAM  mode XML parse example test file in C |
| 7. | test_2.c | The FILE mode XML parse example test file in C |
| 8. | test_3.c | The RAM mode XML compile example test file in C |
| 9. | test_4.html | The XML parse example test file in JavaScript |
| 10. | test_5.html | The XML compile example test file in JavaScript |
| 11. | test_6.c **[NEW]** | Create a processed XML binary object file |
| 12. | test_7.c **[NEW]** | Test the processed XML binary object file |
| 13. | test_data.xml | The example test XML data file |
| 14. | Documentation_Manual.pdf | The Documentation Manual PDF file |
<br>


## <br>Motivation

I created this library as a necessity for my other projects. I could not find any viable, single-file, basic XML-parser (one that not just extracts the tag elements, but also connects them in a meaningful way), and so, I decided to create one myself. While utilising this library for my projects, and thereby testing it, I came across a few minor run-time errors (mostly related to segmentation faults due to faulty array memory allocations), which I had then immediately corrected.

<br>Hope you find this library useful.
