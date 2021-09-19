open Relude.Globals;
open ReOrga;
open OrgDocument__Utils;
open ReactUtils;
open State;

module Styles = {
  open Css;
  open FixedTheme;

  let root =
    style([
      width(pct(100.)),
      height(pct(100.)),
      padding(Spacing.large),
      display(`flex),
      children([flexGrow(1.), flex(`num(1.))]),
    ]);

  let textArea = style([width(pct(100.))]);
};

module Agenda = State__Settings.Agenda;

type state = Agenda.t;

let initialState: Agenda.t = {
  id: "Example",
  files: [|{id: "inbox", workspace: 0}, {id: "work", workspace: 1}|],
  fields: [|Layout(Types__Org.Layout.SimpleTodo)|],
  timerange: Some(Ok(Agenda.Time.CurrentOnly(Agenda.Time.CurrentWeek))),
  tags: Some([|Agenda.Filter.Add("WORK")|]),
  todos: None,
  reverse: Some(true),
};
/* let initialState: Agenda.t = {files: [||], fields: [||]}; */

type action =
  | UpdateSettings(state)
  | NoOp;

let reducer =
    (_state: state, action: action)
    : ReludeReact.Reducer.update(action, state) =>
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
              <li key={file |> makeKey}>
                {workspace |> String.fromInt |> String.concat(id) |> s}
              </li>
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
    | DecodeError(State__Settings.Decode.R.error);

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
             | DecodeError(err) =>
               State__Settings.Decode.failureToDebugString(err)
             }
           )
           |> (x => "Error: " ++ x),
         _x => "Success",
       );
};

[@react.component]
let make = () => {
  let dispatch = State.Store.useDispatch();

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

  let isValid = validation |> Result.isOk;

  let (state, send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  let onSubmit = _ =>
    switch (isValid) {
    | true =>
      SettingsAction(State__Settings.SaveAgendaState(state)) |> dispatch
    | _ => ()
    };

  let {files, fields, timerange, tags}: state = state;

  let layoutType =
    fields
    |> Array.find(
         fun
         | Agenda.Layout(_) => true,
       )
    |> Option.map(
         fun
         | Agenda.Layout(x) => x,
       );

  <div className=Styles.root>
    <Radix.ScrollArea.Wrapper>
      <ReactUtils.Form.Wrapper onSubmit={onSubmit |> eventPrevent}>
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
            |> ReactUtils.JsonParser.parseIgnoreTrailing
            |> Option.map(x =>
                 x
                 |> State__Settings.Decode.decodeAgendaJson
                 |> Result.tap(x => UpdateSettings(x) |> send)
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
        <ReactUtils.Form.SubmitButton disabled={!isValid} />
      </ReactUtils.Form.Wrapper>
    </Radix.ScrollArea.Wrapper>
    <Radix.ScrollArea.Wrapper>
      <Agenda__Root.OrgWrapper agenda=state />
    </Radix.ScrollArea.Wrapper>
  </div>;
};
