open Relude.Globals;
open Shared__API__Workspaces;

module Store = {
  type state = {workspaces: Shared__API__Workspaces.Workspaces.t};

  let initialState = {workspaces: []};

  type action =
    | FetchWorkspacesSuccess(Shared__API__Workspaces.Workspaces.t)
    | FetchWorkspacesFailure(ReludeFetch.Error.t(string));

  let reducer = (state, action) => {
    switch (action) {
    | FetchWorkspacesSuccess(workspaces) => {...state, workspaces}
    };
  };
};

module Response = {
  let decode = (url, xs) =>
    Workspaces.decodeMany(xs)
    |> Relude.IO.fromResult
    |> Relude.IO.mapRight(e => {
         let readableError = Decode.ParseError.failureToDebugString(e);
         ReludeFetch.Error.decodeError({url, innerError: readableError});
       });
};

module type Request = {
  let make: unit => Relude.IO.t(Workspaces.t, ReludeFetch.Error.t(string));
};

module Request: Request = {
  open Relude.IO;

  let url =
    ReludeURL.(
      Shared__Config.backendUrl(
        ~path=Path.make([PathSegment.make("workspaces")]),
        (),
      )
    )
    |> ReludeURL.URI.show;

  let make = () => {
    ReludeFetch.get(url)
    >>= ReludeFetch.Response.StatusCode.ensure2xx
    >>= ReludeFetch.Response.json
    >>= Response.decode(url);
  };
};