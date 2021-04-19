open ReactUtils;
open Relude.Globals;
open State;
open Sidebar__Utils;

[@react.component]
let make = () => {
  let bookmarks = Store.useSelector(Selector.Settings.bookmarks);

  let onClick = x => {
    ReasonReactRouter.replace(x);
  };

  bookmarks
  |> Array.map(({title, value}: State__Settings.Bookmark.t) =>
       <li className=Styles.li key={j|bookmark-$title|j}>
         <button
           className=Styles.button
           onClick={_ => ReasonReactRouter.replace(value)}>
           {title |> s}
         </button>
       </li>
     )
  |> Option.some
  |> Option.filter(Array.isNotEmpty)
  |> Option.map(React.array)
  |> Option.fold(React.null, xs =>
       <>
         <header className=Styles.header> {"Bookmarks" |> s} </header>
         <ul className=Styles.ul> xs </ul>
       </>
     );
};
