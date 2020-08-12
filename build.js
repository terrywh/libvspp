"use strict";

import { normalize } from 'path';
import { dirname } from "path";

async function main(__root, __name) {
    let { build } = await import(__root + "/src/" + __name + "/" + __name + ".js")
    await build(__root + "/gen")
}

main(
    normalize(dirname(process.argv[1])), // __root
    process.argv[2]) // __name
