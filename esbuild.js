const path = require('path')
const fs = require('fs')
const esbuild = require('esbuild')
const ssri = require('ssri')
const ejs = require('ejs')

// Config

const srcDir = './src/WebService';
const distDir = './src/Server/public/dist';
const entryPoints = ['Index.bs.js'].map(x => path.join(srcDir, x))
const outFile = path.join(distDir, 'index.js');

// Plugins

const makeHtml = () => {
    const replacements = {
        jsSrc: '/dist/index.js',
        jsIntegrity: ssri.fromData(
            fs.readFileSync(outFile),
            { algorithms: ['sha256'] }
        ),
    };

    ejs.renderFile(path.join(srcDir, 'index.ejs'), replacements, {}, (err, str) => {
        if (err) console.error(err)
        fs.writeFileSync(path.join(distDir, '../index.html'), str)
    })
}

// Main

const build = async () => {
    await esbuild.build({
        entryPoints,
        logLevel: 'info',
        bundle: true,
        minify: true,
        sourcemap: 'external',
        outfile: outFile,
    })

    makeHtml();
}
build();
