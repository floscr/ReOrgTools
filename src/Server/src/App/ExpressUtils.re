open Relude.Globals;

let resolve = (res, handler, x) =>
  x
  |> Relude.Js.Promise.fromIOWithResult
  |> Js.Promise.(then_(data => res |> handler(data) |> resolve));

let filterResult = (fn, ~err, x) =>
  switch (x) {
  | Result.Ok(y) when fn(y) => x
  | Result.Ok(y) => Result.Error(err(y))
  | _ => x
  };
