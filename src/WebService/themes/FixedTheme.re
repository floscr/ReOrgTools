module Fonts = {
  open Css;

  let small = rem(0.9);
  let regular = rem(1.);
  let large = rem(1.2);
};

module Spacing = {
  open Css;

  let mediumValue = 0.7;
  let negativeMediumValue = (-0.7);
  let smallValue = 0.55;
  let negativeSmallValue = (-0.55);

  let xxlarge = rem(1.75);
  let xlarge = rem(1.5);
  let large = rem(1.);
  let negativeMedium = rem(negativeMediumValue);
  let medium = rem(mediumValue);
  let small = rem(smallValue);
  let negativeSmall = rem(negativeSmallValue);
  let xsmall = rem(0.45);
  let xxsmall = rem(0.35);

  let scrollbarWidth = px(14);
};

module BorderRadius = {
  open Css;

  let xsmall = px(4);
  let small = px(5);
};

module Timing = {
  open Css;
  let hoverShort = 150;
};
