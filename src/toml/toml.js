"use strict";

import builder, {ERRORS, TYPES} from "./toml_core.js"
const {parser, header, include, enumeration} = builder

export async function build(__gen) {
    await parser("before_field")
        .output(__gen)
    await header("toml")
        .append(include("toml_parser.h"))
        .append(enumeration("toml_error").comment("错误编码").fromArray(ERRORS))
        .append(enumeration("toml_type").comment("数据类型").fromArray(TYPES))
        .output(__gen)
}

import "./toml_string.js"