open Shared__API__Workspaces;

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

  let url = "http://localhost:4000/workspaces";

  let make = () => {
    Js.log("Fetching workspaces");
    ReludeFetch.get(url)
    >>= ReludeFetch.Response.StatusCode.ensure2xx
    >>= ReludeFetch.Response.json
    >>= Response.decode(url);
  };
};
