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

module Validation = {
  type err =
    | JsonDecodeError
    | DecodeError(Decode_AsResult_OfParseError.ParseError.failure);

  type succ =
    | Ok;

  type t = result(succ, err);

  let toString = (x: t) =>
    x
    |> Result.fold(
         err =>
           (
             switch (err) {
             | JsonDecodeError => "Failed to decode json"
             | DecodeError(
                 (x: Decode_AsResult_OfParseError.ParseError.failure),
               ) =>
               Decode.ParseError.failureToDebugString(x)
             }
           )
           |> (x => "Error: " ++ x),
         _x => "Success",
       );
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
      |> Result.fold(
           err => Error(Validation.DecodeError(err)),
           _ => Ok(Validation.Ok),
         )
    );

  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  <div className=Styles.root>
    <Radix.ScrollArea.Wrapper>
      <textarea
        value
        rows=30
        className=Styles.textArea
        onChange={event => {
          open ReactEvent.Form;

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
          |> Option.fold(Error(Validation.JsonDecodeError), x =>
               x
               |> Result.fold(
                    err => Error(Validation.DecodeError(err)),
                    _ => Ok(Validation.Ok),
                  )
             )
          |> (x => setValidation(_ => x));
        }}
      />
      <pre> {validation |> Validation.toString |> s} </pre>
    </Radix.ScrollArea.Wrapper>
    <div className=Styles.wrapper>
      <Radix.ScrollArea.Wrapper>
        {"Hello world" |> s}
      </Radix.ScrollArea.Wrapper>
    </div>
  </div>;
};
