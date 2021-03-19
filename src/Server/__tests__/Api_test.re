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

describe("IO basics", () => {
  testAsync("getWorkspaces", onDone => {
    getWorkspaces()
    |> IOE.all
    |> IO.bimap(({name }) => expect(name) |> toEqual(result), _ => fail("Failed"))
    |> IO.unsafeRunAsync(
         fun
         | Ok(assertion) => onDone(assertion)
         | Error(assertion) => onDone(assertion),
       )
  })
});
