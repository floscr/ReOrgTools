open Relude.Globals;
open NodeFS;
open Shared__API__Workspaces;

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
  |> IO.map(xs =>
       xs
       |> Array.filter((entry: ReadDir.DirectoryEntry.t) =>
            Filename.extension(entry.name)
            |> String.eq(".org")
            && ReadDir.DirectoryEntry.isFile(entry)
          )
       |> Array.map(({name}: ReadDir.DirectoryEntry.t) => {
            let path = Filename.concat(dir, name);
            let stats = Stat.statSync(path);
            ({name, mtimeMs: stats.mtimeMs}: File.t);
          })
       |> (xs => (dir, xs))
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
