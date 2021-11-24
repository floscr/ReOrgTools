module Styles = {
  open Css;

  let ul = style([listStyleType(none), margin(zero), padding(zero)]);
  let li = style([margin(zero)]);

  let headerWrapper =
    style([
      display(`flex),
      flexGrow(1.),
      backgroundColor(var(ThemeKeys.baseGray1)),
      borderRadius(FixedTheme.BorderRadius.small),
      justifyContent(spaceBetween),
      alignItems(center),
    ]);

  let header =
    style([
      fontWeight(medium),
      color(var(ThemeKeys.baseGray10)),
      padding(FixedTheme.Spacing.large),
      fontSize(rem(1.)),
    ]);

  let focusStyle = [
    backgroundColor(var(ThemeKeys.focus)),
    color(var(ThemeKeys.bgColor)),
    borderColor(var(ThemeKeys.focus)),
    cursor(pointer),
  ];

  let plusButton =
    style([
      fontSize(rem(0.8)),
      color(var(ThemeKeys.baseGray10)),
      marginRight(FixedTheme.Spacing.xsmall),
      border(px(1), `solid, var(ThemeKeys.baseGray4)),
      lineHeight(`abs(0.)),
      borderRadius(FixedTheme.BorderRadius.xsmall),
      hover(focusStyle),
      focus(focusStyle),
    ]);

  let optionsButton =
    style([color(var(ThemeKeys.baseGray10)), lineHeight(`abs(0.))]);

  let button =
    style([
      padding2(~v=FixedTheme.Spacing.xsmall, ~h=FixedTheme.Spacing.large),
      display(flexBox),
      justifyContent(spaceBetween),
      alignItems(center),
      lineHeight(`abs(1.)),
      borderRadius(px(50)),
      cursor(pointer),
      width(pct(100.)),
      fontWeight(medium),
      textAlign(`left),
      transition(~duration=FixedTheme.Timing.hoverShort, "all"),
      selector(":hover", [backgroundColor(var(ThemeKeys.baseGray3))]),
      selector(
        ":focus",
        [
          outlineStyle(none),
          borderRadius(px(50)),
          boxShadow(
            Shadow.box(~spread=px(3), ~inset=true, rgba(0, 0, 0, pct(7.))),
          ),
        ],
      ),
    ]);
};
