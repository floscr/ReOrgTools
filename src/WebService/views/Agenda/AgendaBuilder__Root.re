open Relude.Globals;
open ReOrga;
open OrgDocument__Utils;
open ReactUtils;
open State__Settings;

module Styles = {
  open Css;
  open FixedTheme;

  let root = style([display(`flex)]);

  let wrapper =
    style([
      width(pct(100.)),
      borderLeft(px(1), `solid, var(ThemeKeys.baseGray3)),
    ]);
};

type state = Agenda.t;

let initialState: Agenda.t = {files: [||], fields: [||]};

type action =
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | NoOp => NoUpdate
  };

module FilePicker = {};

[@react.component]
let make = () => {
  // Move to component
  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  Radix.(
    <div className=Styles.root>
      <ScrollArea.Wrapper>
        <div className=Styles.wrapper> {"Hello World" |> s} </div>
      </ScrollArea.Wrapper>
      <div className=Styles.wrapper>
        <ScrollArea.Wrapper> {"Hello world" |> s} </ScrollArea.Wrapper>
      </div>
    </div>
  );
};
