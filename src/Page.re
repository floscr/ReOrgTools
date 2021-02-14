open Orga;
open ReactUtils;

let orga =
  Org.parseOrga(
    {j|* TODO Headline :FOO:FAA:
** Child 1

Content

*** Child2

|j},
  );

let renderHeadline = x =>
  switch (x) {
  | Headline({content}) => <h1> {s(content)} </h1>
  | _ => React.null
  };

let rec renderItems = xs => {
  Belt.Array.mapWithIndex(xs, (i, x) => {
    switch (getItem(x)) {
    | Headline(_) as z => renderHeadline(z)
    | Section({children}) => <> {renderItems(children)} </>
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
