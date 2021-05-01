#!/usr/bin/env node

let esbuild = require('esbuild')

esbuild.build({
    entryPoints: ['./src/WebService/Index.bs.js' ],
    logLevel: 'info',
    bundle: true,
    minify: true,
    sourcemap: true,
    define: {
        'process.env.NODE_ENV': 'production',
        'production': true,
    },
    outfile: './src/Server/public/dist/index.js'
})
