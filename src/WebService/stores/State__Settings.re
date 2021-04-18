open Relude.Globals;

type lastViewedFile = option(string);
let makeLastViewedFile = (x: string): lastViewedFile => Some(x);

type state = {
  lastViewedFile,
  bookmarks: array(string),
};

type action =
  | SaveLastViewdFile(string)
  | SaveState(state)
  | ToggleBookmark(string)
  /* | LoadState */
  | ResetState;

let initialState = {lastViewedFile: None, bookmarks: [||]};

module Encode = {
  let encodeJson =
    Json.Encode.(
      ({lastViewedFile}) =>
        object_([("lastViewedFile", nullable(string, lastViewedFile))])
    );
};

module Decode = {
  module Decode = Decode.AsResult.OfParseError;

  let make = (lastViewedFile, bookmarks) => {
    lastViewedFile,
    bookmarks: bookmarks |> Option.getOrElse([||]),
  };

  let decodeJson = json =>
    Decode.Pipeline.(
      succeed(make)
      |> field("lastViewedFile", optional(string))
      |> optionalField("bookmarks", array(string))
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
  | ToggleBookmark((x: string)) => {
      ...state,
      bookmarks:
        state.bookmarks
        |> Array.indexOfBy(String.eq, x)
        |> Option.foldLazy(
             _ => Array.append(x, state.bookmarks),
             i => Array.removeAt(i, state.bookmarks),
           ),
    }
  | ResetState => initialState
  };
};
