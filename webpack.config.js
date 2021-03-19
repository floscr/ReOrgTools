const path = require('path');

module.exports = {
  entry: './src/WebService/Index.bs.js',
  mode: 'development',
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
