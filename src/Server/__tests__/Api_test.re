open Jest;
open Expect;
open Relude.Globals;
open Api;

module IOE =
  IO.WithError({
    type t = getDirEntriesError;
  });

let result = [[|"1.org"|], [|"1.org", "2.org"|]];

describe("IO basics", () => {
  testAsync("getWorkspaces", onDone => {
    getWorkspaces(~workspaces=Config.workspaces |> List.prepend("./foo"), ())
    |> IO.bimap(expect >> toEqual(result), _ => fail("Failed"))
    |> IO.unsafeRunAsync(
         fun
         | Ok(assertion) => onDone(assertion)
         | Error(assertion) => onDone(assertion),
       )
  })
});
