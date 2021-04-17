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
};

type commandItems = array(commandItem);

[@react.component]
let make = (~close) => {
  let items = [|{command: ToggleFavorite, label: "Toggle Favorite"}|];

  let onSubmit = (~index=None, results) => {
    results
    |> Array.at(index |> Option.getOrElse(0))
    |> Option.orElse(~fallback=results |> Array.head)
    |> Option.tap(({command}: commandItem) => Js.log(command)) /* ExecuteCommand(command) |> send */;
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
