open Relude.Globals;

module Bookmark = {
  type t = {
    title: string,
    value: string,
  };

  let make = (title, value) => {title, value};

  let eq = (a, b) =>
    String.eq(a.title, b.title) && String.eq(a.value, b.value);
};

type lastViewedFile = option(string);
let makeLastViewedFile = (x: string): lastViewedFile => Some(x);

type state = {
  lastViewedFile,
  bookmarks: array(Bookmark.t),
};

type action =
  | SaveLastViewdFile(string)
  | SaveState(state)
  | ToggleBookmark(Bookmark.t)
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

  let decodeBookmarksJson = json =>
    Decode.Pipeline.(
      succeed(Bookmark.make)
      |> field("title", string)
      |> field("value", string)
      |> run(json)
    );

  let make = (lastViewedFile, bookmarks) => {
    lastViewedFile,
    bookmarks: bookmarks |> Option.getOrElse([||]),
  };

  let decodeJson = json =>
    Decode.Pipeline.(
      succeed(make)
      |> field("lastViewedFile", optional(string))
      |> optionalField("bookmarks", array(decodeBookmarksJson))
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
  | ToggleBookmark((x: Bookmark.t)) => {
      ...state,
      bookmarks:
        state.bookmarks
        |> Array.indexOfBy(Bookmark.eq, x)
        |> Option.foldLazy(
             _ => Array.append(x, state.bookmarks),
             i => Array.removeAt(i, state.bookmarks),
           ),
    }
  | ResetState => initialState
  };
};
