open ReactUtils;
open Relude.Globals;
open State;
open Dialogs__Style;

let id = "Dialogs__CommandMenu";

type state = {
  query: string,
  selection: option(int),
};

let initialState = {query: "", selection: Some(0)};

type command =
  | ToggleFavorite;

type commandItem = {
  command,
  label: string,
  action: unit => unit,
};

type commandItems = array(commandItem);

[@react.component]
let make = (~close) => {
  let url = ReasonReactRouter.useUrl();
  let dispatch = Store.useDispatch();
  let isFileUrl = url.path |> API__Routes.Routes.isFilePath;

  let items =
    [||]
    |> (
      switch (isFileUrl) {
      | true =>
        Array.append({
          command: ToggleFavorite,
          label: "Toggle Favorite",
          action: _ => {
            SettingsAction(
              State__Settings.ToggleBookmark(
                State__Settings.Bookmark.make(
                  switch (url.path) {
                  | [workspaceIndex, _, file] => {j|$workspaceIndex: $file|j}
                  | _ => ""
                  },
                  Webapi.Dom.(window->Window.location->Location.href),
                ),
              ),
            )
            |> dispatch;
          },
        })
      | _ => identity
      }
    );

  let onSubmit = (~index=None, results) => {
    results
    |> Array.at(index |> Option.getOrElse(0))
    |> Option.orElse(~fallback=results |> Array.head)
    |> Option.tap(({command, action}: commandItem) => action())
    |> Option.tap(_ => close());
  };

  let bindings = [|
    (
      [|"ctrl+shift+p", "esc"|],
      _ => {
        close();
        false;
      },
    ),
  |];

  let filter = (~query) =>
    Array.filter(({label}: commandItem) =>
      label |> String.toLowerCase |> String.contains(~search=query)
    );

  <RoundedDialogWrapper bindings>
    <Menu
      onSubmit={({selection, query}) =>
        items |> filter(~query) |> onSubmit(~index=selection) |> ignore
      }
      onEscape=close
      placeholder="Execute Command"
      items={({selection, query}) => items |> filter(~query)}
      renderItems={({selection, query}) =>
        items
        |> filter(~query)
        |> Array.mapWithIndex(({label}: commandItem, i) => {
             let isSelected =
               selection |> Option.filter(Int.eq(i)) |> Option.isSome;
             <React.Fragment key={i |> Int.toString}>
               <Menu.Item
                 isSelected
                 onClick={_ => onSubmit(~index=Some(i), items) |> ignore}
                 value=label
               />
             </React.Fragment>;
           })
        |> React.array
      }
    />
  </RoundedDialogWrapper>;
};
