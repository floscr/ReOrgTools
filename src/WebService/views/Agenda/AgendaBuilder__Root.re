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

module Agenda = Types__Agendas.Agenda;

type state = Agenda.t;

let initialState: Agenda.t = {
  id: "Example",
  files: [|{id: "inbox", workspace: 0}, {id: "work", workspace: 1}|],
  fields: [|Layout(Types__Org.Layout.SimpleTodo)|],
  timerange:
    Some(
      Ok(Types__Agendas.Time.CurrentOnly(Types__Agendas.Time.CurrentWeek)),
    ),
  tags: None,
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

module Validation = {
  type err =
    | JsonDecodeError
    | DecodeError(DecodeUtils.R.error);

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
             | DecodeError(err) => DecodeUtils.failureToDebugString(err)
             }
           )
           |> (x => "Error: " ++ x),
         _x => "Success",
       );
};

[@react.component]
let make = (~id: option(string)=?) => {
  let dispatch = State.Store.useDispatch();

  let initialStateOrEditingState =
    id
    |> Option.flatMap(editingId =>
         Store.useSelector(Selector.Settings.agendas)
         |> Array.find(({id}: Types__Agendas.Agenda.t) =>
              String.eq(id, editingId)
            )
       )
    |> Option.getOrElse(initialState)
    |> Utils.log;

  let (value, setValue) =
    React.useState(() =>
      initialStateOrEditingState
      |> Types__Agendas__Translators.Agendas.Encode.make
      |> (x => Js.Json.stringifyWithSpace(x, 2))
    );

  let (validation, setValidation) =
    React.useState(() =>
      initialState
      |> Types__Agendas__Translators.Agendas.Encode.make
      |> Types__Agendas__Translators.Agendas.Decode.make
      |> Result.fold(
           err => Error(Validation.DecodeError(err)),
           _ => Ok(Validation.Ok),
         )
    );

  let isValid = validation |> Result.isOk;

  let (state, send) =
    ReludeReact.Reducer.useReducer(reducer, initialStateOrEditingState);

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
         | Types__Agendas.Field.Layout(_) => true,
       )
    |> Option.map(
         fun
         | Types__Agendas.Field.Layout(x) => x,
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
                 |> Types__Agendas__Translators.Agendas.Decode.make
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
