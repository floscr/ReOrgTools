open FilesTypes;

module D = Decode.AsResult.OfParseError;

module FilesResponse = {
  let ((<$>), (<*>)) = D.(map, apply);

  let make = (value, label): FilesType.t => {value, label};

  let decodeOne =
    make <$> D.field("label", D.string) <*> D.field("value", D.string);
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

  let optionsUrl = "http://localhost:4000/files";

  let getFilesIO = (url: option(string)) => {
    let url = Belt.Option.getWithDefault(url, optionsUrl);

    ReludeFetch.get(url)
    >>= ReludeFetch.Response.StatusCode.ensure2xx
    >>= ReludeFetch.Response.json
    >>= FilesResponse.decode(url);
  };
};
