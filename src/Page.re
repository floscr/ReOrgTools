open Orga;

let orga =
  Org.parseOrga(
    {j|* TODO Headline :FOO:FAA:
** Child 1

Content

*** Child2

|j},
  );

let render = () =>
  orga.children[0]
  |> Orga.getMainItem
  |> Utils.log
  |> Array.mapi((i, x) =>
       switch (getItem(x)) {
       | Headline({content}) =>
         <span key={string_of_int(i)}> {React.string(content)} </span>
       | _ => React.null
       }
     );

[@react.component]
let make = () => <> {render()->React.array} </>;
