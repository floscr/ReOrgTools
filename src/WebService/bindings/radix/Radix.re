module Label = {
  open Radix__Label;

  module Styles = {
    open Css;
    open FixedTheme;

    let label =
      style([
        display(`flex),
        alignItems(center),
        children([marginRight(Spacing.small)]),
        lastChild([marginRight(zero)]),
      ]);
  };

  module Root = {
    [@react.component]
    let make = (~children: React.element, ~className=Styles.label) =>
      <Label.Root className> children </Label.Root>;
  };
};

module RadioGroup = {
  open Radix__RadioGroup;

  module Styles = {
    open Css;
    open FixedTheme;

    let item =
      style([
        width(px(15)),
        height(px(15)),
        border(px(1), `solid, var(ThemeKeys.baseGray5)),
        borderRadius(pct(50.)),
        display(`flex),
        alignItems(center),
        justifyContent(center),
        focus([
          unsafe("outline", "none"),
          borderWidth(px(2)),
          borderColor(var(ThemeKeys.focus)),
        ]),
      ]);

    let indicator =
      style([
        width(px(7)),
        height(px(7)),
        backgroundColor(var(ThemeKeys.focus)),
        borderRadius(pct(50.)),
      ]);
  };

  module Root = RadioGroup.Root;

  module Indicator = {
    [@react.component]
    let make = (~className=Styles.indicator) =>
      <RadioGroup.Indicator className />;
  };

  module Item = {
    [@react.component]
    let make = (~children, ~value, ~name, ~className=Styles.item) =>
      <RadioGroup.Item value name className> children </RadioGroup.Item>;
  };
};

module ScrollArea = {
  open Radix__ScrollArea;

  module Styles = {
    open Css;
    open FixedTheme;

    let scrollbarSize = 14;

    let root =
      style([
        width(pct(100.)),
        height(pct(100.)),
        selector(
          ":hover [data-orientation=\"vertical\"]",
          [opacity(1.), transitionDelay(0)],
        ),
      ]);

    let viewport =
      style([
        width(pct(100.)),
        height(pct(100.)),
        padding(Spacing.xlarge),
      ]);

    let scrollbar =
      style([
        display(`flex),
        padding(px(4)),
        transition(~duration=160, "background"),
        transition(~duration=160, "opacity"),
        backgroundColor(rgba(0, 0, 0, `num(0.))),
        opacity(0.),
        transitionDelay(500),
        hover([backgroundColor(rgba(0, 0, 0, `num(0.05)))]),
        selector(
          "&[data-orientation=\"vertical\"]",
          [width(px(scrollbarSize))],
        ),
        selector(
          "&[data-orientation=\"horizontal\"]",
          [flexDirection(column), height(px(scrollbarSize))],
        ),
      ]);
    let thumb =
      style([
        flex(`num(1.)),
        borderRadius(px(50)),
        backgroundClip(paddingBox),
        backgroundColor(rgba(0, 0, 0, `num(0.15))),
      ]);
    let corner = style([backgroundColor(black)]);
  };

  module Wrapper = {
    [@react.component]
    let make = (~children, ~scrollHideDelay=1200, ~_type="always") =>
      <ScrollArea.Root className=Styles.root _type scrollHideDelay>
        <ScrollArea.Viewport className=Styles.viewport>
          children
        </ScrollArea.Viewport>
        <ScrollArea.Scrollbar
          orientation_="horizontal" className=Styles.scrollbar>
          <ScrollArea.Thumb className=Styles.thumb />
        </ScrollArea.Scrollbar>
        <ScrollArea.Scrollbar
          orientation_="vertical" className={Styles.scrollbar ++ " foo"}>
          <ScrollArea.Thumb className=Styles.thumb />
        </ScrollArea.Scrollbar>
        <ScrollArea.Corner className=Styles.corner />
      </ScrollArea.Root>;
  };
};
