open Orga;
open ReactUtils;

let orga =
  Org.parseOrga(
    {j|* TODO Headline :FOO:FAA:
** Child 1

Content

- Foo
- Bar
- Baz

1. Foo
2. Bar
3. Baz

*** Child2

|j},
  );

module Heading = {
  [@react.component]
  let make = (~level: int, ~children) => {
    switch (level) {
    | 1 => <h1> children </h1>
    | 2 => <h2> children </h2>
    | 3 => <h3> children </h3>
    | 4 => <h4> children </h4>
    | 5 => <h5> children </h5>
    | _ => children
    };
  };
};

let renderHeadline = x =>
  switch (x) {
  | Headline({content, level}) => <Heading level> {s(content)} </Heading>
  | _ => React.null
  };

let wrapWithKey = (level, index, children) => {
  let key = {j|$level-$index|j};
  <React.Fragment key> children </React.Fragment>;
};

let rec renderParagraphs = xs => {
  Belt.Array.mapWithIndex(xs, (i, x) => {
    switch (getItem(x)) {
    | PlainText({value}) => <p key={string_of_int(i)}> {s(value)} </p>
    | _ => React.null
    }
  })
  |> React.array;
};

let rec renderItems = xs => {
  Belt.Array.mapWithIndex(xs, (i, x) => {
    switch (getItem(x)) {
    | Headline({level}) as z => renderHeadline(z) |> wrapWithKey(level, i)
    | Section({children, level}) =>
      renderItems(children) |> wrapWithKey(level, i)
    | Paragraph({children}) => renderParagraphs(children)
    | _ => React.null
    }
  })
  |> React.array;
};

let render = () =>
  orga.children[0] |> Orga.getMainItem |> Utils.log |> renderItems;
/* |> Array.mapi((i, x) => */
/*      switch (getItem(x)) { */
/*      | Headline({content}) => */
/*        <span key={string_of_int(i)}> {React.string(content)} </span> */
/*      | _ => React.null */
/*      } */
/*    ); */

[@react.component]
let make = () => <> {render()} </>;
