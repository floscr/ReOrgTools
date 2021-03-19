let makeKey = (level, index) => {j|$level-$index|j};

let wrapWithKey = (level, index, children) => {
  <React.Fragment key={makeKey(level, index)}> children </React.Fragment>;
};
