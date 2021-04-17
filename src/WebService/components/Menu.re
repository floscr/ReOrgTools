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

type state = {
  query: string,
  selection: option(int),
};

let initialState = {query: "", selection: Some(0)};

type action =
  | ChangeQuery(string)
  | SelectNext(int)
  | SelectPrev(int)
  | NoOp;

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

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | ChangeQuery(query) => Update({...state, query})
  | SelectNext(bounds) =>
    Update({
      ...state,
      selection: selectNext(~bounds, ~index=state.selection),
    })
  | SelectPrev(bounds) =>
    Update({
      ...state,
      selection: selectPrev(~bounds, ~index=state.selection),
    })

  | NoOp => NoUpdate
  };

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
      ~onSubmit: state => unit,
      ~onEscape: unit => unit,
      ~placeholder: string,
      ~items: state => array('a),
      ~renderItems: state => React.element,
    ) => {
  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);
  let query = state.query |> String.toLowerCase;

  let results = items(state);

  let onChange = event => {
    send(ChangeQuery(event->ReactEvent.Form.target##value));
  };

  let onKeyDown = event => {
    switch (event |> ReactEvent.Keyboard.key) {
    | "Enter" =>
      onSubmit(state);
      ReactEvent.Keyboard.preventDefault(event);
    | "Escape" => onEscape()
    | _ => ()
    };
  };

  let bindings = [|
    (
      [|"ctrl+n", "down"|],
      _ => {
        SelectNext(results |> Array.length) |> send;
        false;
      },
    ),
    (
      [|"ctrl+p", "up"|],
      _ => {
        SelectPrev(results |> Array.length) |> send;
        false;
      },
    ),
  |];

  <ComboKeysWrapper bindings key={results |> Array.length |> Int.toString}>
    <div className=Styles.root>
      <input
        autoFocus=true
        className=Styles.input
        autoComplete="off"
        value={state.query}
        onChange
        onBlur={e => e->ReactEvent.Synthetic.preventDefault}
        onKeyDown
        placeholder
      />
      <div className=Styles.resultsRoot>
        <ul className=Styles.resultsList> {renderItems(state)} </ul>
      </div>
    </div>
  </ComboKeysWrapper>;
};
