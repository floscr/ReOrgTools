const path = require('path')
const fs = require('fs')
const esbuild = require('esbuild')
const ssri = require('ssri')
const ejs = require('ejs')

const isUnixHiddenPath = function (path) {
    return (/(\/|^)\.[^\/\.]/g).test(path);
};

// Config
const srcDir = './src/WebService';
const distDir = './src/Server/public/dist';

const entryPoints = ['Index.bs.js'].map(x => path.join(srcDir, x))
const outFile = path.join(distDir, 'index.js');

const clean = () => {
    const files = fs
        .readdirSync(distDir)
        .filter(x => !isUnixHiddenPath(x))
        .map(x => path.join(distDir, x))
        .forEach(x => fs.unlinkSync(x))
}

const makeHtml = async () => {
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
