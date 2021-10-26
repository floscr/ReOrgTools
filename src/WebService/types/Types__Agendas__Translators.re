open Relude.Globals;

module Time = {
  module Encode = {
    module E = Json.Encode;

    let make = (x: Types__Agendas.Time.timerangeT) =>
      E.(
        Types__Agendas.Time.(
          switch (x) {
          | CurrentOnly(x) =>
            object_([("current", string(stringFromcurrentT(x)))])
          | CurrentTo(current, to_) =>
            object_([
              ("current", string(stringFromcurrentT(current))),
              ("to_", date(to_)),
            ])
          | CurrentFrom(from, current) =>
            object_([
              ("current", string(stringFromcurrentT(current))),
              ("from", date(from)),
            ])
          | Timerange(from, to_) =>
            object_([("from", date(from)), ("to_", date(to_))])
          }
        )
      );
  };

  module Decode = {
    module D = DecodeUtils.D;

    let make = json =>
      D.Pipeline.(
        succeed(Types__Agendas.Time.make)
        |> optionalField("current", string)
        |> optionalField("from", date)
        |> optionalField("to_", date)
        |> hardcoded(json)
        |> run(json)
      );
  };
};

module Filter = {
  module Encode = {
    module E = Json.Encode;

    let make =
      Types__Agendas.Filter.(
        fun
        | Add(x) => String.concat("+", x)
        | Remove(x) => String.concat("-", x)
      )
      >> Json.Encode.string;
  };

  module Decode = {
    module D = DecodeUtils.D;

    let parseFilter = x =>
      x
      |> String.splitAt(1)
      |> (
        fun
        | ("+", x) => Ok(Types__Agendas.Filter.Add(x))
        | ("-", x) => Ok(Types__Agendas.Filter.Remove(x))
        | (_a, _b) =>
          Error(Decode.ParseError.Val(`InvalidFilter, Js.Json.string(x)))
      );

    let make = json => D.string(json) |> Result.flatMap(x => parseFilter(x));
  };
};

module Agendas = {
  module Encode = {
    module E = Json.Encode;

    let make =
      E.(
        (
          {id, files, fields, timerange, tags, reverse}: Types__Agendas.Agenda.t,
        ) =>
          object_([
            ("id", string(id)),
            (
              "files",
              files
              |> Array.map(Types__Files__Translators.Identifier.Encode.make)
              |> jsonArray,
            ),
            (
              "fields",
              fields
              |> Array.map(x =>
                   tuple2(string, string, Types__Agendas.Field.toString(x))
                 )
              |> jsonArray,
            ),
            (
              "timerange",
              timerange
              |> Option.flatMap(Result.toOption)
              |> nullable(Time.Encode.make),
            ),
            ("tags", tags |> nullable(array(Filter.Encode.make))),
            ("todos", tags |> nullable(array(Filter.Encode.make))),
            ("reverse", reverse |> nullable(bool)),
          ])
      );
  };

  module Decode = {
    module D = DecodeUtils.D;

    let make = json =>
      D.Pipeline.(
        succeed(Types__Agendas.Agenda.make)
        |> field("id", string)
        |> field(
             "files",
             array(Types__Files__Translators.Identifier.Decode.make),
           )
        |> field("fields", array(tuple2(string, string)))
        |> optionalField("timerange", Time.Decode.make)
        |> optionalField("tags", array(Filter.Decode.make))
        |> optionalField("todos", array(Filter.Decode.make))
        |> optionalField("reverse", boolean)
        |> run(json)
      );
  };
};
