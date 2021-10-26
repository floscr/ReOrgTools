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
  agendas: array(Types__Agendas.Agenda.t),
  bookmarks: array(Bookmark.t),
  isSidebarOpen: bool,
  lastViewedFile,
};

type action =
  | SaveLastViewdFile(string)
  | SaveAgendaState(Types__Agendas.Agenda.t)
  | RemoveAgenda(Types__Agendas.Agenda.t)
  | EditAgenda(Types__Agendas.Agenda.t)
  | SaveState(state)
  | ToggleSidebar
  | ToggleBookmark(Bookmark.t)
  | ResetState;

let initialState = {
  agendas: [||],
  bookmarks: [||],
  isSidebarOpen: true,
  lastViewedFile: None,
};

module Encode = {
  let encodeBookmarksJson =
    Json.Encode.(
      ({title, value}: Bookmark.t) =>
        object_([("title", string(title)), ("value", string(value))])
    );

  let encodeJson =
    Json.Encode.(
      ({agendas, bookmarks, isSidebarOpen, lastViewedFile}) =>
        object_([
          (
            "agendas",
            array(Types__Agendas__Translators.Agendas.Encode.make, agendas),
          ),
          ("bookmarks", array(encodeBookmarksJson, bookmarks)),
          ("isSidebarOpen", bool(isSidebarOpen)),
          ("lastViewedFile", nullable(string, lastViewedFile)),
        ])
    );
};

module Decode = {
  module R =
    Decode.ParseError.ResultOf({
      type t = [ Decode.ParseError.base | `InvalidFilter];
      let handle = x => (x :> t);
    });

  module D = Decode.Base.Make(R.TransformError, R);

  let failureToPartialString =
    fun
    | `ExpectedBoolean => "Expected boolean"
    | `ExpectedString => "Expected string"
    | `ExpectedNumber => "Expected number"
    | `ExpectedInt => "Expected int"
    | `ExpectedArray => "Expected array"
    | `ExpectedTuple(size) => "Expected tuple of size " ++ Int.toString(size)
    | `ExpectedObject => "Expected object"
    | `ExpectedValidDate => "Expected a valid date"
    | `ExpectedValidOption => "Expected a valid option"
    | `InvalidFilter => "Invalid Filter parsing";

  let failureToString = (v, json) =>
    failureToPartialString(v) ++ " but found " ++ Js.Json.stringify(json);

  let failureToDebugString = err =>
    Decode_ParseError.toDebugString(failureToString, err);

  let decodeBookmarksJson = json =>
    D.Pipeline.(
      succeed(Bookmark.make)
      |> field("title", string)
      |> field("value", string)
      |> run(json)
    );

  let make = (agendas, bookmarks, isSidebarOpen, lastViewedFile) => {
    agendas: agendas |> Option.getOrElse([||]),
    bookmarks: bookmarks |> Option.getOrElse([||]),
    isSidebarOpen:
      isSidebarOpen |> Option.getOrElse(initialState.isSidebarOpen),
    lastViewedFile,
  };

  let decodeJson = json =>
    D.Pipeline.(
      succeed(make)
      |> optionalField(
           "agendas",
           array(Types__Agendas__Translators.Agendas.Decode.make),
         )
      |> optionalField("bookmarks", array(decodeBookmarksJson))
      |> optionalField("isSidebarOpen", boolean)
      |> field("lastViewedFile", optional(string))
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
  | SaveAgendaState(agenda) =>
    {
      ...state,
      agendas:
        Array.indexOfBy(
          (a: Types__Agendas.Agenda.t, b: Types__Agendas.Agenda.t) =>
            a.id === b.id,
          agenda,
          state.agendas,
        )
        |> Option.foldLazy(
             _ => Array.append(agenda, state.agendas),
             index => Array.replaceAt(index, agenda, state.agendas),
           ),
    }
    |> tap(storeSettings)
  | EditAgenda(agenda) =>
    {
      ...state,
      agendas:
        Array.indexOfBy(
          (a: Types__Agendas.Agenda.t, b: Types__Agendas.Agenda.t) =>
            a.id === b.id,
          agenda,
          state.agendas,
        )
        |> Option.foldLazy(
             _ => state.agendas,
             index => Array.removeAt(index, state.agendas),
           ),
    }
    |> tap(storeSettings)
  | RemoveAgenda(agenda) =>
    {
      ...state,
      agendas:
        Array.indexOfBy(
          (a: Types__Agendas.Agenda.t, b: Types__Agendas.Agenda.t) =>
            a.id === b.id,
          agenda,
          state.agendas,
        )
        |> Option.foldLazy(
             _ => state.agendas,
             index => Array.removeAt(index, state.agendas),
           ),
    }
    |> tap(storeSettings)
  | ToggleSidebar =>
    {...state, isSidebarOpen: !state.isSidebarOpen} |> tap(storeSettings)
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
