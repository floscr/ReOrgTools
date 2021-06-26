open ReactUtils;
open Relude.Globals;
open State;
open Dialogs__Style;

module Styles = {
  open Css;
  open FixedTheme;

  let root = style([padding(Spacing.large)]);
};

let id = "Dialogs__AgendaCreate";

type state = {agenda: State__Settings.Agenda.t};

let initialState = {
  agenda: {
    files: [||],
    fields: [|ViewType(Calendar)|],
  },
};

type action =
  | NoOp;

let reducer =
    (state: state, action: action): ReludeReact.Reducer.update(action, state) =>
  switch (action) {
  | NoOp => NoUpdate
  };

[@react.component]
let make = () => {
  open Radix;

  let (state, _send) = ReludeReact.Reducer.useReducer(reducer, initialState);

  <RoundedDialogWrapper bindings=[||] key=id hasPadding=true>
    <form>
      <RadioGroup.Root>
        <Label.Root>
          <RadioGroup.Item value="cat" name="string">
            <RadioGroup.Indicator />
          </RadioGroup.Item>
          {"Foo" |> s}
        </Label.Root>
        <Label.Root>
          <RadioGroup.Item value="cat2" name="string">
            <RadioGroup.Indicator />
          </RadioGroup.Item>
          {"Foo" |> s}
        </Label.Root>
      </RadioGroup.Root>
    </form>
  </RoundedDialogWrapper>;
};
