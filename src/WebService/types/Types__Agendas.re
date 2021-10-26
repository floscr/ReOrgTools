open Relude.Globals;

module Field = {
  type t =
    | Layout(Types__Org.Layout.t);
  type jsonT = (string, string);

  let toString =
    fun
    | Layout(x) => ("Layout", x |> Types__Org.Layout.toString);
};

module Fields = {
  type t = array(Field.t);
  type jsonT = array(Field.jsonT);

  let make = (json: jsonT) =>
    json
    |> Array.foldLeft(
         (acc, (key, value)) => {
           (
             switch (key, value) {
             | ("Layout", x) =>
               Some(Field.Layout(x |> Types__Org.Layout.fromString))
             | _ => None
             }
           )
           |> Option.foldLazy(_ => acc, x => Array.append(x, acc))
         },
         [||],
       );
};

module Time = {
  // Complex type that is outlined in the make function:
  //
  // It accepts:
  // - A time range
  // - An abstract time definition (for the current day/week etc. outlined in currentT)
  // - A mixture of both
  //
  // Since we can't represent this in json without creating arrays of mixed object,
  // we store it as an object with holes in this format:
  // {
  //   current: nullable(currentTFromString),
  //   from: nullable(Js.Date.t),
  //   to_: nullable(Js.Date.t)
  // }
  //
  // When the js format doesnt apply to our restrictions we return an Error.
  type currentT =
    | CurrentDay
    | CurrentWeek
    | CurrentMonth;

  let currentTFromString =
    fun
    | "day" => CurrentDay
    | "week" => CurrentWeek
    | "month" => CurrentMonth
    | _ => CurrentDay;

  let stringFromcurrentT =
    fun
    | CurrentDay => "day"
    | CurrentWeek => "week"
    | CurrentMonth => "month";

  type timestampT = Js.Date.t;

  type timerangeT =
    | CurrentOnly(currentT)
    | CurrentTo(currentT, timestampT)
    | CurrentFrom(timestampT, currentT)
    | Timerange(timestampT, timestampT);

  type t = result(timerangeT, (string, Js.Json.t));

  let make = (current, from, to_, originalJson): t =>
    switch (current, from, to_) {
    | (Some(cur), None, None) => Ok(CurrentOnly(cur |> currentTFromString))

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
};

module Filter = {
  type t =
    | Add(string)
    | Remove(string);

  type tagFilter = t;
  type todoFilter = t;

  let getString: t => string =
    fun
    | Add(x) => x
    | Remove(x) => x;

  let toBool: t => bool =
    fun
    | Add(_) => true
    | Remove(_) => false;

  let eqString = (t, x: string): bool => t |> getString |> String.eq(x);

  let needsTag = (xs: array(t)): bool =>
    Array.find(toBool, xs) |> Option.isSome;
};

module Agenda = {
  type idT = string;

  type t = {
    id: idT,
    files: array(Types__Files.Identifier.t),
    fields: array(Field.t),
    timerange: option(Time.t),
    tags: option(array(Filter.tagFilter)),
    todos: option(array(Filter.todoFilter)),
    reverse: option(bool),
  };

  let make =
      (
        id,
        files,
        fields: array((string, string)),
        timerange,
        tags,
        todos,
        reverse,
      ) => {
    id,
    files,
    fields: fields |> Fields.make,
    timerange,
    tags,
    todos,
    reverse,
  };
};
