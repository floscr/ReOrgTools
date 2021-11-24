open ReactUtils;
open Relude.Globals;
open State;

module Styles = {
  open Css;
  let innerSpacing = FixedTheme.SpacingNew.px20;

  let root =
    style([
      borderRight(px(1), `solid, var(ThemeKeys.baseGray7)),
      backgroundColor(var(ThemeKeys.baseGray3)),
      overflow(hidden),
    ]);

  let scrollviewWrapper = style([padding(innerSpacing)]);

  let backIcon =
    style([position(absolute), top(innerSpacing), right(innerSpacing)]);
};

type mode =
  | Workspaces
  | Outline;

type state = {
  mode,
  x: int,
};

let initialState = {mode: Workspaces, x: 1};

type action =
  | SwitchMode(mode);

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | SwitchMode(mode) => Update({...state, mode})
  };

[@react.component]
let make = (~id) => {
  let files = Store.useSelector(Selector.OrgDocuments.files);
  let file = id |> Option.flatMap(x => StringMap.get(x, files));
  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  let onFileClick = () => send(SwitchMode(Outline));
  let onBackClick = _ => send(SwitchMode(Workspaces));

  <div className=Styles.root>
    <Radix.ScrollArea.Wrapper>
      <div className=Styles.scrollviewWrapper>
        <Sidebar__Agendas />
        <Sidebar__Bookmarks />
        {switch (state.mode, file) {
         | (Outline, Some(Types__Files.File.Fetched({ast}))) =>
           <>
             <button onClick=onBackClick> {"<- Go back" |> s} </button>
             <Outline ast />
           </>
         | _ => <Workspaces__Component onFileClick />
         }}
      </div>
    </Radix.ScrollArea.Wrapper>
  </div>;
};
