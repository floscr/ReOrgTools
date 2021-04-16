module Fonts = {
  open Css;

  let small = rem(0.9);
  let regular = rem(1.);
  let large = rem(1.2);
};

module Spacing = {
  open Css;

  let xxlarge = rem(1.75);
  let xlarge = rem(1.5);
  let large = rem(1.);
  let medium = rem(0.7);
  let small = rem(0.55);
  let xsmall = rem(0.45);

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
