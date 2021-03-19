open Relude.Globals;
open NodeFS;

type getDirEntriesError =
  | ReadDirectoryError(Error.t)
  | ReadEntryError({
      name: string,
      error: Error.t,
    });

let getFile = path =>
  IO.Suspend(() => Config.log({j|Reading from file "$path"|j}))
  |> IO.flatMap(() => ReadFile.readText(path))
  |> IO.mapError(error => ReadEntryError({name: path, error}));

let getDirFiles = dir =>
  IO.Suspend(() => Config.log({j|Reading from entries dir "$dir"|j}))
  |> IO.flatMap(() => ReadDir.readDir(dir))
  |> IO.mapError(error => ReadDirectoryError(error))
  |> IO.map(
       Array.filter((entry: ReadDir.DirectoryEntry.t) =>
         Filename.extension(entry.name)
         |> String.eq(".org")
         && ReadDir.DirectoryEntry.isFile(entry)
       ),
     )
  |> IO.flatMap(xs =>
       IO.suspend(() =>
         Array.map(({name}: ReadDir.DirectoryEntry.t) => name, xs)
       )
     );

let getWorkspaces = (~workspaces=Config.workspaces, ()) =>
  IO.Pure(workspaces |> List.toArray)
  |> IO.flatMap(xs =>
       xs
       |> Array.map(getDirFiles)
       |> Array.foldLeft(
            (accumulator, current) =>
              IO.flatMap(
                entries => IO.map(entry => [entry, ...entries], current),
                accumulator,
              ),
            IO.Pure([]),
          )
     );
/* |> IO.flatMap(xs => */
/*      xs */
/*      |> Js.Array.reduce( */
/*           (accumulator, current) => */
/*             IO.flatMap( */
/*               entries => */
/*                 IO.map(entry => [entry, ...entries], current), */
/*               accumulator, */
/*             ), */
/*           IO.Pure([]), */
/*         ) */
