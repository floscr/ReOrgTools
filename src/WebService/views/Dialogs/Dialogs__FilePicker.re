open ReactUtils;
open Relude.Globals;
open State;

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
      overflow(hidden),
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

  let resultsItem = (~isSelected) =>
    style([
      padding2(~h=paddingSize, ~v=Spacing.xsmall),
      borderBottom(px(1), solid, var(ThemeKeys.grey10)),
      backgroundColor(
        isSelected ? var(ThemeKeys.blue) : var(ThemeKeys.bgColor),
      ),
    ]);
};

let id = "Dialogs__FilePicker";

type state = {
  options: array(string),
  query: string,
  selection: option(int),
};

let initialState = {options: [||], query: "", selection: None};

type action =
  | ChangeQuery(string)
  | SelectNext(int)
  | SelectPrev(int)
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | ChangeQuery(query) => Update({...state, query})
  | SelectNext(bounds) =>
    Update({
      ...state,
      selection:
        state.selection
        |> Option.map(Int.add(1))
        |> Option.reject(a => Int.greaterThan(a, bounds))
        |> Option.orElse(~fallback=Some(0)),
    })
  | SelectPrev(bounds) =>
    Update({
      ...state,
      selection:
        state.selection
        |> Option.map(x => Int.subtract(x, 1))
        |> Option.reject(a => Int.lessThan(a, 0))
        |> Option.orElse(~fallback=Some(bounds)),
    })

  | NoOp => NoUpdate
  };

[@react.component]
let make = (~close) => {
  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);
  let query = state.query |> String.toLowerCase;
  let workspaces = Store.useSelector(Selector.Workspaces.workspaces);

  // Needs to be mutable otherwise the shortcuts point to an outdated reference
  let boundsRef = React.useRef(0);
  let results =
    workspaces
    |> List.foldLeft(
         (acc, (workspace, files)) => {
           files |> Array.map(x => (workspace, x)) |> Array.concat(acc)
         },
         [||],
       )
    |> Array.filter(((a, {name}: Shared__API__Workspaces.File.t)) =>
         name |> String.toLowerCase |> String.contains(~search=query)
       );
  React.Ref.setCurrent(boundsRef, results |> Array.length);

  let selectNext = () => SelectNext(boundsRef |> React.Ref.current) |> send;
  let selectPrev = () => SelectPrev(boundsRef |> React.Ref.current) |> send;
  let onChange = event => {
    send(ChangeQuery(event->ReactEvent.Form.target##value));
  };

  let combokeys: ref(option(Combokeys.t)) = ref(None);
  let getCombokeys = () =>
    switch (combokeys^) {
    | None =>
      let keys =
        Combokeys.init(
          Webapi.Dom.document |> Webapi.Dom.Document.documentElement,
        )
        |> Combokeys.initGlobalBindPlugin;
      combokeys := Some(keys);
      keys;
    | Some(x) => x
    };
  let bindShortcuts = _ => {
    let keys = getCombokeys();
    [|
      (
        [|"ctrl+k", "esc"|],
        _ => {
          close();
          false;
        },
      ),
      (
        [|"ctrl+n"|],
        _ => {
          SelectNext(boundsRef |> React.Ref.current) |> send;
          false;
        },
      ),
      (
        [|"ctrl+p"|],
        _ => {
          SelectPrev(boundsRef |> React.Ref.current) |> send;
          false;
        },
      ),
    |]
    |> Array.forEach(((a, fn)) => Combokeys.bindGlobalArray(a, fn, keys));
  };
  let detachShortcuts = () => {
    getCombokeys() |> Combokeys.detach();
    combokeys := None;
  };
  React.useEffect0(_ => {
    bindShortcuts();
    Some(() => detachShortcuts());
  });

  <div className=Styles.root>
    <input
      autoFocus=true
      className=Styles.input
      autoComplete="off"
      name=id
      value={state.query}
      onChange
      onBlur={e => e->ReactEvent.Synthetic.preventDefault}
      placeholder="Pick File"
    />
    <div className=Styles.resultsRoot>
      <ul className=Styles.resultsList>
        {results
         |> Array.mapWithIndex(
              ((a, {name}: Shared__API__Workspaces.File.t), i) => {
              let isSelected =
                state.selection |> Option.filter(Int.eq(i)) |> Option.isSome;
              <li
                key={i |> Int.toString}
                className={Styles.resultsItem(~isSelected)}>
                {name |> Filename.chop_extension |> s}
              </li>;
            })
         |> React.array}
      </ul>
    </div>
  </div>;
};
