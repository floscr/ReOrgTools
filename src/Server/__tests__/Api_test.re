open Jest;
open Expect;
open Relude.Globals;
open Api;

module IOE =
  IO.WithError({
    type t = getDirEntriesError;
  });

let result = [];

describe("IO basics", () => {
  testAsync("getWorkspaces", onDone => {
    getWorkspaces(~workspaces=Config.workspaces |> List.append("./foo"), ())
    /* |> IO.flatMap(xs => {IO.Pure(Array.map(IO.unsafeRunAsync(ignore)))}) */
    /* |> List.map(IO.unsafeRunAsync(ignore)) */
    /* |> Js.log */
    /* |> List.IO.sequence */
    |> IO.bitap(x => Js.log(x), x => Js.log2("err", x))
    |> IO.bimap(x => expect(x) |> toEqual(result), _ => fail("Failed"))
    |> IO.unsafeRunAsync(
         fun
         | Ok(assertion) => onDone(assertion)
         | Error(assertion) => onDone(assertion),
       )
    |> Js.log
  })
});

/* let io1 = IO.suspendIO(() => IO.pure(42)); */
/* let io2 = io1 |> IO.summonError; */
/* let ios = [io1, io2]; */

/* List.IO.sequence(ios) */
/* |> IO.unsafeRunAsync( */
/*      fun */
/*      | Ok(x) => Js.log(x) */
/*      | _ => Js.log("Errorr"), */
/*    ); */
