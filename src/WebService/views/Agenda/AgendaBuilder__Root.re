open Relude.Globals;
open ReOrga;
open OrgDocument__Utils;
open ReactUtils;

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

module Agenda = State__Settings.Agenda;

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

  let (validation, setValidation) =
    React.useState(() =>
      initialState
      |> State__Settings.Encode.encodeAgendasJson
      |> State__Settings.Decode.decodeAgendaJson
    );

  Js.log(validation);

  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  <div className=Styles.root>
    <Radix.ScrollArea.Wrapper>
      <textarea
        value
        rows=30
        className=Styles.textArea
        onChange={event => {
          open ReactEvent.Form;
          module Decode = Decode.AsResult.OfParseError;

          persist(event);

          let value = target(event)##value;

          setValue(value);

          value
          |> Json.parse
          |> Option.map(x =>
               x
               |> State__Settings.Decode.decodeAgendaJson
               |> Result.tap(x => UpdateSettings(x) |> send)
               |> Result.tapError(err =>
                    Decode.ParseError.failureToDebugString(err) |> Js.log
                  )
             )
          |> Option.tap(x => setValidation(_ => x))
          |> ignore;
        }}
      />
      {validation
       |> Result.fold(Decode.ParseError.failureToDebugString, _ => "Success")
       |> s}
    </Radix.ScrollArea.Wrapper>
    <div className=Styles.wrapper>
      <Radix.ScrollArea.Wrapper>
        {"Hello world" |> s}
      </Radix.ScrollArea.Wrapper>
    </div>
  </div>;
};
