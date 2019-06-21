"use strict";

import llparse from 'llparse';

import { promises as fs } from 'fs';
import { dirname } from "path";
import { fileURLToPath } from "url";

const ERRORS = Object.fromEntries([
    "SUCCESS",
    "UNEXPECTED_TOKEN",
    "UNKNOWN_ERROR",
].map((v, i) => [v, i]));

const VALUE_TYPES = Object.fromEntries([
    "UNKNOWN",
    "BOOLEAN",
    "INTEGER",
    "HEX_INTEGER",
    "OCT_INTEGER",
    "BIN_INTEGER",
    "FLOAT",
    "INFINITY",
    "NAN",
    "BASIC_STRING",
    "RAW_STRING",
    "DATE",
].map((v, i) => [v, i]));

const CONTAINER_TYPES = Object.fromEntries([
    "TABLE",
    "ARRAY",
    "ARRAY_TABLE",
].map((v, i) => [v, i]));

const parser = new llparse.LLParse("toml__internal");

function E(name, where) {
    if(ERRORS[name]) return parser.error(ERRORS[name], where);
    else throw new Error("undefined error '" + name + "'");
}

// 属性
parser.property("i8", "value_type");
parser.property("i8", "container_type");
// 初始
// 前缀
const   type_prefix = parser.node("before_prefix_table");
const before_prefix = parser.node("before_prefix");
const     on_prefix_start = parser.invoke(parser.code.match("toml__internal_on_section"), before_prefix);
const     on_prefix = parser.span(parser.code.span("toml__internal_on_field"));
const        prefix_basic = parser.node("prefix_basic");
const        prefix_quote = parser.node("prefix_quote");
const  after_prefix = parser.node("after_prefix");
const  after_prefix_next = parser.node("after_prefix_next");
const  after_prefix_which = parser.node("after_prefix_which");
// 字段
const    before_field = parser.node("before_field");
const on_before_field = parser.invoke(parser.code.update("value_type", VALUE_TYPES["UNKNOWN"]), before_field);
// const        on_field = parser.span(parser.code.span("toml__internal_on_field"));
const        on_field = on_prefix;
const           field_quote = parser.node("field_quote");
const           field_basic = parser.node("field_basic");
const     after_field = parser.node("after_field");
// const  on_after_field = parser.invoke(parser.code.match("toml__internal_on_after_field"), after_field);
const  on_after_field = after_field;
const     after_field_next = parser.node("after_field_next");
const     after_field_expect_equal = parser.node("after_field_expect_equal");
const     after_field_expect_field = parser.node("after_field_expect_equal");
// 符号
const    before_equal = parser.node("before_equal");
const           equal = parser.node("equal");
const     after_equal = parser.node("after_equal");
// 值
const    before_value = parser.node("before_value");
const on_before_value = parser.invoke(parser.code.update("value_type", VALUE_TYPES["UNKNOWN"]), before_value);
const        on_value = parser.span(parser.code.span("toml__internal_on_value"));
// 值 - 无分隔符
const           value_unknown = parser.node("value_unknown");
const           value_boolean = parser.node("value_boolean");
const        on_value_boolean = parser.invoke(parser.code.update("value_type", VALUE_TYPES["BOOLEAN"]), value_boolean);
const           value_integer = parser.node("value_integer");
const        on_value_integer = parser.invoke(parser.code.update("value_type", VALUE_TYPES["INTEGER"]), value_integer);
const           value_hex_integer = parser.node("value_hex_integer");
const        on_value_hex_integer = parser.invoke(parser.code.update("value_type", VALUE_TYPES["HEX_INTEGER"]), value_hex_integer);
const           value_oct_integer = parser.node("value_oct_integer");
const        on_value_oct_integer = parser.invoke(parser.code.update("value_type", VALUE_TYPES["OCT_INTEGER"]), value_oct_integer);
const           value_bin_integer = parser.node("value_bin_integer");
const        on_value_bin_integer = parser.invoke(parser.code.update("value_type", VALUE_TYPES["BIN_INTEGER"]), value_bin_integer);
const           value_float = parser.node("value_float")
const        on_value_float = parser.invoke(parser.code.update("value_type", VALUE_TYPES["FLOAT"]), value_float);
const           value_infinite = parser.node("value_infinite");
const        on_value_infinite = parser.invoke(parser.code.update("value_type", VALUE_TYPES["INFINITY"]), value_infinite);
const           value_date = parser.node("value_date");
const        on_value_date = parser.invoke(parser.code.update("value_type", VALUE_TYPES["DATE"]), value_date);
const     after_value_date = parser.node("after_value_date");
// 值 - 有分隔符
const    before_value_basic_string = parser.node("before_value_basic_string");
const on_before_value_basic_string = parser.invoke(parser.code.update("value_type", VALUE_TYPES["BASIC_STRING"]), before_value_basic_string);
const           value_basic_string = parser.node("value_basic_string");
const           value_multi_basic_string = parser.node("value_multi_basic_string");
const     after_value_multi_basic_string = parser.node("after_value_multi_basic_string");
const    before_value_raw_string = parser.node("before_value_raw_string");
const on_before_value_raw_string =  parser.invoke(parser.code.update("value_type", VALUE_TYPES["RAW_STRING"]), before_value_raw_string);
const           value_raw_string = parser.node("value_raw_string");
const           value_multi_raw_string = parser.node("value_multi_raw_string");
const     after_value_multi_raw_string = parser.node("after_value_multi_raw_string");
// 结束
const     after_value_expect_field = parser.node("after_value_expect_field");
const     after_value_expect_value = parser.node("after_value_expect_value");
const  on_after_value_which = parser.invoke(parser.code.load("container_type"), {2: after_value_expect_field, 1: after_value_expect_value, 0: after_value_expect_field}, E("UNKNOWN_ERROR", "after_value"));
const  on_after_value = parser.invoke(parser.code.match("toml__internal_on_after_value"), on_after_value_which);

// 容器 数组、表格 层级关系处理
const     on_cstart_array = parser.invoke(parser.code.update("container_type", CONTAINER_TYPES["ARRAY"]),
    parser.invoke(parser.code.match("toml__internal_on_entry_push"), on_before_value));
const     on_cstart_table = parser.invoke(parser.code.update("container_type", CONTAINER_TYPES["TABLE"]),
    parser.invoke(parser.code.match("toml__internal_on_entry_push"), on_before_field));
const     on_cstart_array_table = parser.invoke(parser.code.update("container_type", CONTAINER_TYPES["ARRAY_TABLE"]),
    parser.invoke(parser.code.match("toml__internal_on_entry_push"), on_before_field));
// const before_cfinish = parser.node("before_cfinish");
const     on_cfinish = parser.invoke(parser.code.match("toml__internal_on_entry_pop"), on_after_value_which); // on_entry_pop 需要维护, 不再调用 on_after_value

// 注释
const comment_expect_field = parser.node("begin_comment_expect_field");
const comment_expect_equal = parser.node("begin_comment_expect_equal");
const comment_expect_value = parser.node("begin_comment_expect_value");
// 前缀
// --------------------------------------------------------------------------------
type_prefix
    .match("[[", on_prefix_start)
    .otherwise(on_prefix_start);

before_prefix
    // .skipTo(comment_expect_field)
    .peek(["#", "]"], E("UNEXPECTED_TOKEN", "before_prefix"))
    .match([" ", "\t"], before_prefix)
    .match('"', on_prefix.start(prefix_quote)) // on_prefix 实际上与 on_field 是同一个 SPAN , 区别在 cstart 后的转换过程
    .otherwise(on_prefix.start(prefix_basic));

prefix_basic
    .peek(["#", "\r", "\n"], E("UNEXPECTED_TOKEN", "prefix_basic"))
    .peek([".", " ", "\t", "]"], on_prefix.end(after_prefix))
    .skipTo(prefix_basic);

prefix_quote
    .peek("\"", on_prefix.end(after_prefix))
    .skipTo(prefix_quote);

after_prefix
    .match([" ", "\t", "\r", "\n"], after_prefix)
    .peek("]", after_prefix_which)
    .peek('.', on_prefix.start(after_prefix_next))
    .otherwise(E("UNEXPECTED_TOKEN", "after_prefix"));

after_prefix_which
    .match("]]", on_cstart_array_table)
    .skipTo(on_cstart_table)

after_prefix_next
    .skipTo(on_prefix.end(before_prefix));

// 字段
// --------------------------------------------------------------------------------
before_field // <-- after_field / after_value
    .match([" ", "\t", "\r", "\n"], before_field)
    .peek("#", comment_expect_field)
    .match("[", type_prefix)
    .match("\"", on_field.start(field_quote))
    .otherwise(on_field.start(field_basic));

field_basic
    .peek([".", " ", "\t", "]", "=", "#", "\r", "\n"], on_field.end(on_after_field))
    .skipTo(field_basic);

field_quote
    .peek("\"", on_field.end(on_after_field))
    .skipTo(field_quote);

after_field
    .match(["\"", " ", "\t"], after_field_expect_equal)
    .peek(".", on_field.start(after_field_next)) // 子级 key 不改变数据类型
    .match("]", after_field_expect_field)
    // .peek("#", comment_expect_equal) // 不允许出现此种情况 AAAA #CCCCC \n = BBBBB
    .otherwise( before_equal );

after_field_next
    .skipTo(on_field.end(before_field))

after_field_expect_equal
    .match([" ", "\t"], after_field_expect_equal)
    .otherwise(before_equal);

after_field_expect_field
    .match([" ", "\t"], after_field_expect_field)
    .otherwise(on_before_field); // 注意重置数据类型

// 符号
// --------------------------------------------------------------------------------
before_equal
    .peek(["\r", "\n"], E("UNEXPECTED_TOKEN", "before_equal")) // 不允许出现此种情况 AAAA \n = BBBBB
    .match([" ", "\t"], before_equal)
    .peek("#", comment_expect_equal)
    .otherwise(equal);

equal
    .match("=", after_equal)
    .otherwise(E("UNEXPECTED_TOKEN", "equal"));

after_equal
    .match([" ", "\t"], after_equal)
    .peek("#", comment_expect_value)
    .peek(["\r", "\n"], E("UNEXPECTED_TOKEN", "after_equal"))
    .otherwise(before_value);

// 值
// --------------------------------------------------------------------------------

before_value
    .match([" ", "\t"], before_value)
    .peek(["\r", "\n"], E("UNEXPECTED_TOKEN", "before_value"))
    // 0 比较特别（0x/0o/0b)
    .match("+", on_value.start(on_value_integer))
    .peek(["-", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9"], on_value.start(on_value_integer))
    .peek("t", on_value.start(on_value_boolean)) // true
    .peek("f", on_value.start(on_value_boolean)) // false
    .match("[", on_cstart_array)
    .match("{", on_cstart_table)
    // 待确认是否未多行文本
    .match('"', on_before_value_basic_string)
    .match("'", on_before_value_raw_string)
    .peek("#", comment_expect_value)
    .otherwise(on_value.start(value_unknown))

value_unknown
    .match(["-", ":"], on_value_date)
    .peek([",", "]", "}", " ", "\t", "#", "\r", "\n"], on_value.end(on_after_value))
    .skipTo(value_unknown);

value_boolean
    .match(["true", "false"], on_value.end(on_after_value))
    .otherwise(E("UNEXPECTED_TOKEN", "value_boolean"));

value_integer
    .peek([",", "]", "}", " ", "\t", "#", "\r", "\n"], on_value.end(on_after_value))
    .match("0x", on_value_hex_integer)
    .match("0o", on_value_oct_integer)
    .match("0b", on_value_bin_integer)
    .match("+inf", on_value_infinite)
    // .match("-inf", on_value_infinite) // TODO 归并 -inf 与 - 的检查
    .match("inf", on_value_infinite)
    .match("e-", on_value_float)
    .match("e+", on_value_float)
    .match("E-", on_value_float)
    .match("E+", on_value_float)
    .match(["-", ":"], on_value_date)
    .match(".", on_value_float)
    .skipTo(value_integer);

value_hex_integer
    .match(["_", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "a", "b", "c", "d", "e", "f", "A", "B", "C", "D", "E", "F"], value_hex_integer)
    .otherwise(on_value.end(on_after_value));

value_oct_integer
    .match(["_", "0", "1", "2", "3", "4", "5", "6", "7"], value_hex_integer)
    .otherwise(on_value.end(on_after_value));

value_bin_integer
    .match(["_", "0", "1"], value_hex_integer)
    .otherwise(on_value.end(on_after_value));

value_infinite
    .otherwise(on_value.end(on_after_value));

value_float
    .peek([",", "]", "}", " ", "t", "#", "\r", "\n"], on_value.end(on_after_value))
    .skipTo(value_float);

value_date
    .peek(" ", after_value_date) // 可能是结束符号
    .peek([",", "]", "}", "\t", "#", "\r", "\n"], on_value.end(on_after_value))
    .skipTo(value_date);

after_value_date
    .match([" 0", " 1", " 2"], value_date) // 合理的三种形式
    .otherwise(on_value.end(on_after_value)); // 其他形式结束匹配

// 值 - 分隔符
// ----------------------------------------------------------------------------------------------------
before_value_basic_string
    .match('""', on_value.start(value_multi_basic_string))
    .otherwise(on_value.start(value_basic_string))

value_basic_string
    .peek('"', on_value.end(on_after_value))
    .peek(["\r", "\n"], E("UNEXPECTED_TOKEN", "value_basic_string"))
    .skipTo(value_basic_string);

value_multi_basic_string
    .peek('"', on_value.end(after_value_multi_basic_string)) // 暂停解析，但有可能恢复
    .skipTo(value_multi_basic_string);

after_value_multi_basic_string
    .match('"""', on_after_value)
    .otherwise(on_value.start(value_multi_basic_string)); // 未匹配结束符，恢复

before_value_raw_string
    .match("''", on_value.start(value_multi_raw_string))
    .otherwise(on_value.start(value_raw_string))

value_raw_string
    .peek("'", on_value.end(on_after_value))
    .skipTo(value_raw_string);

value_multi_raw_string
    .peek("'", on_value.end(after_value_multi_raw_string)) // 暂停解析，但有可能恢复
    .skipTo(value_multi_raw_string);

after_value_multi_raw_string
    .match("'''", on_after_value)
    .otherwise(on_value.start(value_multi_raw_string)); // 未匹配结束符，恢复

after_value_expect_field
    .match([",", " ", "\t", "\cfinish"], after_value_expect_field) // 不允许在 value 后继续定义 field 必须换行
    .match(["}"/*, "]"*/], on_cfinish) // 容器结束，需要回到上一层
    .peek("]", E("UNEXPECTED_TOKEN", "after_value"))
    .peek("#", comment_expect_field)
    .otherwise(on_before_field);

after_value_expect_value
    .match([",", " ", "\t", "\cfinish"], after_value_expect_value)
    .match([/*"}",*/ "]"], on_cfinish) // 容器结束，需要回到上一层
    .peek("}", E("UNEXPECTED_TOKEN", "after_value"))
    .peek("#", comment_expect_value)
    .otherwise(on_before_value);

// 注释
// ----------------------------------------------------------------------------------------------
comment_expect_field
    .match(["\r", "\n"], on_before_field)
    // .match("#", comment_expect_field)
    .skipTo(comment_expect_field);

comment_expect_equal
    .match(["\r", "\n"], before_equal)
    // .match("#", comment_expect_equal)
    .skipTo(comment_expect_equal);

comment_expect_value
    .match(["\r", "\n"], before_value)
    // .match("#", comment_expect_value)
    .skipTo(comment_expect_value);

const __dirname  = dirname(fileURLToPath(import.meta.url));

async function constant_header() {
    let header = "#ifndef TOML__INTERNAL_CONSTANT_H\n#define TOML__INTERNAL_CONSTANT_H\n\n#include <cstdint>\n\n";
    header += "namespace llparse::toml {\n";
        
    header += "\n\tenum error_t: std::uint8_t {\n";
    for(let key in ERRORS) {
        header += "\t\t" + key.padEnd(16) + " = " + ERRORS[key] + ",\n";
    }
    header += "\t};\n"

    header += "\n\tenum class container_type_t: std::uint8_t {\n";
    for(let key in CONTAINER_TYPES) {
        header += "\t\t" + key.padEnd(16) + " = " + CONTAINER_TYPES[key] + ",\n";
    }
    header += "\t};\n";

    header += "\n\tenum class value_type_t: std::uint8_t {\n";
    for(let key in VALUE_TYPES) {
        header += "\t\t" + key.padEnd(16) + " = " + VALUE_TYPES[key] + ",\n";
    }
    header += "\t};\n";
    
    header += "}\n#endif // TOML__INTERNAL_CONSTANT_H\n";
    await fs.writeFile(__dirname + "/constant.h", header);
}

async function build() {
    const art = parser.build(before_field, {"c": {
        "header": "parser",
    }});
    await fs.writeFile(__dirname + "/parser.h", art.header);
    await fs.writeFile(__dirname + "/parser.c", art.c);
    await constant_header();
    console.log("parser source and constant header generated.");
}

build().catch((error) => {
    console.log(error);
    process.exit(-1);
});
