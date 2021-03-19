open Jest;
open Expect;
open Relude.Globals;
open Api;

module IOE =
  IO.WithError({
    type t = getDirEntriesError;
  });

let result = [
  ("./__tests__/workspaces/workspace1", [|"1.org"|]),
  ("./__tests__/workspaces/workspace2", [|"1.org", "2.org"|]),
];

describe("IO basics", () => {
  testAsync("getWorkspaces with faulty directory input", onDone => {
    getWorkspaces(
      ~workspaces=Config.workspaces |> List.prepend("./faulty-directory"),
      (),
    )
    |> IO.map(xs => xs)
    // Extract filenames from Api.file
    |> IO.map(
         List.map(((a, fs)) =>
           (a, Array.map(({name}: Api.file) => name, fs))
         ),
       )
    |> IO.bimap(expect >> toEqual(result), _ => fail("Failed"))
    |> IO.unsafeRunAsync(
         fun
         | Ok(assertion) => onDone(assertion)
         | Error(assertion) => onDone(assertion),
       )
  })
});
