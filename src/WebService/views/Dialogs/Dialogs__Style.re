module Styles = {
  open Css;

  let roundedDialog =
    style([
      position(absolute),
      backgroundColor(white),
      borderRadius(FixedTheme.BorderRadius.small),
      overflow(hidden),
    ]);
};

module Wrapper = {
  [@react.component]
  let make = (~bindings, ~className=Styles.roundedDialog, ~children) =>
    <ComboKeysWrapper bindings>
      <div onClick=ReactEvent.Mouse.stopPropagation className> children </div>
    </ComboKeysWrapper>;
};

module RoundedDialogWrapper = {
  [@react.component]
  let make = (~bindings, ~children) =>
    <Wrapper bindings className=Styles.roundedDialog> children </Wrapper>;
};
