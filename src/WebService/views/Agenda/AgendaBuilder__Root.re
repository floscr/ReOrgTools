open Relude.Globals;
open ReOrga;
open OrgDocument__Utils;
open ReactUtils;
open State__Settings;

module Styles = {
  open Css;
  open FixedTheme;

  let root = style([display(`flex)]);

  let textArea = style([width(pct(100.))]);

  let wrapper =
    style([
      width(pct(100.)),
      borderLeft(px(1), `solid, var(ThemeKeys.baseGray3)),
    ]);
};

type state = Agenda.t;

let initialState: Agenda.t = {files: [||], fields: [||]};

type action =
  | UpdateSettings(state)
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | UpdateSettings(state) => Update(state)
  | NoOp => NoUpdate
  };

module FilePicker = {
  let makeKey = ({id, workspace}: Agenda.File.t) => {j|$workspace-$id|j};

  let make = (files: Agenda.filesT) => {
    <div>
      <ul>
        {files
         |> Array.map((({id, workspace}: Agenda.File.t) as file) =>
              <li key={file |> makeKey}> {workspace ++ id |> s} </li>
            )
         |> React.array}
      </ul>
      <button> {"Add file" |> s} </button>
    </div>;
  };
};

[@react.component]
let make = () => {
  let (value, setValue) =
    React.useState(() =>
      initialState
      |> State__Settings.Encode.encodeAgendasJson
      |> (x => Js.Json.stringifyWithSpace(x, 2))
    );
  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  Radix.(
    <div className=Styles.root>
      <ScrollArea.Wrapper>
        <textarea
          rows=30
          className=Styles.textArea
          onChange={event => {
            open ReactEvent.Form;

            persist(event);

            let value = target(event)##value;

            setValue(value);

            value
            |> State__Settings.Decode.decodeAgendaJson
            |> Result.tapError(x => Js_console.error2("Invalid Data", x))
            |> Result.tap(x => UpdateSettings(x) |> send)
            |> ignore;
          }}
          value
        />
      </ScrollArea.Wrapper>
      <div className=Styles.wrapper>
        <ScrollArea.Wrapper> {"Hello world" |> s} </ScrollArea.Wrapper>
      </div>
    </div>
  );
};
