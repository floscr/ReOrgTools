const path = require('path');
const isProduction = process.env.NODE_ENV === 'production';

const config = isProduction ? require('./webpack.config.production.js') : {};

module.exports = {
  // ...config,
  stats: 'errors-only',
  entry: './src/WebService/Index.bs.js',

  mode: isProduction ? 'production' : 'development',

  output: {
    path: path.join(__dirname, "bundleOutput"),
    publicPath: '/',
    filename: 'index.js',
  },
  devServer: {
    port: 8080,
    hot: true,
    inline: true,
    historyApiFallback: true,
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
