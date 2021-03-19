open Jest;
open Expect;
open Relude.Globals;
open Api;

module IOE =
  IO.WithError({
    type t = getDirEntriesError;
  });

describe("IO basics", () => {
  testAsync("all", onDone => {
    let x =
      getWorkspaces()
      |> IOE.all
      |> IO.bimap(Js.log, _ => Js.log("error"))
      |> IO.unsafeRunAsync(
           fun
           | Ok(_) => onDone(expect(1) |> toEqual(42))
           | Error(_) => onDone(fail("Failed")),
         );

    ();
  })
});
