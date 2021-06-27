open Relude.Globals;
open ReOrga;
open OrgDocument__Utils;
open ReactUtils;

module Styles = {
  open Css;
  open FixedTheme;

  let root = style([height(px(4000)), backgroundColor(beige)]);

  let wrapper = style([height(px(300))]);
};

[@react.component]
let make = () => {
  Radix.(
    <div className=Styles.wrapper>
      <ScrollArea.Wrapper>
        <div className=Styles.root> {"Hello world" |> s} </div>
      </ScrollArea.Wrapper>
    </div>
  );
};
