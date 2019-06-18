"use strict";

import llparse from 'llparse';

import { promises as fs } from 'fs';
import { dirname } from "path";
import { fileURLToPath } from "url";

const p = new llparse.LLParse("toml__internal");

const begin = p.node("begin");
const fieldPrefixWS = p.node("field_prefix_whitespace");
const fieldSpan = p.span(p.code.span("toml__on_field"));
const field = p.node("field");
const fieldSuffixWS = p.node("field_suffix_whitespace");

const equalOp = p.node("equal");

const valuePrefixWS = p.node("value_prefix_whitespace");
const valueSpan = p.span(p.code.span("toml__on_value"));
const value = p.node("value");
const valueSuffixWS = p.node("value_suffix_whitespace");

const beforeNext = p.node("before_next");

const invokeFinish = p.invoke(p.code.match("toml__on_finish"), {0: begin}, p.error(1, "unable to finish"));

p.property("ptr", "settings");


begin
    .peek(" ", fieldPrefixWS)
    .match("\0", invokeFinish)
    .otherwise(fieldPrefixWS)

fieldPrefixWS
    .match(" ", fieldPrefixWS)
    .otherwise(fieldSpan.start(field));

field
    .peek(" ", fieldSpan.end(fieldSuffixWS))
    .peek("=", fieldSpan.end(equalOp))
    .peek("\r", fieldSpan.end(beforeNext))
    .peek("\n", fieldSpan.end(beforeNext))
    .peek("\0", fieldSpan.end(begin))
    .skipTo(field);

fieldSuffixWS
    .peek("=", equalOp)
    .peek("\r", beforeNext)
    .peek("\n", beforeNext)
    .match(" ", fieldSuffixWS)
    .otherwise(equalOp);

equalOp
    .match("=", valuePrefixWS)
    .otherwise(p.error(10, "operator '=' expected"));

valuePrefixWS
    .match(" ", valuePrefixWS)
    .otherwise(valueSpan.start(value));

value
    .peek(" ", valueSpan.end(valueSuffixWS))
    .peek("\r", valueSpan.end(beforeNext))
    .peek("\n", valueSpan.end(beforeNext))
    .peek("\0", valueSpan.end(begin))
    .skipTo(value);

valueSuffixWS
    .match(" ", valueSuffixWS)
    .otherwise(beforeNext);

beforeNext
    .match("\r", beforeNext)
    .match("\n", beforeNext)
    .otherwise(fieldPrefixWS);



const art = p.build(fieldPrefixWS, {
    "c": {
        header: "toml",
    }
});

// console.log('----- BITCODE -----');
// console.log(art.bitcode);  // buffer
// console.log('----- BITCODE END -----');

async function build() {
    const __dirname  = dirname(fileURLToPath(import.meta.url));

    await fs.writeFile(__dirname + "/toml.h", art.header);
    await fs.writeFile(__dirname + "/toml.c", art.c);

    console.log("done");
}

build();