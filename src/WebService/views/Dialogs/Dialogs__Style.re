module Styles = {
  open Css;
  open FixedTheme;

  let roundedDialog = hasPadding =>
    style([
      position(absolute),
      backgroundColor(white),
      borderRadius(FixedTheme.BorderRadius.small),
      overflow(hidden),
      padding(hasPadding ? Spacing.large : zero),
    ]);
};

module Wrapper = {
  [@react.component]
  let make = (~bindings, ~className, ~children) =>
    <ComboKeysWrapper bindings>
      <div onClick=ReactEvent.Mouse.stopPropagation className> children </div>
    </ComboKeysWrapper>;
};

module RoundedDialogWrapper = {
  [@react.component]
  let make = (~bindings, ~hasPadding=false, ~children) =>
    <Wrapper bindings className={Styles.roundedDialog(hasPadding)}>
      children
    </Wrapper>;
};
