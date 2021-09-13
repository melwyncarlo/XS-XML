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


const MAX_INDENTATION                      = 10;

const MAX_VERTICAL_SPACING                 = 10;

const NODE_FILE_NAME_SIZE                  = 10;

const RESULT_MESSAGE_MAX_LENGTH            = 200;

const MAX_UTF_8_CHARACTER_VALUE            = 1114112;

const NUMBER_OF_ALPHABETIC_CHARACTERS      = 26;

const LOWER_CASE_LETTER_A_ASCII_VALUE      = 97;

const MAX_ATTRIBUTES_POSSIBLE_N            = 99999;

const MAX_ATTRIBUTES_POSSIBLE_N_DIGITS     = 5;

const CHARACTER_ENTRY_REFERENCE_MAX_LENGTH = 10;

const GIBBERISH_STRING = "r3WQKJy0oxCBo021a8hUz2ZBcHDW264bHOwXECE5";


const Xsxml_Parse_Mode = 
{
    XSXML_TAG             : 0, 
    XSXML_ATTRIBUTE_NAME  : 1, 
    XSXML_ATTRIBUTE_VALUE : 2, 
    XSXML_PCDATA_CONTENT  : 3
};


const Xsxml_Result = 
{
    XSXML_RESULT_SUCCESS      :  1, 
    XSXML_RESULT_FILE_FAILURE : -1, 
    XSXML_RESULT_XML_FAILURE  : -2
};


const Xsxml_Direction = 
{
   XSXML_DIRECTION_FORWARD  :  1, 
   XSXML_DIRECTION_BACKWARD : -1
};


function Xsxml_Nodes()
{
    this.ancestor             = null;
    this.descendant           = null;
    this.next_sibling         = null;
    this.previous_sibling     = null;

    this.depth                = 0;
    this.number_of_contents   = 0;
    this.number_of_attributes = 0;

    this.node_name            = "";
    this.content              = [];
    this.attribute_name       = [];
    this.attribute_value      = [];
};


function Xsxml()
{
    this.result          = Xsxml_Result.XSXML_RESULT_SUCCESS;
    this.result_message  = '';

    this.number_of_nodes = 0;

    this.node            = [];
};


function append_character_to_word(word_object, character)
{
    word_object.word_len++;

    word_object.word += character;
}


function reset_word(word_object)
{
    word_object.word = [];

    word_object.word_len = 0;
}

function parse_cer(word_object)
{
    const BASE_ENTITY_REFERENCES = 
    [
        [   'lt', 
            'gt', 
            'amp', 
            'apos', 
            'quot'
        ], 

        [   '<', 
            '>', 
            '&', 
            '\'', 
            '\"'
        ]
    ];

    for (var i = 0; i < 5; i++)
    {
        if (word_object.character_entry_reference.localeCompare(BASE_ENTITY_REFERENCES[0][i]) == 0)
        {
            word_object.character_entry_reference = BASE_ENTITY_REFERENCES[1][i];
            return 1;
        }
    }

    if (word_object.character_entry_reference.charAt(0) != '#')
    {
        return 0;
    }

    var i = 1;

    var cer_is_hex = 0;

    if (word_object.character_entry_reference.charAt(1) == 'x' 
    ||  word_object.character_entry_reference.charAt(1) == 'X')
    {
        i = 2;
        cer_is_hex = 1;
    }

    while (word_object.character_entry_reference.charAt(i) == '0')
    {
        i++;
    }

    var decimal_value = 0;

    /* Convert the hexa-decimal value to its decimal form. */
    if (cer_is_hex)
    {
        var power_count = 0;

        for (var j = word_object.character_entry_reference.length - 1; j >= i ; j--)
        {
            var temp_val;

            if (word_object.character_entry_reference.charAt(j) >= '0' 
            &&  word_object.character_entry_reference.charAt(j) <= '9')
            {
                temp_val = word_object.character_entry_reference.charCodeAt(j) - '0'.charCodeAt(0);
            }
            else
            {
                if (word_object.character_entry_reference.toLowerCase().charCodeAt(j) > 'f'.charCodeAt(0))
                {
                    return 0;
                }

                temp_val = word_object.character_entry_reference.toLowerCase().charCodeAt(j) - 'a'.charCodeAt(0) + 10;
            }

            decimal_value += temp_val * Math.pow(16 , power_count);

            power_count++;
        }
    }
    else
    {
        word_object.character_entry_reference = word_object.character_entry_reference.substring(1);

        decimal_value = parseInt(word_object.character_entry_reference);
    }

    /* Converting decimal value to UTF-8. */
    if (decimal_value >= MAX_UTF_8_CHARACTER_VALUE)
    {
        return 0;
    }

    word_object.character_entry_reference = String.fromCharCode(decimal_value);

    return 1;
}


function xsxml_unset_node(xsxml_node)
{
    let sub_node = xsxml_node.descendant;

    if (sub_node != null && sub_node != undefined)
    {
        xsxml_unset_node(sub_node);

        delete xsxml_node.ancestor;
        delete xsxml_node.descendant;
        delete xsxml_node.next_sibling;
        delete xsxml_node.previous_sibling;

        delete xsxml_node.depth;
        delete xsxml_node.number_of_contents;
        delete xsxml_node.number_of_attributes;

        delete xsxml_node.node_name;
        delete xsxml_node.content;
        delete xsxml_node.attribute_name;
        delete xsxml_node.attribute_value;

        sub_node = null;

        sub_node = sub_node.next_sibling;
    }
}


function xsxml_unset(xsxml_object)
{
    xsxml_object.result          = null;
    xsxml_object.result_message  = null;
    xsxml_object.number_of_nodes = null;

    delete xsxml_object.result;
    delete xsxml_object.result_message;

    for (let i = 0; i < xsxml_object.number_of_nodes; i++)
    {
        xsxml_unset_node(xsxml_object.node[i]);
    }

    delete xsxml_object.number_of_nodes;
    delete xsxml_object.node;

    xsxml_object = null;
}


function parse_sub_operation_ram_mode(xsxml_object, parse_mode, node_level, data)
{
    const DATA_LEN = data.length;

    const n = xsxml_object.number_of_nodes;

    if (parse_mode == Xsxml_Parse_Mode.XSXML_TAG)
    {
        if (node_level == 0)
        {
            if (xsxml_object.number_of_nodes > 0)
            {
                return 0;
            }
        }

        xsxml_object.number_of_nodes++;

        var new_xsxml_node = new Xsxml_Nodes();

        new_xsxml_node.node_name = data;

        new_xsxml_node.depth                = node_level;
        new_xsxml_node.number_of_contents   = 0;
        new_xsxml_node.number_of_attributes = 0;

        if (node_level != 0)
        {
            if (xsxml_object.node[n-1].depth == node_level)
            {
                new_xsxml_node.previous_sibling     = xsxml_object.node[n-1];
                xsxml_object.node[n-1].next_sibling = new_xsxml_node;

                new_xsxml_node.ancestor = xsxml_object.node[n-1].ancestor;
            }
            else
            {
                new_xsxml_node.ancestor           = xsxml_object.node[n-1];
                xsxml_object.node[n-1].descendant = new_xsxml_node;

                for (var i = n - 1; i >= 0; i--)
                {
                    if (xsxml_object.node[i].depth == node_level)
                    {
                        new_xsxml_node.previous_sibling   = xsxml_object.node[i];
                        xsxml_object.node[i].next_sibling = new_xsxml_node;
                        break;
                    }
                }
            }
        }

        xsxml_object.node.push(new_xsxml_node);
    }
    else if (parse_mode == Xsxml_Parse_Mode.XSXML_ATTRIBUTE_NAME)
    {
        const n_a = xsxml_object.node[n-1].number_of_attributes;

        for (var i = 0; i < n_a; i++)
        {
            if (data.localeCompare(xsxml_object.node[n-1].attribute_name[i]) == 0)
            {
                return 0;
            }
        }

        xsxml_object.node[n-1].number_of_attributes++;

        xsxml_object.node[n-1].attribute_name.push(data);
    }
    else if (parse_mode == Xsxml_Parse_Mode.XSXML_ATTRIBUTE_VALUE)
    {
        xsxml_object.node[n-1].attribute_value.push(data);
    }
    else if (parse_mode == Xsxml_Parse_Mode.XSXML_PCDATA_CONTENT)
    {
        xsxml_object.node[n-1].number_of_contents++;

        xsxml_object.node[n-1].content.push(data);
    }

    return 1;
}


async function chunk_read(file_obj, index) {
    let myPromise = new Promise(function(myResolve, myReject) {
        var file_reader = new FileReader();
        var file_chunk = file_obj.slice(index, index + 1);
        file_reader.onload = function(e) {
            text_chunk = file_reader.result;
            myResolve(text_chunk);
        };
        file_reader.onerror = function(e) {
            myResolve(GIBBERISH_STRING);
        };
        file_reader.readAsText(file_chunk);
    });

    return await myPromise;
}


async function xsxml_parse(input_file)
{
    var xsxml_object = new Xsxml();

    if (  input_file.size  == null || input_file.size  == undefined 
    ||  ((input_file.name  == null || input_file.name  == undefined) 
    &&   (input_file.value == null || input_file.value == undefined)))
    {
        xsxml_object.result = Xsxml_Result.XSXML_RESULT_FILE_FAILURE;

        xsxml_object.result_message = "An input file object has not been inputted.";

        return xsxml_object;
    }


    xsxml_object.number_of_nodes = 0;


    var XML_X                   = 0;    /* X */
    var XML_M                   = 0;    /* M */
    var XML_EQUAL               = 0;    /* = */
    var XML_HYPHEN              = 0;    /* - */
    var XML_QUESTION            = 0;    /* ? */
    var XML_AMPERSAND           = 0;    /* & */
    var XML_WHITESPACE          = 0;    /*   */
    var XML_SINGLE_QUOTE        = 0;    /* - */
    var XML_DOUBLE_QUOTE        = 0;    /* - */
    var XML_FORWARD_SLASH_END   = 0;    /* / */
    var XML_FORWARD_SLASH_START = 0;    /* / */
    var XML_END_SQUARE_BRACKET  = 0;    /* ] */

    var XML_HEADING             = 0;
    var XML_COMMENT_CONTENT     = 0;
    var XML_COMMENT_START_TAG   = 0;
    var XML_TAG                 = 0;
    var XML_ATTRIBUTE           = 0;
    var XML_ATTRIBUTE_NAME      = 0;
    var XML_ATTRIBUTE_VALUE     = 0;
    var XML_CDATA_CONTENT       = 0;
    var XML_CDATA_START_TAG     = 0;
    var XML_PCDATA_CONTENT      = 0;


    var cer_i      = 0;                 /* Character entry reference count */
    var word_len   = 0;                 /* Word characters count           */
    var node_level = 0;                 /* Current hierarchical depth      */


    var word_object = 
    {
        word : '', 
        word_len : 0, 
        character_entry_reference : '' 
    };


    for (var i = 0; i < input_file.size; i++)
    {
        var file_data_character = await chunk_read(input_file, i);

        if (file_data_character == GIBBERISH_STRING)
        {
            xsxml_object.result = Xsxml_Result.XSXML_RESULT_FILE_FAILURE;

            xsxml_object.result_message = "JavaScript was unable to read the file.";

            return xsxml_object;
        }


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
                    xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                    xsxml_object.result_message = 
                    "A double hyphen (--) within comments is not allowed.";

                    return xsxml_object;
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

            xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

            xsxml_object.result_message = 
            "Tag names must start with an alphabetical character [a-zA-Z] or underscore (_)";

            return xsxml_object;
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

            if (XML_END_SQUARE_BRACKET == 2 && file_data_character == '>')
            {
                /* Working on CDATA. */

                XML_CDATA_CONTENT      = 0;
                XML_END_SQUARE_BRACKET = 0;
                continue;
            }

            while (XML_END_SQUARE_BRACKET > 0)
            {
                append_character_to_word(word_object, ']');

                XML_END_SQUARE_BRACKET--;
            }

            append_character_to_word(word_object, file_data_character);

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

            xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

            xsxml_object.result_message = 
            "Tag names must start with an alphabetical character [a-zA-Z] or underscore (_)";

            return xsxml_object;
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

                if (word_object.word.substr(word_object.word_len - 1, 1) == ' ')
                    word_object.word = word_object.word.substr(0, word_object.word_len - 1);

                parse_sub_operation_ram_mode( xsxml_object, 
                                              Xsxml_Parse_Mode.XSXML_PCDATA_CONTENT, 
                                              node_level, 
                                              word_object.word);

                reset_word(word_object);

                XML_PCDATA_CONTENT = 0;
            }

            if (!XML_ATTRIBUTE && word_object.word_len == 0)
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

                if (!(/[a-zA-Z]/).test(file_data_character) && file_data_character != '_')
                {
                    xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                    xsxml_object.result_message = 
                    "Tag names must start with an alphabetical character [a-zA-Z] or underscore (_)";

                    return xsxml_object;
                }

                if ((file_data_character == 'x') || (file_data_character == 'X'))
                {
                    XML_X = 1;
                    append_character_to_word(word_object, file_data_character);

                    continue;
                }
            }

            if (XML_X)
            {
                if ((file_data_character == 'm') || (file_data_character == 'M'))
                {
                    XML_M = 1;
                    append_character_to_word(word_object, file_data_character);
                    continue;
                }
                XML_X = 0;
            }

            if (XML_M)
            {
                if ((file_data_character == 'l') || (file_data_character == 'L'))
                {
                    xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                    xsxml_object.result_message = 
                    "Tag names cannot start with 'xml' or any of its variants like XML, Xml, etc.";

                    return xsxml_object;
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
                    xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                    xsxml_object.result_message = 
                    "Attribute values may not contain the less-than (<) characters.";

                    return xsxml_object;
                }

                if (file_data_character == '&')
                {
                    if (!XML_AMPERSAND)
                    {
                        cer_i = 0;

                        word_object.character_entry_reference = '';

                        XML_AMPERSAND = 1;

                        continue;
                    }
                }

                if (file_data_character == ';')
                {
                    if (parse_cer(word_object))
                    {
                        append_character_to_word(word_object, word_object.character_entry_reference);
                    }
                    else
                    {
                        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                        xsxml_object.result_message = 
                        "Attribute values may contain the ampersand (&) characters only as character entity references.";

                        return xsxml_object;
                    }

                    XML_AMPERSAND = 0;
                    continue;
                }

                if (XML_AMPERSAND)
                {
                    if (cer_i == (CHARACTER_ENTRY_REFERENCE_MAX_LENGTH - 1))
                    {
                        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                        xsxml_object.result_message = 
                        "Attribute values may contain the ampersand (&) "  + 
                        "characters only as character entity references. " + 
                        CHARACTER_ENTRY_REFERENCE_MAX_LENGTH.toString()    + 
                        " number of characters.";

                        return xsxml_object;
                    }

                    word_object.character_entry_reference += file_data_character;
                    cer_i++;
                    continue;
                }

                if (((file_data_character == '\'') && XML_SINGLE_QUOTE) 
                ||  ((file_data_character == '\"') && XML_DOUBLE_QUOTE))
                {
                    if (XML_AMPERSAND)
                    {
                        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                        xsxml_object.result_message = 
                        "Attribute values may contain the ampersand (&) " + 
                        "characters only as character entity references.";

                        return xsxml_object;
                    }

                    /* Working on attribute value. */

                    parse_sub_operation_ram_mode( xsxml_object, 
                                                  Xsxml_Parse_Mode.XSXML_ATTRIBUTE_VALUE, 
                                                  node_level, 
                                                  word_object.word);

                    reset_word(word_object);

                    XML_ATTRIBUTE       = 1;
                    XML_ATTRIBUTE_NAME  = 0;
                    XML_ATTRIBUTE_VALUE = 0;
                    XML_SINGLE_QUOTE    = 0;
                    XML_DOUBLE_QUOTE    = 0;
                    XML_EQUAL           = 0;
                    continue;
                }

                append_character_to_word(word_object, file_data_character);
                continue;
            }


/* ########################################################################## */
/*                                 XML END TAG                                */
/* ########################################################################## */
            if (file_data_character == '>')
            {
                if (XML_ATTRIBUTE_NAME)
                {
                    xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                    xsxml_object.result_message = 
                    "If empty, then attribute values must atleast contain " + 
                    "the double quotes (\"\").";

                    return xsxml_object;
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
                    reset_word(word_object);
                    continue;
                }

                /* Working on tag. */

                if (!parse_sub_operation_ram_mode( xsxml_object, 
                                                   Xsxml_Parse_Mode.XSXML_TAG, 
                                                   node_level, 
                                                   word_object.word))
                {
                    xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                    xsxml_object.result_message = 
                    "There cannot be more than one outermost tag.";

                    return xsxml_object;
                }

                reset_word(word_object);

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
                if (file_data_character == ' ')
                {
                    continue;
                }

                if (XML_FORWARD_SLASH_START)
                {
                    xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                    xsxml_object.result_message = 
                    "End tags cannot have any attributes.";

                    return xsxml_object;
                }

                if (file_data_character == '=')
                {
                    if (!XML_ATTRIBUTE_NAME)
                    {
                        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                        xsxml_object.result_message = 
                        "Attribute names cannot be empty (i.e. contain a " + 
                        "lone equal-to sign (=).";

                        return xsxml_object;
                    }

                    /* Working on attribute name. */

                        if (!parse_sub_operation_ram_mode( xsxml_object, 
                                                           Xsxml_Parse_Mode.XSXML_ATTRIBUTE_NAME, 
                                                           node_level, 
                                                           word_object.word))
                        {
                            xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                            xsxml_object.result_message = 
                            "Within a given tag, attributes cannot share the same name.";

                            return xsxml_object;
                        }

                    reset_word(word_object);

                    if (file_data_character == '=')
                    {
                        XML_EQUAL = 1;
                    }

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
                        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                        xsxml_object.result_message = 
                        "Attribute assignment (=) must be followed either " + 
                        "by a single quote (') or a double quote (\").";

                        return xsxml_object;
                    }
                }

                if (file_data_character != '/')
                {
                    if (!XML_ATTRIBUTE_NAME)
                    {
                        if (!(/[a-zA-Z]/).test(file_data_character) && file_data_character != '_')
                        {
                            xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                            xsxml_object.result_message = 
                            "Attribute names must start with an alphabetical " + 
                            "character [a-zA-Z] or underscore (_)";

                            return xsxml_object;
                        }

                        XML_ATTRIBUTE_NAME = 1;
                    }

                    append_character_to_word(word_object, file_data_character);

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
                    xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                    xsxml_object.result_message = 
                    "A tag cannot have more than one forward slash.";

                    return xsxml_object;
                }

                XML_FORWARD_SLASH_END = 1;

                continue;
            }

            if (file_data_character == ' ')
            {
                if (word_object.word_len != 0)
                {
                    /* Working on tag. */

                    if (!parse_sub_operation_ram_mode( xsxml_object, 
                                                       Xsxml_Parse_Mode.XSXML_TAG, 
                                                       node_level, 
                                                       word_object.word))
                    {
                        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                        xsxml_object.result_message = 
                        "There cannot be more than one outermost tag.";

                        return xsxml_object;
                    }

                    reset_word(word_object);

                    XML_ATTRIBUTE      = 1;
                    XML_ATTRIBUTE_NAME = 0;
                }

                continue;
            }

            if (file_data_character == '=')
            {
                if (XML_ATTRIBUTE_VALUE)
                {
                    append_character_to_word(word_object, file_data_character);
                }

                if (XML_ATTRIBUTE)
                {
                    XML_ATTRIBUTE_VALUE = 1;
                }

                continue;
            }

            if (!(/[a-zA-Z0-9]/).test(file_data_character) 
            &&  file_data_character != '-' 
            &&  file_data_character != '_' 
            &&  file_data_character != '.')
            {
                xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                xsxml_object.result_message = 
                "Tag names may contain letters [a-zA-z], digits [0-9], " + 
                "hyphens (-), underscores (_), and periods (.) only.";

                return xsxml_object;
            }

            append_character_to_word(word_object, file_data_character);

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
                xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                xsxml_object.result_message = 
                "A tag's PCDATA may contain the ampersand (&) " + 
                "characters only as character entity references.";

                return xsxml_object;
            }

            if (!XML_PCDATA_CONTENT)
            {
                continue;
            }

            if (!XML_WHITESPACE)
            {
                XML_WHITESPACE = 1;
                append_character_to_word(word_object, ' ');
            }

            continue;
        }

        XML_WHITESPACE = 0;

        if (file_data_character == '<')
        {
            if (XML_AMPERSAND)
            {
                xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                xsxml_object.result_message = 
                "1A tag's PCDATA may contain the ampersand (&) " + 
                "characters only as character entity references.";

                return xsxml_object;
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

                word_object.character_entry_reference = '';

                XML_AMPERSAND = 1;

                continue;
            }
        }

        if (file_data_character == ';')
        {
            if (parse_cer(word_object))
            {
                append_character_to_word(word_object, word_object.character_entry_reference);
            }
            else
            {
                xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                xsxml_object.result_message = 
                "A tag's PCDATA may contain the ampersand (&) " + 
                "characters only as character entity references.";

                return xsxml_object;
            }

            XML_AMPERSAND = 0;
            continue;
        }

        if (XML_AMPERSAND)
        {
            if (cer_i == (CHARACTER_ENTRY_REFERENCE_MAX_LENGTH - 1))
            {
                xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                xsxml_object.result_message = 
                "A tag's PCDATA may contain the ampersand (&) characters " + 
                "only as character entity references. Character entry "    + 
                "references may contain atmost "                           + 
                CHARACTER_ENTRY_REFERENCE_MAX_LENGTH.toString()            + 
                " number of characters.";

                return xsxml_object;
            }

            word_object.character_entry_reference += file_data_character;
            cer_i++;
            continue;
        }

        if (node_level == 0)
        {
            xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

            xsxml_object.result_message = 
            "Parsed character data (PCDATA or text data) " + 
            "cannot be placed outside the outermost tag.";

            return xsxml_object;
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

        append_character_to_word(word_object, file_data_character);
    }


/* ########################################################################## */
/*                          XML TAGS COMPLETION CHECK                         */
/* ########################################################################## */
    if (node_level != 0)
    {
        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

        if (XML_CDATA_CONTENT)
        {
            xsxml_object.result_message = "A CDATA entity does not terminate.";
        }
        else
        {
            xsxml_object.result_message = node_level.toString() + 
            " start tag elements do not have their end tag counterparts.";
        }

        return xsxml_object;
    }

    xsxml_object.result = Xsxml_Result.XSXML_RESULT_SUCCESS;

    var input_file_name = input_file.value;

    if (input_file_name == null || input_file_name == undefined)
    {
        input_file_name = input_file.name;
    }

    if (input_file_name.indexOf('/') != -1 || input_file_name.indexOf('\\') != -1)
    {
        input_file_name = input_file_name.split('/').reverse()[0];

        if (input_file_name.length == input_file.value.length)
        {
            input_file_name = input_file.value.split('\\').reverse()[0];
        }
    }

    xsxml_object.result_message = 
    "The file \'" + input_file_name + "\' has been successfully parsed.";

    return xsxml_object;
}


function xsxml_occurrence( xsxml_object, 
                           tag_name, 
                           attribute_name, 
                           attribute_value, 
                           content, 
                           direction)
{
    var return_node_indices = [ 0 ];

    if ((tag_name == null || tag_name == undefined) 
    &&  (content  == null || content  == undefined) 
    &&  (attribute_name  == null || attribute_name  == undefined) 
    &&  (attribute_value == null || attribute_value == undefined))
    {
        return return_node_indices;
    }

    var i_start;
    var i_negatory_end_condition;

    if (direction == Xsxml_Direction.XSXML_DIRECTION_FORWARD)
    {
        i_start = 0;
        i_negatory_end_condition = xsxml_object.number_of_nodes;
    }
    else /* if (direction == Xsxml_Direction.XSXML_DIRECTION_BACKWARD) */
    {
        i_start = xsxml_object.number_of_nodes - 1;
        i_negatory_end_condition = -1;
        direction = -1;
    }

    for (var i = i_start; i != i_negatory_end_condition; i += direction)
    {
        var node_i_is_true = 0;

        if (tag_name != null && tag_name != undefined)
        {
            if (tag_name.localeCompare(xsxml_object.node[i].node_name) == 0)
            {
                node_i_is_true = 1;
            }
            else
            {
                continue;
            }
        }

        for (var j = 0; j < xsxml_object.node[i].number_of_attributes; j++)
        {
            if (attribute_name != null && attribute_name != undefined)
            {
                if (attribute_name.localeCompare(xsxml_object.node[i].attribute_name[j]) == 0)
                {
                    node_i_is_true = 1;
                }
                else
                {
                    node_i_is_true = 0;
                    continue;
                }
            }

            if (attribute_value != null && attribute_value != undefined)
            {
                if (attribute_value.localeCompare(xsxml_object.node[i].attribute_value[j]) == 0)
                {
                    node_i_is_true = 1;
                }
                else
                {
                    node_i_is_true = 0;
                    continue;
                }
            }

            if (node_i_is_true)
            {
                break;
            }
        }

        if (content != null && content != undefined)
        {
            node_i_is_true = 0;

            for (var k = 0; k < xsxml_object.node[i].number_of_contents; k++)
            {
                if (xsxml_object.node[i].content[k].indexOf(content) != -1)
                {
                    node_i_is_true = 1;
                    break;
                }
            }
        }

        if (node_i_is_true)
        {
            return_node_indices.push(i);
        }
    }

    return return_node_indices;
}


function compile_all_nodes( xsxml_object, 
                            save_file_object, 
                            xsxml_node_object, 
                            indentation, 
                            vertical_spacing)
{
    var temp_string = '';

    /* Validating tag. */

    if ((xsxml_node_object.node_name.indexOf(' ')  != -1) 
    ||  (xsxml_node_object.node_name.indexOf('\r') != -1) 
    ||  (xsxml_node_object.node_name.indexOf('\n') != -1) 
    ||  (xsxml_node_object.node_name.indexOf('\t') != -1) 
    ||  (xsxml_node_object.node_name.indexOf('\v') != -1) 
    ||  (xsxml_node_object.node_name.indexOf('\f') != -1))
    {
        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

        xsxml_object.result_message = 
        "Tag names cannot have any space characters.";

        return;
    }

    if (!(/[a-zA-Z]/).test(xsxml_node_object.node_name.substr(0, 1)) 
    && (xsxml_node_object.node_name.substr(0, 1) != '_'))
    {
        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

        xsxml_object.result_message = 
        "Tag names must start with an alphabetical character " + 
        "[a-zA-Z] or underscore (_)";

        return;
    }

    if ((xsxml_node_object.node_name.toLowerCase().substr(0, 1) == 'x') 
    &&  (xsxml_node_object.node_name.toLowerCase().substr(1, 1) == 'm') 
    &&  (xsxml_node_object.node_name.toLowerCase().substr(2, 1) == 'l'))
    {
        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

        xsxml_object.result_message = 
        "Tag names cannot start with 'xml' or any of its variants like XML, Xml, etc.";

        return;
    }

    for (var i = 0; i < xsxml_node_object.node_name.length; i++)
    {
        if (!(/[a-zA-Z0-9]/).test(xsxml_node_object.node_name.substr(i, 1)) 
        &&  (xsxml_node_object.node_name.substr(i, 1) != '-') 
        &&  (xsxml_node_object.node_name.substr(i, 1) != '_') 
        &&  (xsxml_node_object.node_name.substr(i, 1) != '.'))
        {
            xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

            xsxml_object.result_message = 
            "Tag names may contain letters [a-zA-z], digits [0-9], " + 
            "hyphens (-), underscores (_), and periods (.) only.";

            return;
        }
    }

    save_file_object.xml_data += '\n' + '\n'.repeat(vertical_spacing);

    save_file_object.xml_data += ' '.repeat(indentation * save_file_object.level);

    save_file_object.xml_data += '<' + xsxml_node_object.node_name;

    const n_attributes = xsxml_node_object.number_of_attributes;

    for (var j = 0; j < n_attributes; j++)
    {
        /* Validating attribute name. */

        for (var k = 0; k < n_attributes; k++)
        {
            if (k == j)
            {
                continue;
            }

            if (xsxml_node_object.attribute_name[j].localeCompare(xsxml_node_object.attribute_name[k]) == 0)
            {
                xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                xsxml_object.result_message = 
                "Within a given tag, attributes cannot share the same name.";

                return;
            }
        }

        if ((xsxml_node_object.attribute_name[j].indexOf(' ')  != -1) 
        ||  (xsxml_node_object.attribute_name[j].indexOf('\r') != -1) 
        ||  (xsxml_node_object.attribute_name[j].indexOf('\n') != -1) 
        ||  (xsxml_node_object.attribute_name[j].indexOf('\t') != -1) 
        ||  (xsxml_node_object.attribute_name[j].indexOf('\v') != -1) 
        ||  (xsxml_node_object.attribute_name[j].indexOf('\f') != -1))
        {
            xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

            xsxml_object.result_message = 
            "Tag names cannot have any space characters.";

            return;
        }

        if (!(/[a-zA-Z]/).test(xsxml_node_object.attribute_name[j].substr(0, 1)) 
        && (xsxml_node_object.attribute_name[j].substr(0, 1) != '_'))
        {
            xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

            xsxml_object.result_message = 
            "Tag names must start with an alphabetical character " + 
            "[a-zA-Z] or underscore (_)";

            return;
        }

        for (var i = 0; i < xsxml_node_object.attribute_name[j].length; i++)
        {
            if (!(/[a-zA-Z0-9]/).test(xsxml_node_object.attribute_name[j].substr(i, 1)) 
            && (xsxml_node_object.attribute_name[j].substr(i, 1) != '-') 
            && (xsxml_node_object.attribute_name[j].substr(i, 1) != '_') 
            && (xsxml_node_object.attribute_name[j].substr(i, 1) != '.'))
            {
                xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                xsxml_object.result_message = 
                "Tag names may contain letters [a-zA-z], digits [0-9], " + 
                "hyphens (-), underscores (_), and periods (.) only.";

                return;
            }
        }


        /* Validating attribute value. */

        if (xsxml_node_object.attribute_value[j].indexOf('<') != -1)
        {
            xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

            xsxml_object.result_message = 
            "Attribute values may not contain the less-than (<) characters.";

            return;
        }

        temp_string = xsxml_node_object.attribute_value[j];

        while (1)
        {
            const ret_1 = temp_string.indexOf('&');

            if (ret_1 == -1)
            {
                break;
            }
            else /* if (ret_1 != -1) */
            {
                temp_string = temp_string.slice(ret_1);

                const ret_2 = temp_string.indexOf(';');

                if (ret_2 == -1)
                {
                    xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                    xsxml_object.result_message = 
                    "Attribute values may contain the ampersand (&) " + 
                    "characters only as character entity references.";

                    return;
                }
                else /* if (ret_2 != -1) */
                {
                    var cer_data_object = 
                    {
                        character_entry_reference : temp_string.substring(1, ret_2 - 1)
                    };

                    if (!parse_cer(cer_data_object))
                    {
                        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                        xsxml_object.result_message = 
                        "Attribute values may contain the ampersand (&) " + 
                        "characters only as character entity references.";

                        return;
                    }
                }

                temp_string = temp_string.slice(ret_2);
            }
        }

        save_file_object.xml_data += ' ' + xsxml_node_object.attribute_name[j];

        save_file_object.xml_data += '=\"' + xsxml_node_object.attribute_value[j] + '\"';
    }

    save_file_object.xml_data += '>';

    const n_contents = xsxml_node_object.number_of_contents;

    if (n_contents > 0)
    {
        /* Validating PCDATA. */

        for (var j = 0; j < n_contents; j++)
        {
            var cdata_tags_n = 0;

            var pos_prefix_cumulative = 0;

            var cdata_tag_pos_start = [];
            var cdata_tag_pos_end   = [];

            temp_string = xsxml_node_object.content[j];

            while (1)
            {
                const ret_1 = temp_string.indexOf('<![CDATA[');

                if (ret_1 == -1)
                {
                    break;
                }
                else /* if (ret_1 != -1) */
                {
                    temp_string = temp_string.slice(ret_1);

                    const ret_2 = temp_string.indexOf(']]>');

                    if (ret_2 == -1)
                    {
                        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                        xsxml_object.result_message = 
                        "A CDATA entity does not terminate.";

                        return;
                    }
                    else /* if (ret_2 != -1) */
                    {
                        cdata_tags_n++;

                        cdata_tag_pos_start [cdata_tags_n - 1] = 
                        pos_prefix_cumulative + ret_1;

                        cdata_tag_pos_end [cdata_tags_n - 1] = 
                        pos_prefix_cumulative + ret_2;

                        pos_prefix_cumulative += ret_2;

                        temp_string = temp_string.slice(ret_2);
                    }
                }
            }

            temp_string = xsxml_node_object.content[j];

            while (1)
            {
                const ret_1 = temp_string.indexOf('<');

                if (ret_1 == -1)
                {
                    break;
                }
                else /* if (ret_1 != -1) */
                {
                    temp_string = temp_string.slice(ret_1);

                    var char_is_valid = 0;

                    for (var k = 0; k < cdata_tags_n; k++)
                    {
                        if ((ret_1 > cdata_tag_pos_start[k]) 
                        &&  (ret_1 < cdata_tag_pos_end[k]))
                        {
                            char_is_valid = 1;
                            break;
                        }
                    }

                    if (!char_is_valid)
                    {
                        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                        xsxml_object.result_message = 
                        "A tag's PCDATA may not contain the less-than (<) characters.";

                        return;
                    }
                }
            }

            temp_string = xsxml_node_object.content[j];

            while (1)
            {
                const ret_1 = temp_string.indexOf('&');

                if (ret_1 == -1)
                {
                    break;
                }
                else /* if (ret_1 != -1) */
                {
                    temp_string = temp_string.slice(ret_1);

                    var char_is_valid = 0;

                    for (var k = 0; k < cdata_tags_n; k++)
                    {
                        if ((ret_1 > cdata_tag_pos_start[k]) 
                        &&  (ret_1 < cdata_tag_pos_end[k]))
                        {
                            char_is_valid = 1;
                            break;
                        }
                    }

                    if (!char_is_valid)
                    {
                        const ret_2 = temp_string.indexOf(';');

                        if (ret_2 == -1)
                        {
                            xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                            xsxml_object.result_message = 
                            "A tag's PCDATA may contain the ampersand (&) " + 
                            "characters only as character entity references.";

                            return;
                        }
                        else /* if (ret_2 != -1) */
                        {
                            temp_string = temp_string.slice(ret_2);

                            var cer_data_object = 
                            {
                                character_entry_reference : temp_string.substring(1, ret_2 - 1)
                            };

                            if (!parse_cer(cer_data_object))
                            {
                                xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

                                xsxml_object.result_message = 
                                "A tag's PCDATA may contain the ampersand (&) "
                                "characters only as character entity references.";

                                return;
                            }
                        }
                    }
                }
            }
        }
    }

    var content_i = 0;

    var last_element_was_tag;

    if (n_contents > 0)
    {
        xsxml_node_object.content[content_i].replaceAll('\r', ' ');
        xsxml_node_object.content[content_i].replaceAll('\n', ' ');
        xsxml_node_object.content[content_i].replaceAll('\t', ' ');
        xsxml_node_object.content[content_i].replaceAll('\v', ' ');
        xsxml_node_object.content[content_i].replaceAll('\f', ' ');

        while (xsxml_node_object.content[content_i].indexOf('  ') != -1)
        {
            xsxml_node_object.content[content_i].replaceAll('  ', ' ');
        }

        save_file_object.xml_data += xsxml_node_object.content[content_i];

        content_i++;

        last_element_was_tag = 0;
    }
    else /* if (n_contents == 0) */
    {
        last_element_was_tag = 1;
    }

    if (xsxml_node_object.descendant != null && xsxml_node_object.descendant != undefined)
    {
        var xsxml_sub_node_object = xsxml_node_object.descendant;

        save_file_object.level++;

        while (xsxml_sub_node_object != null && xsxml_sub_node_object != undefined)
        {
            compile_all_nodes( xsxml_object, 
                               save_file_object, 
                               xsxml_sub_node_object, 
                               indentation, 
                               vertical_spacing);

            if (xsxml_object.result != Xsxml_Result.XSXML_RESULT_SUCCESS)
            {
                return;
            }

            if (content_i < n_contents)
            {
                /*
                save_file_object.xml_data += ' '.repeat(indentation * (save_file_object.level + 1));

                save_file_object.xml_data += 
                xsxml_node_object.content[content_i].replaceAll( '\n', 
                                                                 '\n' + 
                                                                 ' '.repeat(    indentation 
                                                                             * (save_file_object.level + 1)
                                                                           )
                                                               ) + '\n';

                save_file_object.xml_data += '\n'.repeat(vertical_spacing);
                */

                xsxml_node_object.content[content_i].replaceAll('\r', ' ');
                xsxml_node_object.content[content_i].replaceAll('\n', ' ');
                xsxml_node_object.content[content_i].replaceAll('\t', ' ');
                xsxml_node_object.content[content_i].replaceAll('\v', ' ');
                xsxml_node_object.content[content_i].replaceAll('\f', ' ');

                while (xsxml_node_object.content[content_i].indexOf('  ') != -1)
                {
                    xsxml_node_object.content[content_i].replaceAll('  ', ' ');
                }

                save_file_object.xml_data += xsxml_node_object.content[content_i];

                content_i++;

                last_element_was_tag = 0;
            }
            else /* if (content_i == n_contents) */
            {
                last_element_was_tag = 1;
            }

            xsxml_sub_node_object = xsxml_sub_node_object.next_sibling;
        }

        save_file_object.level--;

        if (last_element_was_tag)
        {
           save_file_object.xml_data += '\n' + '\n'.repeat(vertical_spacing);

           save_file_object.xml_data += ' '.repeat(indentation * save_file_object.level);
        }

        save_file_object.xml_data += '</' + xsxml_node_object.node_name;
    }
    else
    {
        if (n_contents > 0)
        {
            save_file_object.xml_data += '</' + xsxml_node_object.node_name;
        }
        else
        {
            save_file_object.xml_data = save_file_object.xml_data.slice(0, -1);
            save_file_object.xml_data += '/';
        }
    }

    save_file_object.xml_data += '>';
}


function xsxml_compile( xsxml_object, 
                        indentation, 
                        vertical_spacing)
{
    if (indentation > MAX_INDENTATION)
    {
        xsxml_object.result = Xsxml_Result.XSXML_RESULT_FILE_FAILURE;

        xsxml_object.result_message = 
        "Error in input argument number 4 of function 'xsxml_compile'." + 
        "You cannot have more than " + MAX_INDENTATION.toString()       + 
        " horizontal spaces as indentation.";

        return '';
    }

    if (vertical_spacing > MAX_VERTICAL_SPACING)
    {
        xsxml_object.result = Xsxml_Result.XSXML_RESULT_FILE_FAILURE;

        xsxml_object.result_message = 
        "Error in input argument number 5 of function 'xsxml_compile'." + 
        "You cannot have more than " + MAX_VERTICAL_SPACING.toString()  + 
        " vertical spaces.";

        return '';
    }

    if (xsxml_object.node == null || xsxml_object.node == undefined)
    {
        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

        xsxml_object.result_message = "There must be exactly one outermost tag.";

        return '';
    }

    if (xsxml_object.node[0] == null || xsxml_object.node[0] == undefined)
    {
        xsxml_object.result = Xsxml_Result.XSXML_RESULT_XML_FAILURE;

        xsxml_object.result_message = "There must be exactly one outermost tag.";

        return '';
    }

    var save_file_object = { xml_data : '', level : 0 };

    save_file_object.xml_data = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";

    if (vertical_spacing == 0)
    {
        save_file_object.xml_data += '\n';
    }

    xsxml_object.result = Xsxml_Result.XSXML_RESULT_SUCCESS;

    xsxml_object.result_message = 
    "The XSXML object's information has been successfully compiled.";

    compile_all_nodes( xsxml_object, 
                       save_file_object, 
                       xsxml_object.node[0], 
                       indentation, 
                       vertical_spacing);

    if (xsxml_object.result == Xsxml_Result.XSXML_RESULT_SUCCESS)
    {
        save_file_object.xml_data += '\n\n';

        return save_file_object.xml_data;
    }
    else /* if (xsxml_object.result != Xsxml_Result.XSXML_RESULT_SUCCESS) */
    {
        return '';
    }
}

