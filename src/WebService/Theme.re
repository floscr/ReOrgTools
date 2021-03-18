module BaseTheme = {
  open Css;
  let bgColor = hex("fff");

  let grey1 = hex("e3ddd3");
};

module LightTheme = {
  open Css;
  include BaseTheme;
  let bgColor = hex("fefefe");
  let textColor = hex("333");
};

module Spacing = {
  open Css;

  let large = rem(1.);
  let medium = rem(0.7);
  let small = rem(0.55);
};

module BorderRadius = {
  open Css;

  let small = px(5);
};
