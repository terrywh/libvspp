"use strict";

import {create} from "../builder.js"

const ERRORS = [
    "SUCCESS",
    "UNEXPECTED_TOKEN",
    "BOUNDARY",
    "COMPLETE",
]

const {
    ERROR, NODE, CONSUME, SPAN, ON_MATCH, CALLBACK,
    parser, header, source, enumeration, include, callback
} = create({
    name: "multipart_parser", 
    errors: ERRORS, 
    props: {
        "boundary": "ptr",
        "boundary_length": "i8",
        "boundary_origin_": "i8",
        "boundary_offset_": "i8",
    }
})

export async function build(__gen) {
    await parser("on_bounary_length")
        .output(__gen)
    await header("multipart_core")
        .append( enumeration("multipart_error").comment("错误编码").fromArray(ERRORS) )
        .output(__gen)
    await source("multipart_core")
        .append( include("multipart_core.h") )
        .append( include("multipart_parser.h") )
        .append( include("string.h").system() )
        .append( callback("on_bounary_length")
            .comment("初始化 boundary 长度")
            .body(`
    if(s->boundary_length == 0) s->boundary_length = strlen(s->boundary);
    return !(s->boundary_length > 0);`) )
        .append( callback("on_boundary_reset")
            .comment("重置 OFFSET 检查位置")
            .body(`
    s->boundary_origin_ = s->boundary_length;
    s->boundary_offset_ = 0;
    return 0;`) )
        .append( callback("on_content").comment("代理错误的 BOUNDARY 匹配") )
        .append(`
static unsigned char eol[2] = "\\r\\n";`)
        .append( callback("on_boundary").comment("匹配 BOUNDARY 数据").body(`
    const unsigned char* boundary = (unsigned char*)s->boundary;
    if(s->boundary_offset_ > s->boundary_length) { // 已匹配失败
        if(endp > p) ${CALLBACK("on_content")}(s, p, endp);
        return 0;
    }
    if(memcmp(boundary + s->boundary_offset_, p, endp - p) != 0) { // 本次匹配失败
        ${CALLBACK("on_content")}(s, eol, eol+2);
        ${CALLBACK("on_content")}(s, boundary, boundary + s->boundary_offset_);
        ${CALLBACK("on_content")}(s, p, endp);
        s->boundary_offset_ = -1;
        return 0;
    }
    s->boundary_offset_ += endp - p;
    return 0;`))
        .append( callback("if_boundary").comment("检查 BOUNDARY 是否完整匹配").body(`
    return s->boundary_origin_ == 0 && s->boundary_offset_ == s->boundary_length;`) )
        .output(__gen)
}

ON_MATCH("on_bounary_length")
    .match(0, ON_MATCH("on_boundary_reset") )
    .otherwise( ERROR("BOUNDARY", "on_boundary_length") )

NODE("before_boundary")
    .match("\r\n", ON_MATCH("on_boundary_reset"))
    .otherwise( SPAN("on_content").start( NODE("content_ex") ))

ON_MATCH("on_boundary_reset")
    .otherwise( SPAN("on_boundary").start( CONSUME("boundary") ))

CONSUME("boundary")
    .consume("boundary_origin_")
    .otherwise( SPAN("on_boundary").end( ON_MATCH("if_boundary") ))

ON_MATCH("if_boundary")
    .match(1, NODE("after_boundary") )
    .otherwise( SPAN("on_content").start( NODE("content") ))

NODE("after_boundary")
    .match("\r\n", SPAN("on_field").start( NODE("field") ))
    .match("--", ON_MATCH("on_complete") )
    .otherwise( ERROR("UNEXPECTED_TOKEN", "after_boundary")  )

NODE("before_field")
    .match("\r\n", SPAN("on_field").start( NODE("field") ))
    .otherwise( ERROR("UNEXPECTED_TOKEN", "before_field") )

NODE("field")
    .peek(":", SPAN("on_field").end( NODE("before_value") ))
    .peek("\r", SPAN("on_field").end( NODE("after_value_1") ))
    .skipTo( NODE("field") )

NODE("before_value")
    .match([":"," "], NODE("before_value"))
    .otherwise(SPAN("on_value").start(NODE("value")))

NODE("value")
    .peek("\r", SPAN("on_value").end( NODE("after_value_1") ))
    .skipTo(NODE("value"));

NODE("after_value_1")
    .match("\r\n", NODE("after_value_2"))
    .otherwise( ERROR("UNEXPECTED_TOKEN", "after_value_1") )

NODE("after_value_2")
    .match("\r\n", SPAN("on_content").start( NODE("content") ))
    .otherwise(SPAN("on_field").start( NODE("field") ))

NODE("content")
    .peek("\r", SPAN("on_content").end( NODE("before_boundary") ))
    .skipTo( NODE("content") )

NODE("content_ex")
    .skipTo(NODE("content"))

ON_MATCH("on_complete")
    .match(0, SPAN("on_boundary").start( CONSUME("boundary") ))
    .otherwise( ERROR("COMPLETE", "complete") )
