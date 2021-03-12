open PageTypes;
open ReactUtils;
open Relude.Globals;
open ReOrga;

[@react.component]
let make = (~id, ~header, ~send, ~page) => {
  switch (id) {
  | Some(id) =>
    ReludeReact.Effect.useEffect1WithEq(
      () =>
        PageAPI.PageRequest.getPageIO(id)
        |> Relude.IO.unsafeRunAsync(
             fun
             | Ok(data) => State.FetchPagesSuccess(data)->send
             | Error(data) => State.FetchPagesFailure(data)->send,
           )
        |> ignore,
      (a, b) => a === b,
      id,
    )
  | _ => ()
  };

  switch (page) {
  | State.FetchedPage({ast}) => <Page doc=ast header />
  | State.FetchingPage => "Loading" |> s
  | _ => React.null
  };
};
