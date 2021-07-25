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
    // Complex type that is outlined in the make function, but in short accepts:
    // A time range, An abstract time definition (for the current day/week etc. outlined in currentT) or a mixture of both
    //
    // Since we can't represent this in json without creating arrays of mixed object,
    // we store it as an object with holes in this format:
    // Js Format: { current: nullable(currentTFromString), from: nullable(Js.Date.t), to_: nullable(Js.Date.t) }
    //
    // When the js format doesnt apply to our restrictions (see make) we return an Error
    type currentT =
      | CurrentDay
      | CurrentWeek
      | CurrentMonth;

    let currentTFromString =
      fun
      | "Day" => CurrentDay
      | "Week" => CurrentWeek
      | "Month" => CurrentMonth
      | _ => CurrentDay;

    let stringFromcurrentT =
      fun
      | CurrentDay => "Day"
      | CurrentWeek => "Week"
      | CurrentMonth => "Month";

    type timestampT = Js.Date.t;

    type timerangeT =
      | CurrentOnly(currentT)
      | CurrentTo(currentT, timestampT)
      | CurrentFrom(timestampT, currentT)
      | Timerange(timestampT, timestampT);

    type t = result(timerangeT, (string, Js.Json.t));

    let make = (current, from, to_, originalJson): t =>
      switch (current, from, to_) {
      | (Some(cur), None, None) =>
        Ok(CurrentOnly(cur |> currentTFromString))

      | (Some(cur), None, Some(to_)) =>
        Ok(CurrentTo(cur |> currentTFromString, to_))

      | (Some(cur), Some(from), None) =>
        Ok(CurrentFrom(from, cur |> currentTFromString))

      | (None, Some(from), Some(to_)) => Ok(Timerange(from, to_))

      // Invalid
      | _ => Error(("Invalid time format", originalJson))
      };

    let currentToInterval = (current: currentT): ReDate.interval => {
      let now = Js.Date.make();
      switch (current) {
      | CurrentDay => {
          start: now |> ReDate.startOfDay,
          end_: now |> ReDate.endOfDay,
        }
      | CurrentWeek => {
          start: now |> ReDate.startOfWeek(~weekStartsOn=ReDate.Monday),
          end_: now |> ReDate.endOfWeek(~weekStartsOn=ReDate.Monday),
        }
      | CurrentMonth => {
          start: now |> ReDate.startOfMonth,
          end_: now |> ReDate.endOfMonth,
        }
      };
    };

    let timeRangeToInterval = (timerange: timerangeT) => {
      switch (timerange) {
      | CurrentOnly(current) => currentToInterval(current)
      | CurrentTo(current, end_) => {
          start: currentToInterval(current).start,
          end_,
        }
      | CurrentFrom(start, current) => {
          start,
          end_: currentToInterval(current).end_,
        }
      | Timerange(start, end_) => {start, end_}
      };
    };

    let isWithinSingle = (current: currentT, date: Js.Date.t) => {
      let now = Js.Date.make();
      switch (current) {
      | CurrentDay => ReDate.isSameDay(now, date)
      | CurrentWeek =>
        ReDate.isSameWeek(~weekStartsOn=ReDate.Monday, now, date)
      | CurrentMonth => ReDate.isSameMonth(now, date)
      };
    };
  };

  module Filter = {
    type t =
      | Add(string)
      | Remove(string);

    type tagFilter = t;
    type todoFilter = t;
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
    tags: option(array(Filter.tagFilter)),
    todos: option(array(Filter.todoFilter)),
  };

  let make =
      (files, fields: array((string, string)), timerange, tags, todos) => {
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
    tags,
    todos,
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

  let encodeAgendasTimerangeJson = (x: Agenda.Time.timerangeT) =>
    Json.Encode.(
      (
        switch (x) {
        | CurrentOnly(x) =>
          Some(
            object_([
              ("current", string(Agenda.Time.stringFromcurrentT(x))),
            ]),
          )
        | CurrentTo(current, to_) =>
          Some(
            object_([
              ("current", string(Agenda.Time.stringFromcurrentT(current))),
              ("to_", date(to_)),
            ]),
          )
        | CurrentFrom(from, current) =>
          Some(
            object_([
              ("current", string(Agenda.Time.stringFromcurrentT(current))),
              ("from", date(from)),
            ]),
          )
        | Timerange(from, to_) =>
          Some(object_([("from", date(from)), ("to_", date(to_))]))
        }
      )
      |> Option.map(x => [("timerange", x)])
    );

  let encodeFiltersJson =
    Agenda.Filter.(
      fun
      | Add(x) => String.concat("+", x)
      | Remove(x) => String.concat("-", x)
    )
    >> Json.Encode.string;

  let encodeAgendasJson =
    Json.Encode.(
      ({files, fields, timerange, tags, todos}: Agenda.t) =>
        object_(
          [
            (
              "files",
              files |> Array.map(encodeAgendasFilesJson) |> jsonArray,
            ),
            (
              "fields",
              fields
              |> Array.map(x =>
                   tuple2(string, string, Agenda.fieldToString(x))
                 )
              |> jsonArray,
            ),
          ]
          |> (
            xs =>
              timerange
              |> Option.flatMap(Result.toOption)
              |> Option.flatMap(encodeAgendasTimerangeJson)
              |> Option.fold(xs, List.concat(xs))
          )
          |> (
            xs =>
              tags
              |> Option.map(xs => [("tags", array(encodeFiltersJson, xs))])
              |> Option.fold(xs, List.concat(xs))
          )
          |> (
            xs =>
              todos
              |> Option.map(xs =>
                   [("todos", array(encodeFiltersJson, xs))]
                 )
              |> Option.fold(xs, List.concat(xs))
          ),
        )
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

  let decodeAgendaFilesJson = json =>
    D.Pipeline.(
      succeed(Agenda.File.make)
      |> field("id", string)
      |> field("workspace", intFromNumber)
      |> run(json)
    );

  let decodeAgendaTimerangeJson = json =>
    D.Pipeline.(
      succeed(Agenda.Time.make)
      |> optionalField("current", string)
      |> optionalField("from", date)
      |> optionalField("to_", date)
      |> hardcoded(json)
      |> run(json)
    );

  let parseFilter = x =>
    x
    |> String.splitAt(1)
    |> (
      fun
      | ("+", x) => Ok(Agenda.Filter.Add(x))
      | ("-", x) => Ok(Agenda.Filter.Remove(x))
      | (_a, _b) =>
        Error(Decode.ParseError.Val(`InvalidFilter, Js.Json.string(x)))
    );

  let decodeFilter = json =>
    D.string(json) |> Result.flatMap(x => parseFilter(x));

  let decodeAgendaJson = json =>
    D.Pipeline.(
      succeed(Agenda.make)
      |> field("files", array(decodeAgendaFilesJson))
      |> field("fields", array(tuple2(string, string)))
      |> optionalField("timerange", decodeAgendaTimerangeJson)
      |> optionalField("tags", array(decodeFilter))
      |> optionalField("todos", array(decodeFilter))
      |> run(json)
    );

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
