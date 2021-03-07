const path = require('path');

module.exports = {
  entry: './src/WebService/WebService.bs.js',
  mode: 'development',
  output: {
    path: path.join(__dirname, "bundleOutput"),
    filename: 'index.js',
  },
  devServer: {
    // hot: false,
    // inline: false,
    historyApiFallback: {
      index: 'index.html',
    },
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
