open ReactUtils;
open Relude.Globals;
open State;
open Dialogs__Style;

let id = "Dialogs__FilePicker";

[@react.component]
let make = (~close) => {
  let workspaces = Store.useSelector(Selector.Workspaces.workspaces);

  let results =
    workspaces
    |> List.foldLeft(
         (acc, (workspace, files)) => {
           let index =
             acc
             |> Array.last
             |> Option.map(((i, _, _)) => Int.add(1, i))
             |> Option.getOrElse(0);

           files
           |> Array.map(x => (index, workspace, x))
           |> Array.concat(acc);
         },
         [||],
       );

  let onSubmit = (~index=None, results) => {
    results
    |> Array.at(index |> Option.getOrElse(0))
    |> Option.orElse(~fallback=results |> Array.head)
    |> Option.flatMap(
         ((_, workspaceName, {name}: Shared__API__Workspaces.File.t)) =>
         workspaces
         |> List.map(((a, _)) => a)
         |> List.indexOfBy(String.eq, workspaceName)
         |> Option.map(i => (i, name))
       )
    |> Option.tap(((i, name)) => {
         let name = Filename.chop_extension(name);
         close();
         ReasonReactRouter.push({j|/file/$i/$name|j});
       });
  };

  let bindings = [|
    (
      [|"ctrl+k", "ctrl+shift+k", "esc"|],
      _ => {
        close();
        false;
      },
    ),
  |];

  let filter = (~query) =>
    Array.filter(((_, _, {name}: Shared__API__Workspaces.File.t)) =>
      name |> String.toLowerCase |> String.contains(~search=query)
    );

  <RoundedDialogWrapper bindings key={results |> Array.length |> Int.toString}>
    <Menu
      onSubmit={({selection, query}) =>
        results |> filter(~query) |> onSubmit(~index=selection) |> ignore
      }
      onEscape=close
      placeholder="Pick File"
      items={({selection, query}) => results |> filter(~query)}
      renderItems={({selection, query}) =>
        results
        |> filter(~query)
        |> Array.mapWithIndex(
             ((_, _, {name}: Shared__API__Workspaces.File.t), i) => {
             let isSelected =
               selection |> Option.filter(Int.eq(i)) |> Option.isSome;
             <React.Fragment key={i |> Int.toString}>
               <Menu.Item
                 isSelected
                 onClick={_ => onSubmit(~index=Some(i), results) |> ignore}
                 value={name |> Filename.chop_extension}
               />
             </React.Fragment>;
           })
        |> React.array
      }
    />
  </RoundedDialogWrapper>;
};
