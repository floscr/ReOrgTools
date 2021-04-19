open Relude.Globals;
open ReactUtils;

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
  | ResetState;

let initialState = {lastViewedFile: None, bookmarks: [||]};

module Encode = {
  let encodeBookmarksJson =
    Json.Encode.(
      ({title, value}: Bookmark.t) =>
        object_([("title", string(title)), ("value", string(value))])
    );

  let encodeJson =
    Json.Encode.(
      ({lastViewedFile, bookmarks}) =>
        object_([
          ("lastViewedFile", nullable(string, lastViewedFile)),
          ("bookmarks", array(encodeBookmarksJson, bookmarks)),
        ])
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

  let make = (lastViewedFile, bookmarks) => {lastViewedFile, bookmarks};

  let decodeJson = json =>
    Decode.Pipeline.(
      succeed(make)
      |> field("lastViewedFile", optional(string))
      |> field("bookmarks", array(decodeBookmarksJson))
      |> run(json)
    );
};

let storeSettings = state => {
  Localforage.Localforage_IO.set(
    Types__LocalStorage.settings,
    Encode.encodeJson(state),
  )
  |> IO.unsafeRunAsync(ignore);
};

let reducer = (state, action) => {
  switch (action) {
  | SaveState(state) => state
  | SaveLastViewdFile((lastViewedFile: string)) =>
    let lastViewedFile = lastViewedFile |> makeLastViewedFile;
    {...state, lastViewedFile} |> tap(storeSettings);
  | ToggleBookmark((x: Bookmark.t)) =>
    {
      ...state,
      bookmarks:
        state.bookmarks
        |> Array.indexOfBy(Bookmark.eq, x)
        |> Option.foldLazy(
             _ => Array.append(x, state.bookmarks),
             i => Array.removeAt(i, state.bookmarks),
           ),
    }
    |> tap(storeSettings)
  | ResetState => initialState
  };
};
