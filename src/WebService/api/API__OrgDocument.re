open API__OrgDocument__Types;

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
    string => Relude.IO.t(OrgDocumentType.t, ReludeFetch.Error.t(string));
};

module Request: Request = {
  open Relude.IO;

  let baseUrl = "http://localhost:4000/file/";

  let getPageIO = (file: string) => {
    let url = baseUrl ++ file ++ ".org";

    ReludeFetch.get(url)
    >>= ReludeFetch.Response.StatusCode.ensure2xx
    >>= ReludeFetch.Response.json
    >>= Response.decode(url);
  };
};
