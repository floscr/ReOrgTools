module BaseTheme = {
  open Css;
  let bgColor = hex("fff");
};

module LightTheme = {
  open Css;
  include BaseTheme;
  let bgColor = hex("f0eeed");
  let textColor = hex("101010");
};
