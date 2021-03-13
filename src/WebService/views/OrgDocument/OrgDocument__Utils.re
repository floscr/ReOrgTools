let makeKey = (level, index) => {j|$level-$index|j};

let wrapWithKey = (level, index, xs) => {
  <React.Fragment key={makeKey(level, index)}> xs </React.Fragment>;
};
