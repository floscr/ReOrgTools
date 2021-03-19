module BaseTheme = {
  open Css;
  let bgColor = hex("fff");

  let grey1 = hex("f5f5f5");
  let grey2 = hex("E0E0E0");
  let grey4 = hex("333333");

  let sidebarBgColor = grey1;
};

module LightTheme = {
  open Css;
  include BaseTheme;
  let bgColor = hex("fefefe");
  let textColor = hex("333");
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

  let small = px(5);
};

module Timing = {
  open Css;
  let hoverShort = 150;
};
