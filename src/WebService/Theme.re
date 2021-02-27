module BaseTheme = {
  open Css;
  let bgColor = hex("fff");
};

module LightTheme = {
  open Css;
  include BaseTheme;
  let bgColor = hex("fefefe");
  let textColor = hex("333");
};
