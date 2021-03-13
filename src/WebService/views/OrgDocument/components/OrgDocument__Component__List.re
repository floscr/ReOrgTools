open ReOrga;
open ReactUtils;

let renderListItem = xs => {
  Relude.Globals.(
    xs
    |> Array.mapWithIndex((x, i) => {
         let key = string_of_int(i);
         (
           switch (x |> getItem) {
           | ListItemCheckBox({checked}) =>
             <input type_="checkbox" defaultChecked=checked />
           | _ => OrgDocument__Component__Text.renderParagraphs([|x|])
           }
         )
         |> (x => <React.Fragment key> x </React.Fragment>);
       })
    |> React.array
  );
};

let rec renderList = (xs, ordered) => {
  Relude.Globals.(
    xs
    |> Array.mapWithIndex((x, i) => {
         let key = string_of_int(i);
         let next = Array.at(i + 1, xs) |> Option.map(getItem);
         switch (getItem(x), next) {
         | (ListItem({children}), Some(List(sublist))) =>
           <li key>
             {renderListItem(children)}
             {renderList(sublist.children, sublist.ordered)}
           </li>
         | (ListItem({children}), _) =>
           <li key> {renderListItem(children)} </li>
         /* | (List(_), _) => React.null */
         | _ => React.null
         };
       })
    |> React.array
    |> (
      xs =>
        switch (ordered) {
        | true => <ol> xs </ol>
        | _ => <ul> xs </ul>
        }
    )
  );
};
