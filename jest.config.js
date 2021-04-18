const transformIgnore = [
  '(@.*/)?(bs-.*|reason-.*)/)',
  'relude',
];

module.exports = {
  verbose: true,
  rootDir: __dirname,
  testMatch: [
    "<rootDir>/__tests__/*_test.bs.js"
  ],
  transformIgnorePatterns: [
    `node_modules/(?!(${transformIgnore.join('|')}).+\\.js$`,
  ],
};
