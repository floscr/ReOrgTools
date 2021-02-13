open Orga;

let orga =
  Org.parseOrga(
    {j|* TODO Headline :FOO:FAA:
** Child 1

Content

*** Child2

|j},
  );

/* let renderHeadline = x => */
/*   switch (x) { */
/*   | Headline({content}) => <h1> {React.string(content)} </h1> */
/*   | _ => React.null */
/*   }; */

let renderHeadline = x =>
  switch (x) {
  | Headline({content}) => <h1> {React.string(content)} </h1>
  | _ => React.null
  };

let rec renderItems = xs =>
  switch (xs |> Array.map(getItem) |> Array.to_list) {
  | [Headline(_) as y, ...ys] => <> {renderHeadline(y)} </>
  | _ => React.null
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
