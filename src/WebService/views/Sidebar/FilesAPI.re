open Relude.Globals;

module FilesType = {
  type t = {name: string};
};

module D = Decode.AsResult.OfParseError;

module FilesResponse = {
  let ((<$>), (<*>)) = D.(map, apply);

  let make = (name): FilesType.t => {name: name};

  let decodeOne = make <$> D.field("name", D.string);
  let decodeMany = D.array(decodeOne);

  let decode = (url, json) => {
    decodeMany(json)
    |> Relude.IO.fromResult
    |> Relude.IO.mapRight(e => {
         let readableError = D.ParseError.failureToDebugString(e);
         ReludeFetch.Error.decodeError({url, innerError: readableError});
       });
  };
};

module type FilesRequest = {
  let getFilesIO:
    option(string) =>
    Relude.IO.t(array(FilesType.t), ReludeFetch.Error.t(string));
};

module FilesRequest: FilesRequest = {
  open Relude.IO;

  let defaultUrl =
    ReludeURL.(
      Shared__Config.frontendUrl(
        ~path=Path.make([PathSegment.make("files")]),
        (),
      )
    )
    |> ReludeURL.URI.show;

  let getFilesIO = (url: option(string)) => {
    let url = Belt.Option.getWithDefault(url, defaultUrl);

    ReludeFetch.get(url)
    >>= ReludeFetch.Response.StatusCode.ensure2xx
    >>= ReludeFetch.Response.json
    >>= FilesResponse.decode(url);
  };
};
