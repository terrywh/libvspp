"use strict";

import { resolve, dirname } from 'path';
import { mkdir } from 'node:fs/promises';



async function main(__parser, __build) {
    let { build } = await import(__parser)
    await mkdir(__build, {"recursive":true})
    await build(__build)
}

main(process.argv[2], // /path/to/parser.js
    process.argv[3]) // /dir/of/generated/code
