open Relude.Globals;

type lastViewedFile = option(string);

type state = {lastViewedFile};

type action =
  | SaveLastViewdFile(lastViewedFile)
  /* | SaveState(state) */
  /* | LoadState */
  | ResetState;

let initialState = {lastViewedFile: None};

module Encode = {
  let encodeJson =
    Json.Encode.(
      ({lastViewedFile}) =>
        object_([("lastViewedFile", nullable(string, lastViewedFile))])
    );
};

module Decode = {
  module Decode = Decode.AsResult.OfParseError;

  let make = lastViewedFile => {
    lastViewedFile;
  };

  let decodeJson = json =>
    Decode.Pipeline.(
      succeed(make)
      |> field("lastViewedFile", optional(string))
      |> run(json)
    );
};

let reducer = (state, action) => {
  switch (action) {
  | SaveLastViewdFile(lastViewedFile) =>
    let state = {...state, lastViewedFile};

    Localforage.Localforage_IO.set(
      Types__LocalStorage.settings,
      Encode.encodeJson(state),
    )
    |> IO.unsafeRunAsync(ignore);

    state;
  | ResetState => initialState
  };
};
