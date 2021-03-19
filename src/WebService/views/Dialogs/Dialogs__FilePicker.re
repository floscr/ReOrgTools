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

let id = "Dialogs__FilePicker";

type state = {
  options: array(string),
  query: string,
  selection: option(int),
};

let initialState = {options: [||], query: "", selection: Some(0)};

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
           let index =
             acc
             |> Array.last
             |> Option.map(((i, _, _)) => Int.add(1, i))
             |> Option.getOrElse(0);

           files
           |> Array.map(x => (index, workspace, x))
           |> Array.concat(acc);
         },
         [||],
       )
    |> Array.filter(((_, _, {name}: Shared__API__Workspaces.File.t)) =>
         name |> String.toLowerCase |> String.contains(~search=query)
       );
  React.Ref.setCurrent(boundsRef, results |> Array.length);

  let onSubmit = (~index=None, results) => {
    results
    |> Array.at(index |> Option.getOrElse(0))
    |> Option.flatMap(
         ((_, workspaceName, {name}: Shared__API__Workspaces.File.t)) =>
         workspaces
         |> List.map(((a, _)) => a)
         |> List.indexOfBy(String.eq, workspaceName)
         |> Option.map(i => (i, name))
       )
    |> Option.tap(((i, name)) => {
         let name = Filename.chop_extension(name);
         close();
         ReasonReactRouter.push({j|/file/$i/$name|j});
       });
  };

  let selectNext = () => SelectNext(boundsRef |> React.Ref.current) |> send;
  let selectPrev = () => SelectPrev(boundsRef |> React.Ref.current) |> send;
  let onChange = event => {
    send(ChangeQuery(event->ReactEvent.Form.target##value));
  };
  let onKeyDown = event => {
    switch (event |> ReactEvent.Keyboard.key) {
    | "Enter" =>
      onSubmit(~index=state.selection, results) |> ignore;
      ReactEvent.Keyboard.preventDefault(event);
    | "Escape" => close()
    | _ => ()
    };
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
        [|"ctrl+n", "down"|],
        _ => {
          SelectNext(boundsRef |> React.Ref.current) |> send;
          false;
        },
      ),
      (
        [|"ctrl+p", "up"|],
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
      onKeyDown
      onBlur={e => e->ReactEvent.Synthetic.preventDefault}
      placeholder="Pick File"
    />
    <div className=Styles.resultsRoot>
      <ul className=Styles.resultsList>
        {results
         |> Array.mapWithIndex(
              ((_, _, {name}: Shared__API__Workspaces.File.t), i) => {
              let isSelected =
                state.selection |> Option.filter(Int.eq(i)) |> Option.isSome;
              <React.Fragment key={i |> Int.toString}>
                <Item
                  isSelected
                  onClick={_ => onSubmit(~index=Some(i), results) |> ignore}
                  value={name |> Filename.chop_extension}
                />
              </React.Fragment>;
            })
         |> React.array}
      </ul>
    </div>
  </div>;
};
