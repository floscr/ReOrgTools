open ReOrga;
open ReactUtils;
open Relude.Globals;

module Styles = {
  open Css;
  open FixedTheme;

  let table =
    style([
      border(px(1), `solid, var(ThemeKeys.baseGray1)),
      borderCollapse(`collapse),
      margin(zero),
      padding(zero),
      tableLayout(`fixed),
      selector("tr", [border(px(1), `solid, var(ThemeKeys.baseGray1))]),
      selector(
        "th",
        [
          padding(Spacing.small),
          textAlign(`left),
          fontWeight(`normal),
          border(px(1), `solid, var(ThemeKeys.baseGray1)),
        ],
      ),
      selector("thead", [backgroundColor(var(ThemeKeys.baseGray1))]),
    ]);
};

let renderRow = (~group="", xs) =>
  xs
  |> Array.mapWithIndex((x, i) => {
       let rowKey = {j|$group-$i|j};
       switch (x |> getItem) {
       | TableRow({children}) =>
         <tr key=rowKey>
           {children
            |> Array.mapWithIndex((x, i) => {
                 let key = {j|$rowKey-$i|j};
                 switch (x |> getItem) {
                 | TableCell({children}) =>
                   <th key>
                     {OrgDocument__Component__Text.renderParagraphs(children)}
                   </th>
                 | _ => React.null
                 };
               })
            |> React.array}
         </tr>
       | _ => React.null
       };
     })
  |> React.array;

let renderTable = xs => {
  let (thead, tbody) =
    xs
    |> Array.take(3)
    |> (
      ys =>
        switch (ys |> Array.map(getItem)) {
        | [|TableRow(_), TableHr(_), _|] => (
            Some(Array.take(2, xs)),
            Some(Array.drop(2, xs)),
          )
        | [|TableHr(_), TableRow(_), TableHr(_)|] => (
            Some(ys),
            Some(Array.drop(3, xs)),
          )
        | _ => (None, Some(xs))
        }
    );

  <table className=Styles.table>
    {thead
     |> Option.map(renderRow(~group="thead"))
     |> Option.map(xs => <thead> xs </thead>)
     |> Option.getOrElse(React.null)}
    {tbody
     |> Option.map(renderRow(~group="tbody"))
     |> Option.map(xs => <tbody> xs </tbody>)
     |> Option.getOrElse(React.null)}
  </table>;
};
