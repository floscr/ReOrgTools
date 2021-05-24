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

module Org = {
  let extractArchiveItem = (~text: string, ~bounds: (int, int)) => {
    let (start, end_) = bounds;

    let contents = text |> String.splitAsArray(~delimiter="\n");

    contents
    |> Array.splitAt(start)
    |> Result.fromOption({j| Could not split text at $start.

$text|j})
    |> Result.flatMap(((head, bodyWithTail)) => {
         let index = end_ - start;
         bodyWithTail
         |> Array.splitAt(index)
         |> Option.map(((body, tail)) => {
              (body, Array.concat(head, tail))
            })
         |> Result.fromOption({j| Could  |j});
       });
  };
};
