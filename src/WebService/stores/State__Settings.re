open Relude.Globals;

type lastViewedFile = option(string);
let makeLastViewedFile = (x: string): lastViewedFile => Some(x);

type state = {lastViewedFile};

type action =
  | SaveLastViewdFile(string)
  | SaveState(state)
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

  let make = lastViewedFile => {lastViewedFile: lastViewedFile};

  let decodeJson = json =>
    Decode.Pipeline.(
      succeed(make)
      |> field("lastViewedFile", optional(string))
      |> run(json)
    );
};

let reducer = (state, action) => {
  switch (action) {
  | SaveLastViewdFile((lastViewedFile: string)) =>
    let lastViewedFile = lastViewedFile |> makeLastViewedFile;
    let state = {...state, lastViewedFile};

    Localforage.Localforage_IO.set(
      Types__LocalStorage.settings,
      Encode.encodeJson(state),
    )
    |> IO.unsafeRunAsync(ignore);

    state;
  | SaveState(state) => state
  | ResetState => initialState
  };
};
