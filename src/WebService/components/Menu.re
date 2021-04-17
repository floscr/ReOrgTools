open Relude.Globals;
open ReactUtils;

module Styles = {
  open Css;
  open FixedTheme;

  let paddingSize = Spacing.medium;
  let inputSize = px(50);

  let root =
    style([
      display(`flex),
      flexDirection(column),
      height(vh(90.)),
      width(vw(90.)),
      maxWidth(px(460)),
      maxHeight(px(380)),
    ]);

  let input =
    style([
      border(zero, none, currentColor),
      borderBottom(px(1), solid, var(ThemeKeys.grey10)),
      padding(paddingSize),
      fontSize(Fonts.regular),
      selector(":focus", [outlineStyle(none)]),
    ]);

  let resultsRoot =
    style([
      overflowY(scroll),
      flexGrow(1.),
      display(`flex),
      flexDirection(column),
      backgroundColor(var(ThemeKeys.grey00)),
    ]);

  let resultsList =
    style([
      listStyleType(none),
      flexGrow(1.),
      margin(zero),
      padding(zero),
      borderRight(px(1), solid, var(ThemeKeys.grey10)),
    ]);

  let resultsItem =
    style([
      fontSize(Fonts.small),
      fontWeight(medium),
      color(var(ThemeKeys.grey40)),
      padding2(~h=paddingSize, ~v=Spacing.xsmall),
      borderBottom(px(1), solid, var(ThemeKeys.grey10)),
      backgroundColor(var(ThemeKeys.bgColor)),
      selector(
        ":hover",
        [cursor(pointer), backgroundColor(var(ThemeKeys.grey15))],
      ),
    ]);

  let resultsItemFocused =
    style([
      backgroundColor(var(ThemeKeys.grey15)),
      boxShadow(
        Shadow.box(~x=px(2), ~inset=true, var(ThemeKeys.accentMain)),
      ),
    ]);
};

let selectPrev = (~bounds: int, ~index: option(int)) =>
  index
  |> Option.map(x => Int.subtract(x, 1))
  |> Option.reject(a => Int.lessThan(a, 0))
  |> Option.orElse(~fallback=Some(bounds));

let selectNext = (~bounds: int, ~index: option(int)) =>
  index
  |> Option.map(Int.add(1))
  |> Option.reject(a => Int.greaterThan(a, bounds))
  |> Option.orElse(~fallback=Some(0));

module Item = {
  [@react.component]
  let make = (~isSelected: bool, ~value: string, ~onClick: unit => unit) => {
    let rootRef: React.ref(Js.Nullable.t(Dom.element)) =
      React.useRef(Js.Nullable.null);

    let className =
      ClassName.pure(Styles.resultsItem)
      |> ClassName.condAppend(isSelected, Styles.resultsItemFocused)
      |> ClassName.unwrap;

    React.useEffect1(
      () => {
        if (isSelected) {
          ReactUtils.scrollIntoViewIfNeeded(rootRef);
        };

        None;
      },
      [|isSelected|],
    );

    <li
      ref={ReactDOMRe.Ref.domRef(rootRef)}
      className
      onClick={ReactUtils.omit(onClick)}>
      {value |> s}
    </li>;
  };
};

[@react.component]
let make =
    (
      ~onChange: ReactEvent.Form.t => unit,
      ~onKeyDown: ReactEvent.Keyboard.t => unit,
      ~placeholder: string,
      ~value: string,
      ~children,
    ) => {
  <div className=Styles.root>
    <input
      autoFocus=true
      className=Styles.input
      autoComplete="off"
      value
      onChange
      onBlur={e => e->ReactEvent.Synthetic.preventDefault}
      onKeyDown
      placeholder
    />
    <div className=Styles.resultsRoot>
      <ul className=Styles.resultsList> children </ul>
    </div>
  </div>;
};
