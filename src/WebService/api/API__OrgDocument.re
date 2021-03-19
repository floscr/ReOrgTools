open API__OrgDocument__Types;
open Relude.Globals;

module D = Decode.AsResult.OfParseError;

module Response = {
  let ((<$>), (<*>)) = D.(map, apply);

  let make = (text): OrgDocumentType.t => {text: text};

  let decodeOne = make <$> D.field("text", D.string);

  let decode = (url, json) => {
    decodeOne(json)
    |> Relude.IO.fromResult
    |> Relude.IO.mapRight(e => {
         let readableError = D.ParseError.failureToDebugString(e);
         ReludeFetch.Error.decodeError({url, innerError: readableError});
       });
  };
};

module type Request = {
  let getPageIO:
    (~workspaceIndex: int, ~file: string) =>
    Relude.IO.t(OrgDocumentType.t, ReludeFetch.Error.t(string));
};

module Request: Request = {
  open Relude.IO;

  let getPageIO = (~workspaceIndex: int, ~file: string) => {
    let url =
      ReludeURL.(
        URI.makeWithLabels(
          ~scheme=Scheme("http"),
          ~authority=
            Authority.fromHostnameAndPort(
              Hostname.make("localhost"),
              Port.make(4000),
            ),
          ~path=
            Path.make([
              PathSegment.make("file"),
              PathSegment.make(workspaceIndex |> Int.toString),
              PathSegment.make(file ++ ".org"),
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
