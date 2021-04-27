const path = require('path');
const isProduction = process.env.NODE_ENV === 'production';

const devConfig = {
  mode: 'development',
  devServer: {
    port: 8080,
    hot: true,
    inline: true,
    historyApiFallback: true,
  },
}

const prodConfig = {
  mode: 'productions',
}

const extendConfig = isProduction ? devConfig : prodConfig;

module.exports = {
  ...extendConfig,

  stats: 'errors-only',

  entry: './src/WebService/Index.bs.js',

  output: {
    path: path.join(__dirname, "bundleOutput"),
    publicPath: '/',
    filename: 'index.js',
  },

  module: {
    rules: [
      {
        test: /\.org$/i,
        use: 'raw-loader',
      },
    ],
  },
};
