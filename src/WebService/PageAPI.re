open PageTypes;

module D = Decode.AsResult.OfParseError;

module PageResponse = {
  let ((<$>), (<*>)) = D.(map, apply);

  let make = (text): PageType.t => {text: text};

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

module type PageRequest = {
  let getPageIO:
    string => Relude.IO.t(PageType.t, ReludeFetch.Error.t(string));
};

module PageRequest: PageRequest = {
  open Relude.IO;

  let baseUrl = "http://localhost:4000/file/";

  let getPageIO = (file: string) => {
    let url = baseUrl ++ file ++ ".org";

    ReludeFetch.get(url)
    >>= ReludeFetch.Response.StatusCode.ensure2xx
    >>= ReludeFetch.Response.json
    >>= PageResponse.decode(url);
  };
};
