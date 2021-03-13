open Relude.Globals;
open ReOrga;
open ReactUtils;

module Styles = {
  open Css;

  let tagsWrapper =
    style([
      listStyleType(none),
      margin(zero),
      marginTop(rem(0.75)),
      padding(zero),
      display(`inlineFlex),
      border(px(2), `solid, hex("e3ddd3")),
      borderRadius(px(5)),
    ]);
  let tagsItem =
    style([
      padding2(~v=px(2), ~h=px(5)),
      fontWeight(medium),
      fontSize(rem(0.9)),
      selector("& + &", [borderLeft(px(2), `solid, hex("e3ddd3"))]),
      selector(":hover", [backgroundColor(hex("e3ddd3"))]),
      cursor(`default),
    ]);
};

let renderTags = xs =>
  <ul className=Styles.tagsWrapper>
    {Belt.Array.mapWithIndex(
       xs,
       (i, x) => {
         let key = string_of_int(i);
         <li key className=Styles.tagsItem> {s(x)} </li>;
       },
     )
     |> React.array}
  </ul>;
