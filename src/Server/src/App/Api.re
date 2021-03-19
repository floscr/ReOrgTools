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
  workspaces
  |> List.map(x =>
       x
       |> getDirFiles
       |> IO.tapError(_ => Config.log({j|Workspace $x doesn't exist|j}))
       // Turn the error into Void so it can't fail
       |> IO.summonError
     )
  |> List.IO.sequence
  // Reject the void values so we get a list of only succesful entries
  |> IO.map(List.mapOption(Result.getOk));
