let makeKey = (level, index) => {j|$level-$index|j};

let wrapWithKey = (level, index, children) => {
  <React.Fragment key={makeKey(level, index)}> children </React.Fragment>;
};

module Wrappers = {
  module Styles = {
    open Css;
    open FixedTheme;

    let mainWrapper =
      style([
        padding2(~h=Spacing.xxlarge, ~v=Spacing.xlarge),
        flexGrow(1.),
        flexShrink(1.),
        maxWidth(px(860)),
        wordWrap(breakWord),
        width(pct(100.)),
        margin2(~v=zero, ~h=auto),
      ]);
  };

  let paddedWrapper = children =>
    <div className=Styles.mainWrapper> children </div>;
};
