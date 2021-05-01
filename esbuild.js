#!/usr/bin/env node

let esbuild = require('esbuild')

const build = async () => {
    await esbuild.build({
        entryPoints: ['./src/WebService/Index.bs.js' ],
        logLevel: 'info',
        bundle: true,
        minify: true,
        sourcemap: 'external',
        outfile: './src/Server/public/dist/index.js'
    })

    console.log('done');
}

build();
