module Styles = {
  open Css;

  let ul = style([listStyleType(none), margin(zero), padding(zero)]);
  let li = style([margin(zero)]);

  let header =
    style([
      fontWeight(medium),
      color(var(ThemeKeys.baseGray10)),
      padding(FixedTheme.Spacing.large),
      fontSize(rem(1.)),
    ]);

  let button =
    style([
      padding2(~v=FixedTheme.Spacing.xsmall, ~h=FixedTheme.Spacing.large),
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
