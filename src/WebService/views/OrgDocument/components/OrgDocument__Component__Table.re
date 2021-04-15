open ReOrga;
open ReactUtils;

module Styles = {
  open Css;
  open FixedTheme;

  let table =
    style([
      border(px(1), `solid, var(ThemeKeys.grey10)),
      borderCollapse(`collapse),
      margin(zero),
      padding(zero),
      tableLayout(`fixed),
      selector("tr", [border(px(1), `solid, var(ThemeKeys.grey10))]),
      selector(
        "th",
        [
          padding(Spacing.small),
          textAlign(`left),
          fontWeight(`normal),
          border(px(1), `solid, var(ThemeKeys.grey10)),
        ],
      ),
      selector("thead", [backgroundColor(var(ThemeKeys.grey00))]),
    ]);
};

let renderTable = xs => {
  let hasTableHead =
    switch (Js.Array.slice(~start=0, ~end_=2, xs) |> Array.map(getItem)) {
    | [|TableRow(_), TableHr(_)|] => true
    | _ => false
    };

  /* let lastIndex = Array.length(xs) - 1; */
  /* let hasTableFoot = */
  /*   switch (Array.sub(xs, lastIndex - 3, lastIndex) |> Array.map(getItem)) { */
  /*   | [|TableHr(_), TableRow(_)|] => true */
  /*   | _ => false */
  /*   }; */

  <table className=Styles.table>
    <tbody>
      {Belt.Array.mapWithIndex(
         xs,
         (rowIndex, x) => {
           let rowKey = string_of_int(rowIndex);
           switch (getItem(x)) {
           | TableRow({children}) =>
             <tr key=rowKey>
               {Belt.Array.mapWithIndex(
                  children,
                  (cellIndex, x) => {
                    let cellKey = string_of_int(cellIndex);
                    let key = {j|$rowKey-$cellKey|j};
                    switch (getItem(x)) {
                    | TableCell({children}) =>
                      <th key>
                        {OrgDocument__Component__Text.renderParagraphs(
                           children,
                         )}
                      </th>
                    | _ => React.null
                    };
                  },
                )
                |> React.array}
             </tr>
             |> (
               x =>
                 switch (rowIndex, hasTableHead) {
                 | (0, true) => <thead> x </thead>
                 /* | (lastIndex, _, true) => <tfoot> x </tfoot> */
                 | _ => x
                 }
             )
           | _ => React.null
           };
         },
       )
       |> React.array}
    </tbody>
  </table>;
};
