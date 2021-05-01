open API__OrgDocument__Types;
open Relude.Globals;

module D = Decode.AsResult.OfParseError;

module Response = {
  let decode = (url, json) => {
    Shared__API__File.File.decodeJson(json)
    |> Relude.IO.fromResult
    |> Relude.IO.mapRight(e => {
         let readableError = D.ParseError.failureToDebugString(e);
         ReludeFetch.Error.decodeError({url, innerError: readableError});
       });
  };
};

module type Request = {
  let make:
    (~workspaceIndex: int, ~file: string) =>
    Relude.IO.t(Shared__API__File.File.t, ReludeFetch.Error.t(string));
};

module Request: Request = {
  open Relude.IO;

  let make = (~workspaceIndex: int, ~file: string) => {
    let url =
      ReludeURL.(
        Shared__Config.backendUrl(
          ~path=
            Path.make([
              PathSegment.make("api"),
              PathSegment.make("file"),
              PathSegment.make(workspaceIndex |> Int.toString),
              PathSegment.make(file ++ ".org" ++ ".json"),
            ]),
          (),
        )
      )
      |> ReludeURL.URI.show;

    ReludeFetch.get(url)
    >>= ReludeFetch.Response.StatusCode.ensure2xx
    >>= ReludeFetch.Response.json
    >>= Response.decode(url);
  };
};
