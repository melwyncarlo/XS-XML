# XS-XML : An extra-small XML parser.

<br>

| Sr. No. | File name | File Description |
| --- | --- | --- |
| 1. | xsxml.c | The XS-XML source code file |
| 2. | xsxml.h | The XS-XML header  file     |
| 3. | test_1.c | The RAM  mode test file     |
| 4. | test_2.c | The FILE mode test file     |
| 5. | test_data.xml | The test data XML  file |
<br>


## Description
**Xsxml** is a tiny, basic, single-file XML parser.


Xsxml stands for *'Extra small XML'*.

It works fine with simple XML files written as per 
the standards of *version 1.0* and encoding of *UTF-8*.

It does not cater to the following features:
* XML schemas and DTDs
* Name prefixes
* XML namespaces (the xmlns attribute) 

Though it can parse `CDATA` [1] and character entity references (CER [2]).

Also note that all alphabetic input and data are case-sensitive.


<br>[1] CDATA are used for storing illegal characters like '<' and '&' : `<![CDATA[<&]]>`<br><br>
[2] CERs are used to repace ASCII or Unicode (UTF-8) characters.<br>
For example, consider the less-than (<) character:<br>
- ASCII mode        :  `&lt;`
- Decimal mode      :  `&#60;`
- Hexa-decimal mode :  `&#x3C;`


<br>To know more about the functions (their input names and types, and their output types), 
and to know about the library's structures and enumerations, refer to the 
**xsxml.h** file.


## <br>Modes
The XS-XML parser works in two modes:
1. RAM  mode
2. FILE mode


The RAM and FILE mode examples have been provided in the test files 1 and 2 respectively.


### <br>RAM mode

In RAM mode, data is stored on the RAM as a cluster of pointers, 
pointing either to data or to other pointers within the cluster.

A parse operation requires the **input** of the XML file path (not just file name).

A parse operation **returns** an Xsxml object which contains the following members:
1. The parse operation's result code
2. The parse operation's result message
3. The number of tag elements (nodes)
4. An array of tag element (node) objects

A tag element (node) object contains the following members:
1. A pointer to this node's ancestor node,         if it exists, or a `NULL` pointer
2. A pointer to this node's descendant node,       if it exists, or a `NULL` pointer
3. A pointer to this node's next sibling node,     if it exists, or a `NULL` pointer
4. A pointer to this node's previous sibling node, if it exists, or a `NULL` pointer
5. The hierarchical level (depth) of this node
6. The number of distinct `PCDATA` contents within this node [3]
7. The number of attributes associated with this node
8.  The tag element (node) name (label)
9.  An array of `PCDATA` contents within this node [3]
10. An array of attribute names (labels) associated with this node
11. An array of attribute values associated with this node


[3] PCDATA contents separated by other tag elements (nodes) within a given node
    are considered as different (distinct) contents within the same node.


### <br>FILE mode

In FILE mode, data is stored in the file system as a cluster of hidden files.
The storage is similar as in RAM mode, where each data is stored in a unique 
file, bearing a unique file name. 

In case of pointers to other nodes, as in ancestor and descendant nodes and so on, 
there are files for that too; here, the file would store the node number of the 
node to which it is pointed. Because all the files in the cluster share a common 
prefix name, it makes it easier to locate the files based on node numbers.

In FILE mode, a parse operation requires the **input** of the XML file path 
(not just file name), as well as a temporary directory path (optional).
If the temporary directory path is not needed, then input a `NULL` pointer, 
or an empty string `""` instead; this will cause the parse operation to use 
the same directory as the XML file.

The **occurrence** function provides with an array of node indices that match 
the input description of tag name, attribute name, attribute value, and 
`PCDATA` content. If you wish to skip an input, simply provide a `NULL` pointer instead.
Another input 'direction' states whether the search be from top to bottom 
(`XSXML_DIRECTION_FORWARD`) or from bottom to top (`XSXML_DIRECTION_BACKWARD`).
The first element of the resultant array contains the number of resultant nodes.

A FILE mode parse operation **returns** an Xsxml_Files object 
which contains the following members:
1. The parse operation's result code
2. The parse operation's result message
3. The number of tag elements (nodes)
4. The temporary directory path
5. The common file name prefix

The FILE mode, too, contains the **occurrence** function that takes in the 
same input and provides the same output, although, their inner workings are 
different.

Furthermore, the FILE mode also contains the **property** function, which, 
upon providing it with a node index, a property name, and optionally, 
a property index (only if more than one exists), retrieves the corresponding 
node information.


## <br>Modes (cont'd)

For both the RAM and FILE modes, there are **unset** functions, 
which are important to execute after making use of the extracted XML information.

In RAM mode, it frees memory space at the addresses as pointed by the node pointers.
If the program exits immediately after working on the extracted XML information, 
then, by default, most operating systems (OS) are capable to freeing the memory 
space themselves. The problem arises if the program does not exit for a very long time.
Regardless, it is a good practice to utilise the 'unset' function.

In FILE mode, the 'unset' function is especially important, as operating systems (OS)
cannot delete files in the file system by themselves when the program exits.


I am aware of the **FILE \*tmpfile(void)** function provided by the **stdio.h** library.<br>
I was, however, reluctant to use it for two reasons:

1. The file deletes itself upon closing the file, and a program cannot simultaneously 
   keep open multiple files.
2. In case of debugging, it makes it easier to recognise a cluster of files that 
   share a similar name.<br>But, in this case, each temporary file name is random.
