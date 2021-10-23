const webpack = require('webpack');
const path = require('path');
const MiniCssExtractPlugin = require('mini-css-extract-plugin');

module.exports = {
  mode: 'development',
  stats: 'errors-only',
  entry: './src/WebService/HotIndex.bs.js',
  devServer: {
    port: 8080,
    hot: true,
    inline: true,
    historyApiFallback: true,
    clientLogLevel: 'silent',
  },
  output: {
    path: path.join(__dirname, 'bundleOutput'),
    publicPath: '/',
    filename: 'index.js',
  },
  module: {
    rules: [
      {
        test: /\.org$/i,
        use: 'raw-loader',
      },
      {
        test: /\.css$/,
        use: ['style-loader', 'css-loader'],
      },
    ],
  },
};
