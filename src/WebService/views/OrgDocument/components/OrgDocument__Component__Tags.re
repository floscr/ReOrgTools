open Relude.Globals;
open ReOrga;
open ReactUtils;

module Styles = {
  open Css;
  open FixedTheme;

  let tagsWrapper =
    style([
      listStyleType(none),
      margin(zero),
      padding(zero),
      display(`inlineFlex),
    ]);
  let tagsItem =
    style([
      fontWeight(medium),
      fontSize(rem(0.9)),
      textTransform(lowercase),
      /* selector("& + &", [borderLeft(px(2), `solid, hex("e3ddd3"))]), */
      selector(
        "&:after",
        [
          unsafe("content", {j|", "|j}),
          display(inlineBlock),
          width(em(0.5)),
        ],
      ),
      selector(":last-child:after", [display(none)]),
      hover([color(var(ThemeKeys.accentMain)), cursor(`pointer)]),
    ]);
};

let renderTags = xs =>
  <ul className=Styles.tagsWrapper>
    {Belt.Array.mapWithIndex(xs, (i, x) =>
       <li key={Int.toString(i)} className=Styles.tagsItem> {s(x)} </li>
     )
     |> React.array}
  </ul>;
