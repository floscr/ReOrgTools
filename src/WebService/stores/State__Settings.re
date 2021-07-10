open Relude.Globals;
open ReactUtils;

module Agenda = {
  module File = {
    type t = State__OrgDocuments.File.identifier;

    let make = (id, workspace): State__OrgDocuments.File.identifier => {
      id,
      workspace,
    };
  };

  module Time = {
    // Js Format: { current: nullable(currentTFromString), from: nullable(Js.Date.t), to_: nullable(Js.Date.t) }
    type currentT =
      | CurrentDay
      | CurrentWeek
      | CurrentMonth;

    let currentTFromString =
      fun
      | "Day" => CurrentDay
      | "Week" => CurrentWeek
      | "Month" => CurrentMonth
      | _ => CurrentMonth;

    type timeT =
      | Current(currentT)
      | Timestamp(Js.Date.t);

    type timerangeT =
      | TimeRange((option(timeT), option(timeT)));
    type t = result(timerangeT, string);

    let make = (current, from, to_) =>
      switch (current, from, to_) {
      // Current without from/to delimiters
      | (Some(cur), None, None) =>
        Ok(TimeRange((Some(Current(cur |> currentTFromString)), None)))

      // From Date - Current
      | (Some(cur), Some(from), None) =>
        Ok(
          TimeRange((
            Some(Timestamp(from)),
            Some(Current(cur |> currentTFromString)),
          )),
        )

      // Current - To Date
      | (Some(cur), None, Some(to_)) =>
        Ok(
          TimeRange((
            Some(Current(cur |> currentTFromString)),
            Some(Timestamp(to_)),
          )),
        )

      // Timerange
      | (None, Some(from), Some(to_)) =>
        Ok(TimeRange((Some(Timestamp(from)), Some(Timestamp(to_)))))

      // Invalid
      | _ => Error("Invalid time format")
      };
  };

  type field =
    | Layout(Types__Org.Layout.t);

  let fieldToString =
    fun
    | Layout(x) => ("Layout", x |> Types__Org.Layout.toString);

  type filesT = array(File.t);

  type t = {
    files: filesT,
    fields: array(field),
    timerange: option(Time.t),
  };

  let make = (files, fields: array((string, string)), timerange) => {
    files,
    fields:
      fields
      |> Array.foldLeft(
           (acc, (key, value)) => {
             (
               switch (key, value) {
               | ("Layout", x) =>
                 Some(Layout(x |> Types__Org.Layout.fromString))
               | _ => None
               }
             )
             |> Option.foldLazy(_ => acc, x => Array.append(x, acc))
           },
           [||],
         ),
    timerange,
  };
};

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
  agendas: array(Agenda.t),
  bookmarks: array(Bookmark.t),
  isSidebarOpen: bool,
  lastViewedFile,
};

type action =
  | SaveLastViewdFile(string)
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
  let encodeAgendasFilesJson =
    Json.Encode.(
      ({id, workspace}: Agenda.File.t) =>
        object_([("id", string(id)), ("workspace", int(workspace))])
    );

  let encodeAgendasJson =
    Json.Encode.(
      ({files, fields}: Agenda.t) =>
        object_([
          ("files", files |> Array.map(encodeAgendasFilesJson) |> jsonArray),
          (
            "fields",
            fields
            |> Array.map(x =>
                 tuple2(string, string, Agenda.fieldToString(x))
               )
            |> jsonArray,
          ),
        ])
    );

  let encodeBookmarksJson =
    Json.Encode.(
      ({title, value}: Bookmark.t) =>
        object_([("title", string(title)), ("value", string(value))])
    );

  let encodeJson =
    Json.Encode.(
      ({agendas, bookmarks, isSidebarOpen, lastViewedFile}) =>
        object_([
          ("agendas", array(encodeAgendasJson, agendas)),
          ("bookmarks", array(encodeBookmarksJson, bookmarks)),
          ("isSidebarOpen", bool(isSidebarOpen)),
          ("lastViewedFile", nullable(string, lastViewedFile)),
        ])
    );
};

module Decode = {
  module Decode = Decode.AsResult.OfParseError;

  let decodeAgendaFilesJson = json =>
    Decode.Pipeline.(
      succeed(Agenda.File.make)
      |> field("id", string)
      |> field("workspace", intFromNumber)
      |> run(json)
    );

  let decodeAgendaTimerangeJson = json =>
    Decode.Pipeline.(
      succeed(Agenda.Time.make)
      |> field("current", optional(string))
      |> field("from", optional(date))
      |> field("to_", optional(date))
      |> run(json)
    );

  let decodeAgendaJson = json =>
    Decode.Pipeline.(
      succeed(Agenda.make)
      |> field("files", array(decodeAgendaFilesJson))
      |> field("fields", array(tuple2(string, string)))
      |> optionalField("timerange", decodeAgendaTimerangeJson)
      |> run(json)
    );

  let decodeBookmarksJson = json =>
    Decode.Pipeline.(
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
    Decode.Pipeline.(
      succeed(make)
      |> optionalField("agendas", array(decodeAgendaJson))
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
